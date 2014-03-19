#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "uv.h"
#include "uv-unix.h"
#include "uv-linux.h"

#include "utils.h"
#include "types.h"

#ifndef CONTROL_MAIN_H
#define CONTROL_MAIN_H

void client_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* rcvbuf, const struct sockaddr* addr, unsigned flags);
void server_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* rcvbuf, const struct sockaddr* addr, unsigned flags);
void bootstrap_send_cb(uv_udp_send_t* req, int status);
void client_send_cb(uv_udp_send_t* req, int status);
void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void offer_timer_cb(uv_timer_t *handle, int status);
void offer_timer_send_cb(uv_udp_send_t *req, int status);
void request_send_cb(uv_udp_send_t, int status);
void request_timer_cb(uv_timer_t *handle, int status);
void broadcast_timer_cb(uv_timer_t *handle, int status);

#endif