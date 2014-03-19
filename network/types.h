#ifndef CONTROL_TYPES_H
#define CONTROL_TYPES_H
typedef struct node_s
{
    unsigned char mac[5];
    long ip;
    struct node_s *next;
}node_t;

typedef struct offer_timer_s
{
    uv_timer_t *timer;
    struct offer_timer_s *next;
}offer_timer_t;

typedef enum client_state_s
{
    C_START,
    C_SEND,
    C_RECV,
    C_REQUEST,
    C_SET
}client_state_t;

typedef enum server_state_s
{
    S_WAIT,
    S_SENDING,
    S_CANCELED
}server_state_t;

typedef enum packet_s
{
    P_BOOTSTRAP,
    P_OFFER,
    P_REQUEST,
    P_DENY,
    P_BROADCAST,
    P_BYE
}packet_t;

#endif