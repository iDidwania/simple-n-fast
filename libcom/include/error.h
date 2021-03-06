#ifndef _SNF_ERROR_H_
#define _SNF_ERROR_H_

#define	E_ok                    0
#define E_invalid_arg           -1
#define E_invalid_state         -2
#define E_not_found             -3
#define E_open_failed           -4
#define E_close_failed          -5
#define E_read_failed           -6
#define E_write_failed          -7
#define E_seek_failed           -8
#define E_sync_failed           -9
#define E_stat_failed           -10
#define E_trunc_failed          -11
#define E_lock_failed           -12
#define E_unlock_failed         -13
#define E_try_again             -14
#define E_mkdir_failed          -15
#define E_rename_failed         -16
#define E_remove_failed         -17
#define E_syscall_failed        -18
#define E_load_failed           -19
#define E_unload_failed         -20
#define E_eof_detected          -21
#define E_no_memory             -22
#define E_xlate_failed          -23
#define E_insufficient_buffer   -24
#define E_connect_failed        -25
#define E_connection_reset      -26
#define E_accept_failed         -27
#define E_bind_failed           -28
#define E_broken_pipe           -29
#define E_timed_out             -30
#define E_ssl_error             -31

#endif // _SNF_ERROR_H_
