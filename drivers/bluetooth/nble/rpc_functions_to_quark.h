/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* declare the list of functions sorted by signature */
#define LIST_FN_SIG_NONE

#define LIST_FN_SIG_S						\
	FN_SIG_S(on_nble_get_version_rsp,			\
		 const struct nble_get_version_rsp *)		\
	FN_SIG_S(on_nble_gap_connect_evt,			\
		 const struct nble_gap_connect_evt *)		\
	FN_SIG_S(on_nble_gap_disconnect_evt,			\
		 const struct nble_gap_disconnect_evt *)	\
	FN_SIG_S(on_nble_gap_conn_update_evt,			\
		 const struct nble_gap_conn_update_evt *)	\
	FN_SIG_S(on_nble_sm_status_evt,				\
		 const struct nble_sm_status_evt *)		\
	FN_SIG_S(on_nble_sm_passkey_disp_evt,			\
		 const struct nble_sm_passkey_disp_evt *)	\
	FN_SIG_S(on_nble_sm_passkey_req_evt,			\
		 const struct nble_sm_passkey_req_evt *)	\
	FN_SIG_S(on_nble_gap_rssi_evt,				\
		 const struct nble_gap_rssi_evt *)		\
	FN_SIG_S(on_nble_common_rsp,				\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_gap_connect_rsp,			\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_gap_cancel_connect_rsp,		\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_get_bda_rsp,				\
		 const struct nble_get_bda_rsp *)		\
	FN_SIG_S(on_nble_sm_config_rsp,				\
		 struct nble_sm_config_rsp *)			\
	FN_SIG_S(on_nble_sm_common_rsp,				\
		 const struct nble_sm_common_rsp *)		\
	FN_SIG_S(on_nble_set_bda_rsp,				\
		 const struct nble_set_bda_rsp *)		\
	FN_SIG_S(on_nble_gap_set_rssi_report_rsp,		\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_dbg_rsp,				\
		 const struct nble_dbg_rsp *)			\
	FN_SIG_S(on_nble_gap_dir_adv_timeout_evt,		\
		 const struct nble_gap_dir_adv_timeout_evt *)	\
	FN_SIG_S(on_nble_gatts_notify_tx_evt,			\
		 const struct nble_gatts_notify_tx_evt *)	\
	FN_SIG_S(on_nble_gatts_indicate_rsp,			\
		 const struct nble_gatts_indicate_rsp *)	\
	FN_SIG_S(on_nble_gap_start_adv_rsp,			\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_gap_scan_start_stop_rsp,		\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_gatts_read_evt,			\
		 const struct nble_gatts_read_evt *)		\
	FN_SIG_S(on_nble_gatts_write_exec_evt,			\
		 const struct nble_gatts_write_exec_evt *)	\
	FN_SIG_S(on_nble_uas_bucket_change,			\
		 const struct nble_uas_bucket_change *)		\
	FN_SIG_S(on_nble_gattc_write_rsp,			\
		 const struct nble_gattc_write_rsp *)		\
	FN_SIG_S(on_nble_gap_set_tx_power_rsp,			\
		 const struct nble_common_rsp *)		\
	FN_SIG_S(on_nble_sm_pairing_request_evt,		\
		 const struct nble_sm_pairing_request_evt *)	\
	FN_SIG_S(on_nble_sm_security_request_evt,		\
		 const struct nble_sm_security_request_evt *)


#define LIST_FN_SIG_P						\
	FN_SIG_P(on_nble_gap_dtm_init_rsp, void *)

#define LIST_FN_SIG_S_B						\
	FN_SIG_S_B(nble_log, const struct nble_log_s *, char *,	\
		   u8_t)					\
	FN_SIG_S_B(on_nble_gattc_value_evt,			\
		   const struct nble_gattc_value_evt *,		\
		   u8_t *, u8_t)				\
	FN_SIG_S_B(on_nble_gatts_write_evt,			\
		   const struct nble_gatts_write_evt *,		\
		   const u8_t *, u8_t)			\
	FN_SIG_S_B(on_nble_gatts_register_rsp,			\
		   const struct nble_gatts_register_rsp *,	\
		   const struct nble_gatt_attr_handles *,	\
		   u8_t)					\
	FN_SIG_S_B(on_nble_gattc_discover_rsp,			\
		   const struct nble_gattc_discover_rsp *,	\
		   const u8_t *, u8_t)			\
	FN_SIG_S_B(on_nble_gap_adv_report_evt,			\
		   const struct nble_gap_adv_report_evt *,	\
		   const u8_t *, u8_t)			\
	FN_SIG_S_B(on_nble_sm_bond_info_rsp,			\
		   const struct nble_sm_bond_info_rsp *,	\
		   const bt_addr_le_t *, u16_t)		\
	FN_SIG_S_B(on_nble_gattc_read_rsp,			\
		   const struct nble_gattc_read_rsp *,		\
		   u8_t *, u8_t)				\
	FN_SIG_S_B(on_nble_gattc_read_multi_rsp,		\
		   const struct nble_gattc_read_rsp *,		\
		   u8_t *, u8_t)				\
	FN_SIG_S_B(on_nble_uart_test_evt,			\
		   const struct nble_uart_test_evt *,		\
		   const u8_t *, u8_t)

#define LIST_FN_SIG_B_B_P

#define LIST_FN_SIG_S_P

#define LIST_FN_SIG_S_B_P

#define LIST_FN_SIG_S_B_B_P
