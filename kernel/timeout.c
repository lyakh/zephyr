/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <spinlock.h>
#include <ksched.h>
#include <timeout_q.h>
#include <syscall_handler.h>
#include <drivers/timer/system_timer.h>
#include <sys_clock.h>

#define LOCKED(lck) for (k_spinlock_key_t __i = {},			\
					  __key = k_spin_lock(lck);	\
			__i.key == 0;					\
			k_spin_unlock(lck, __key), __i.key = 1)

static u64_t curr_tick;

static sys_dlist_t timeout_list = SYS_DLIST_STATIC_INIT(&timeout_list);

static struct k_spinlock timeout_lock;

#define MAX_WAIT (IS_ENABLED(CONFIG_SYSTEM_CLOCK_SLOPPY_IDLE) \
		  ? K_TICKS_FOREVER : INT_MAX)

/* Cycles left to process in the currently-executing z_clock_announce() */
static int announce_remaining;

#if defined(CONFIG_TIMER_READS_ITS_FREQUENCY_AT_RUNTIME)
int z_clock_hw_cycles_per_sec = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC;

#ifdef CONFIG_USERSPACE
static inline int z_vrfy_z_clock_hw_cycles_per_sec_runtime_get(void)
{
	return z_impl_z_clock_hw_cycles_per_sec_runtime_get();
}
#include <syscalls/z_clock_hw_cycles_per_sec_runtime_get_mrsh.c>
#endif /* CONFIG_USERSPACE */
#endif /* CONFIG_TIMER_READS_ITS_FREQUENCY_AT_RUNTIME */

static struct _timeout *first(void)
{
	sys_dnode_t *t = sys_dlist_peek_head(&timeout_list);

	return t == NULL ? NULL : CONTAINER_OF(t, struct _timeout, node);
}

static struct _timeout *next(struct _timeout *t)
{
	sys_dnode_t *n = sys_dlist_peek_next(&timeout_list, &t->node);

	return n == NULL ? NULL : CONTAINER_OF(n, struct _timeout, node);
}

static void remove_timeout(struct _timeout *t)
{
	if (next(t) != NULL) {
		next(t)->dticks += t->dticks;
	}

	sys_dlist_remove(&t->node);
}

static s32_t elapsed(void)
{
	return announce_remaining == 0 ? z_clock_elapsed() : 0;
}

static s32_t next_timeout(void)
{
	struct _timeout *to = first();
	s32_t ticks_elapsed = elapsed();
	s32_t ret = to == NULL ? MAX_WAIT : MAX(0, to->dticks - ticks_elapsed);

#ifdef CONFIG_TIMESLICING
	if (_current_cpu->slice_ticks && _current_cpu->slice_ticks < ret) {
		ret = _current_cpu->slice_ticks;
	}
#endif
	return ret;
}

void z_add_timeout(struct _timeout *to, _timeout_func_t fn,
		   k_timeout_t timeout)
{
#ifdef CONFIG_LEGACY_TIMEOUT_API
	k_ticks_t ticks = timeout;
#else
	k_ticks_t ticks = timeout.ticks + 1;
#endif

	__ASSERT(!sys_dnode_is_linked(&to->node), "");
	to->fn = fn;
	ticks = MAX(1, ticks);

	LOCKED(&timeout_lock) {
		struct _timeout *t;

		to->dticks = ticks + elapsed();
		for (t = first(); t != NULL; t = next(t)) {
			__ASSERT(t->dticks >= 0, "");

			if (t->dticks > to->dticks) {
				t->dticks -= to->dticks;
				sys_dlist_insert(&t->node, &to->node);
				break;
			}
			to->dticks -= t->dticks;
		}

		if (t == NULL) {
			sys_dlist_append(&timeout_list, &to->node);
		}

		if (to == first()) {
			z_clock_set_timeout(next_timeout(), false);
		}
	}
}

int z_abort_timeout(struct _timeout *to)
{
	int ret = -EINVAL;

	LOCKED(&timeout_lock) {
		if (sys_dnode_is_linked(&to->node)) {
			remove_timeout(to);
			ret = 0;
		}
	}

	return ret;
}

s32_t z_timeout_remaining(struct _timeout *timeout)
{
	s32_t ticks = 0;

	if (z_is_inactive_timeout(timeout)) {
		return 0;
	}

	LOCKED(&timeout_lock) {
		for (struct _timeout *t = first(); t != NULL; t = next(t)) {
			ticks += t->dticks;
			if (timeout == t) {
				break;
			}
		}
	}

	return ticks - elapsed();
}

s32_t z_get_next_timeout_expiry(void)
{
	s32_t ret = K_TICKS_FOREVER;

	LOCKED(&timeout_lock) {
		ret = next_timeout();
	}
	return ret;
}

void z_set_timeout_expiry(s32_t ticks, bool idle)
{
	LOCKED(&timeout_lock) {
		int next = next_timeout();
		bool sooner = (next == K_TICKS_FOREVER) || (ticks < next);
		bool imminent = next <= 1;

		/* Only set new timeouts when they are sooner than
		 * what we have.  Also don't try to set a timeout when
		 * one is about to expire: drivers have internal logic
		 * that will bump the timeout to the "next" tick if
		 * it's not considered to be settable as directed.
		 * SMP can't use this optimization though: we don't
		 * know when context switches happen until interrupt
		 * exit and so can't get the timeslicing clamp folded
		 * in.
		 */
		if (!imminent && (sooner || IS_ENABLED(CONFIG_SMP))) {
			z_clock_set_timeout(ticks, idle);
		}
	}
}

void z_clock_announce(s32_t ticks)
{
#ifdef CONFIG_TIMESLICING
	z_time_slice(ticks);
#endif

	k_spinlock_key_t key = k_spin_lock(&timeout_lock);

	announce_remaining = ticks;

	while (first() != NULL && first()->dticks <= announce_remaining) {
		struct _timeout *t = first();
		int dt = t->dticks;

		curr_tick += dt;
		announce_remaining -= dt;
		t->dticks = 0;
		remove_timeout(t);

		k_spin_unlock(&timeout_lock, key);
		t->fn(t);
		key = k_spin_lock(&timeout_lock);
	}

	if (first() != NULL) {
		first()->dticks -= announce_remaining;
	}

	curr_tick += announce_remaining;
	announce_remaining = 0;

	z_clock_set_timeout(next_timeout(), false);

	k_spin_unlock(&timeout_lock, key);
}

s64_t z_tick_get(void)
{
	u64_t t = 0U;

	LOCKED(&timeout_lock) {
		t = curr_tick + z_clock_elapsed();
	}
	return t;
}

u32_t z_tick_get_32(void)
{
#ifdef CONFIG_TICKLESS_KERNEL
	return (u32_t)z_tick_get();
#else
	return (u32_t)curr_tick;
#endif
}

s64_t z_impl_k_uptime_get(void)
{
	return k_ticks_to_ms_floor64(z_tick_get());
}

#ifdef CONFIG_USERSPACE
static inline s64_t z_vrfy_k_uptime_get(void)
{
	return z_impl_k_uptime_get();
}
#include <syscalls/k_uptime_get_mrsh.c>
#endif

/* Returns the uptime expiration (relative to an unlocked "now"!) of a
 * timeout object.
 */
u64_t z_timeout_end_calc(k_timeout_t timeout)
{
	k_ticks_t dt;

	if (K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		return UINT64_MAX;
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		return z_tick_get();
	}

#ifdef CONFIG_LEGACY_TIMEOUT_API
	dt = k_ms_to_ticks_ceil32(timeout);
#else
	dt = timeout.ticks;
#endif
	return z_tick_get() + MAX(1, dt);
}
