#include "utils.h"

int get_mac(char *ifr, unsigned char *mac)
{
    struct ifreq interface;
    int sock;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket error\n");
        return -1;
    }

    strncpy(interface.ifr_name,ifr, IF_NAMESIZE);

    if (ioctl(sock, SIOCGIFHWADDR, &interface) < 0)
    {
        printf("ioctl error\n");
        return -1;
    }
    memcpy(mac, (unsigned char *)interface.ifr_hwaddr.sa_data, 5);

    //mac[0] = (unsigned char)interface.ifr_hwaddr.sa_data[0];
    //mac[1] = (unsigned char)interface.ifr_hwaddr.sa_data[1];
    //mac[2] = (unsigned char)interface.ifr_hwaddr.sa_data[2];
    //mac[3] = (unsigned char)interface.ifr_hwaddr.sa_data[3];
    //mac[4] = (unsigned char)interface.ifr_hwaddr.sa_data[4];
    return 0;    
}

void print_mac(unsigned char *mac)
{
    printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

uv_buf_t create_bootstrap_packet(unsigned char *local_mac)
{
    /*len: type    : 1
           mac     : 5
           content : 0  */
    uv_buf_t *buf = malloc(sizeof(uv_buf_t));
    buf->base = malloc(6);
    buf->len = 6;

    buf->base[0] = P_BOOTSTRAP;
    memcpy(&(uf->base[1]), local_mac, 5);
    return buf;
}
uv_buf_t create_offer_packet(unsigned char *remote_mac, node_t *node_head)
{

}
uv_buf_t create_request_packet(node_t *node)
{

}
uv_buf_t create_deny_packet(unsigned char *remote_mac)
{

}
uv_buf_t create_broadcast_packet(node_t *node_head)
{

}
uv_buf_t create_bye_packet(node_t *node_head)
{

}

int add_send_timer(send_timer_t *node, uv_timer_t *timer, unsigned char *mac)
{
    send_timer_t *tmp;
    tmp = seek_send_timer(node, mac, tmp);
    if (seek_send_timer(node, mac, &tmp) == 1) //find mac in send list
    {
        printf("the mac is already in send list")
        return -1;
    }
    //not find
    tmp->next = malloc(sizeof(send_timer_t));
    node = tmp->next;
    node->next = NULL;
    memcpy(node->mac, mac, 5);
    node->timer = timer;
}

int *seek_send_timer(send_timer_t *node, unsigned char *mac, send_timer_t *found)
{
    if (node != NULL) 
    {
        if (!memcmp(node->mac, mac, 5)) //test 1st node
        {
            found = node;  //find it, return 1
            return 1;
        }
        while (node->next != NULL) //seek to the end
        {
            if (!memcmp(node->next->mac, mac, 5)) //if the mac is in the list
            {
                printf("the mac is in the send timer list, return\n");
                found = node;
                return 1;
            }
            else
            {
                node = node->next;
            }
        }
    }
    found = NULL;
    return 0;
}