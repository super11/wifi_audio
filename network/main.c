#include "main.h"

#define INTERFACE "eth0"

#define OFFER_DELAY 10
//OFFER_DELAY is in milliseconds
#define REQUEST_DELAY 1000

char interface[8] = "eth0";

uv_udp_t server;
uv_udp_t client;

uv_udp_send_t server_send_req;
uv_udp_send_t client_send_req;

uv_buf_t send_buf;

client_state_t client_state;
uv_rwlock_t *client_state_rwlock;

node_t *node_head;
//local node is the first element of the node
int node_count;
uv_mutex_t *node_mutex;

offer_timer_t *offer_timer_head;
offer_timer_t *offer_timer_tail;
uv_mutex_t offer_timer_mutex;

uv_timer_t *request_timer;
uv_timer_t *broadcast_timer;

struct sockaddr_in client_recv_addr;
struct sockaddr_in client_send_addr;
struct sockaddr_in server_recv_addr;
struct sockaddr_in server_send_addr;

void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void timer_cb(uv_timer_t* handle, int status)
{
    //handle->data
    uv_close((struct uv_handle_t *)handle);
}

void bootstrap_send_cb(uv_udp_send_t* req, int status)
{
    if (status != 0)
        printf("%s\n", uv_strerror(status));
    assert(status == 0);
    printf("client bootstrap send\n");

    free(send_buf.base);

    uv_rwlock_wrlock(client_state_rwlock);
    if (client_state == C_START)
        client_state = C_SEND;
    uv_rwlock_wrunlock(client_state_rwlock);
}

void server_send_cb(uv_udp_send_t* req, int status)
{
    uv_close((uv_handle_t*)req->handle, NULL);
    if (status != 0)
        printf("%s\n", uv_strerror(status));
    assert(status == 0);
    printf("server sent\n");
}

void server_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
    uv_udp_send_t* req;

    if (nread < 0)
    {
        printf("server recv error");
        return;
    }

    if (nread == 0)
    {
        return;
    }

    uv_mutex_lock(node_mutex);
    if (!memcmp(node_head->mac, &(buf->base[1]), 5)) //ignore packet from self
    {
        uv_mutex_unlock(node_mutex);
        printf("server ignored a packet from self\n");
        free(buf->base);
        return;
    }

    uv_mutex_unlock(node_mutex);
    switch (buf->base[0])
    {
        case P_BOOTSTRAP:
        {
            uv_rwlock_rdlock(client_state_rwlock);
            if (client_state != C_SET)
            {
                uv_rwlock_rdunlock(client_state_rwlock);
                printf("server recv bootstrap, but client not in C_SET");
                return;
            }
            uv_rwlock_rdunlock(client_state_rwlock);

            //search recv mac in offer_timer list
            uv_mutex_lock(offer_timer_mutex);
            if (search_offer_timer(offer_timer_head, &(buf->base[1])) != NULL)
            {
                uv_mutex_unlock(offer_timer_mutex);
                return;
            }
            uv_mutex_unlock(offer_timer_mutex);

            uv_mutex_lock(offer_timer_mutex);
            offer_timer_tail = add_offer_timer(offer_timer_tail, &(buf->base[1]); //add a offer_timer_t in last of offer_timer list

            uv_timer_init(uv_default_loop(), offer_timer_tail->timer);
            //offer_timer_tail->timer->data = malloc(5);
            //memcpy(offer_timer_tail->timer->data, &(buf->base[1], 5); //set mac as data of offer_timer
            uv_timer_start(offer_timer_tail->timer, offer_timer_cb, OFFER_DELAY, 0);
            uv_mutex_unlock(offer_timer_mutex);

            return;
        }
        case P_REQUEST:
        {

            break;
        }
        default:
        {
            printf("server resolve packet error\n");
        }
    }
    
}

void client_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
    if (nread < 0)
    {
        printf("client recv error");
    }

    if (nread == 0)
    {
        return;
    }

    uv_mutex_lock(node_mutex);
    if (memcmp(node_head->mac, &(buf->base[1]), 5)) //recv mac != mac self own,
    {
        uv_mutex_unlock(node_mutex);
        if (buf->base[0] == P_OFFER)
        {
            uv_rwlock_rdlock(client_state_rwlock);
            if (client_state == C_SET) //get a offer from another node
            {
                uv_rwlock_rdunlock(client_state_rwlock);

                uv_mutex_lock(offer_timer_mutex);
                //cancel current offer_timer
                if (search_offer_timer(offer_timer_head, &(buf->base[1])) != NULL) //found mac in offer timer list
                {
                    uv_timer_t *tmp;
                    remove_offer_timer(offer_timer_head, &(buf->base[1]));
                    uv_mutex_unlock(offer_timer_mutex);
                    uv_timer_stop()
                    return;
                }
                uv_mutex_unlock(offer_timer_mutex);
            }
            uv_rwlock_rdunlock(client_state_rwlock);
        }

        printf("client ignored a packet from self\n");
        free(buf->base);
        return;
    }
    

    switch (buf->base[0])
    {
        case P_OFFER:
        { 
            uv_mutex_lock(node_mutex);
            if (memcmp(node_head->mac, &(buf->base[1]), 5)) //memcpy() :0 if the same
            {
                uv_mutex_unlock(node_mutex);
                printf("recv a offer for another node");
                return;
            }

            uv_mutex_unlock(node_mutex);
            uv_rwlock_rdlock(client_state_rwlock);
            if (client_state == C_SEND) //wait for a offer
            {
                uv_rwlock_rdunlock(client_state_rwlock);

                uv_mutex_lock(node_mutex);
                node_head->next = build_node_list(&(buf->base[6]));
                node_head->ip = search_available_ip(node->next);

                send_buf = create_request_packet(node_head);
                uv_mutex_unlock(node_mutex);

                uv_udp_send(client_send_req, &client, &send_buf, 1, (const struct sockaddr *)&client_send_addr, request_send_cb);
            }
            else 
            else
            {
                uv_rwlock_rdunlock(client_state_rwlock);
                return;
            }

            break;
        }
        case P_BROADCAST:
        {
            break;
        }
        case P_BYE:
        {
            break;
        }
        default:
        {
            printf("client resolve packet error\n");
        }
    }
}

int main() 
{
    node_head = malloc(sizeof(node_t *));
    //first node is local node
    mac_head = NULL;

    uv_ip4_addr("0.0.0.0", 5000, &client_recv_addr);
    uv_udp_init(uv_default_loop(), &client);
    uv_udp_bind(&client, (const struct sockaddr*)&client_recv_addr, 0);
    uv_udp_recv_start(&client, alloc_cb, client_recv_cb);
    
    uv_ip4_addr("0.0.0.0", 5002, &server_recv_addr);
    uv_udp_init(uv_default_loop(), &server);
    uv_udp_bind(&server, (const struct sockaddr*)&server_recv_addr, 0);
    uv_udp_recv_start(&server, alloc_cb, server_recv_cb);

    uv_ip4_addr("255.255.255.255", 5002, &client_send_addr);
    uv_udp_set_broadcast(&client, 1);
    
    uv_ip4_addr("255.255.255.255", 5000, &server_send_addr);
    uv_udp_set_broadcast(&server, 1);

    if (get_mac(INTERFACE, node_head->mac) != 0)
    {
        printf("get mac error\n");
        return 1;
    }

    send_buf = create_bootstrap_packet(node_head->mac);
    printf("local mac is:");
    print_mac(&(send_buf.base[1]));

    client_state_rwlock = malloc(sizeof(uv_rwlock_t));
    uv_rwlock_init(client_state_rwlock);
    uv_rwlock_wrlock(client_state_rwlock);
    client_state = C_START;
    uv_rwlock_wrunlock(client_state_rwlock);

    uv_udp_send(&client_send_req, &client, &send_buf, 1, (const struct sockaddr*)&client_send_addr, bootstrap_send_cb);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}

void offer_timer_cb(uv_timer_t *handle, int status)
{
    uv_mutex_lock(node_mutex);
    send_buf = create_offer_packet(node_head, handle->data);
    uv_mutex_unlock(node_mutex);

    //remove this timer from offer_timer list, and free handle->data
    uv_udp_send(server_send_req, &server, &send_buf, 1, (const struct sockaddr *)&server_send_addr, offer_timer_send_cb);
}

void offer_timer_send_cb(uv_udp_send_t *req, int status)
{
    free(send_buf.base);
}

void request_send_cb(uv_udp_send_t *req, int status)
{
    free(send_buf.base)

    uv_rwlock_wrlock(client_state_rwlock);
    client_state = C_REQUEST;
    uv_rwlock_wrunlock(client_state_rwlock);

    request_timer = malloc(uv_timer_t);
    uv_timer_init(uv_default_loop(), request_timer);
    uv_timer_start(request_timer, request_timer_cb, REQUEST_DELAY, 0);
}

void request_timer_cb(uv_timer_t *handle, int status)
{
    uv_rwlock_wrlock(client_state_rwlock);
    client_state = C_SET;
    uv_rwlock_wrunlock(client_state_rwlock);

    uv_mutex_lock(node_mutex);
    set_ip(node_head);
    uv_mutex_unlock(node_mutex);
    //notify audio app

    uv_timer_init(uv_default_loop(), broadcast_timer);
    uv_timer_start(broadcast_timer, broadcast_timer_cb, BROADCAST_DELAY, BROADCAST_REPEAT);
}

void broadcast_timer_cb(uv_timer_t *handle, int status)
{
    send_buf.base = malloc();

}