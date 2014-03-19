#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <uv-unix.h>
#include <uv-linux.h>
#include "types.h"

#ifndef CONTROL_UTILS_H
#define CONTROL_UTILS_H
int get_mac(char *ifr, unsigned char *mac);
void print_mac(unsigned char *mac);
unsigned char *add_mac_t(uv_buf_t *buf);
uv_buf_t create_bootstrap_packet(node_t *node);
uv_buf_t create_offer_packet(node_t *node_head, unsigned char *remote_mac);
uv_buf_t create_request_packet(node_t *node);
uv_buf_t create_deny_packet(unsigned char *remote_mac);
uv_buf_t create_broadcast_packet(node_t *node_head);
uv_buf_t create_bye_packet(node_t *node_head);

int find_send_timer(send_timer_t *node, unsigned char *mac, send_timer_t *found);
int add_send_timer(send_timer_t *node, uv_timer_t *timer, unsigned char *mac);
int del_send_timer(send_timer_t *node, unsigned char *mac);

offer_timer_t *search_offer_timer(offer_timer_t *head, unsigned cahr *mac);
offer_timer_t *add_offer_timer(offer_timer_t *last, unsigned cahr *mac);
offer_timer_t *remove_offer_timer(offer_timer_t *head, unsigned cahr *mac);

long search_available_ip(node_t *head);
node_t *build_node_list(unsigned char *buf);
#endif