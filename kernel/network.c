/**
 * Alpha OS - Network Stack Implementation
 * 
 * TCP/IP Networking
 */

#include "../include/kernel/network.h"
#include "../include/kernel/console.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#else
/* Kernel mode - minimal implementations */
#endif

/* Network interfaces */
static network_interface_t* network_ifaces = NULL;
static network_interface_t* primary_iface = NULL;
static socket_t* socket_list = NULL;
static int next_fd = 1;

/* Network statistics */
static network_stats_t net_stats = {0};

/* ============================================================
 * Initialization
 * ============================================================ */

#ifdef TEST_MODE

int network_init(void) {
    printf("[NET] Network stack initialized\n");
    
    /* Create loopback interface */
    network_interface_t* lo = network_interface_create("lo");
    if (lo) {
        uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
        network_interface_set_mac(lo, mac);
        network_interface_set_ip(lo, 0x7F000001);  /* 127.0.0.1 */
        network_interface_set_netmask(lo, 0xFF000000);  /* 255.0.0.0 */
        network_interface_up(lo);
        primary_iface = lo;
    }
    
    return 0;
}

int network_shutdown(void) {
    printf("[NET] Network stack shutting down\n");
    /* Cleanup sockets and interfaces */
    return 0;
}

#else

int network_init(void) {
    return 0;
}

int network_shutdown(void) {
    return 0;
}

#endif

/* ============================================================
 * Interface Management
 * ============================================================ */

network_interface_t* network_interface_create(const char* name) {
    network_interface_t* iface = ((network_interface_t*)malloc(sizeof(network_interface_t)));
    if (!iface) return NULL;
    
    memset(iface, 0, sizeof(network_interface_t));
    strncpy(iface->name, name, 15);
    iface->enabled = false;
    iface->up = false;
    
    /* Add to list */
    iface->next = network_ifaces;
    network_ifaces = iface;
    
    return iface;
}

int network_interface_destroy(network_interface_t* iface) {
    if (!iface) return -1;
    
    /* Remove from list */
    network_interface_t** ptr = &network_ifaces;
    while (*ptr) {
        if (*ptr == iface) {
            *ptr = iface->next;
            break;
        }
        ptr = &(*ptr)->next;
    }
    
    free(iface);
    return 0;
}

int network_interface_set_mac(network_interface_t* iface, const uint8_t* mac) {
    if (!iface || !mac) return -1;
    memcpy(iface->mac, mac, 6);
    return 0;
}

int network_interface_set_ip(network_interface_t* iface, uint32_t ip) {
    if (!iface) return -1;
    iface->ip = ip;
    return 0;
}

int network_interface_set_netmask(network_interface_t* iface, uint32_t mask) {
    if (!iface) return -1;
    iface->netmask = mask;
    return 0;
}

int network_interface_set_gateway(network_interface_t* iface, uint32_t gw) {
    if (!iface) return -1;
    iface->gateway = gw;
    return 0;
}

int network_interface_up(network_interface_t* iface) {
    if (!iface) return -1;
    iface->up = true;
    iface->enabled = true;
    return 0;
}

int network_interface_down(network_interface_t* iface) {
    if (!iface) return -1;
    iface->up = false;
    return 0;
}

network_interface_t* network_interface_get_by_name(const char* name) {
    network_interface_t* iface = network_ifaces;
    while (iface) {
        if (strcmp(iface->name, name) == 0) {
            return iface;
        }
        iface = iface->next;
    }
    return NULL;
}

network_interface_t* network_interface_get_by_ip(uint32_t ip) {
    network_interface_t* iface = network_ifaces;
    while (iface) {
        if (iface->ip == ip) {
            return iface;
        }
        iface = iface->next;
    }
    return NULL;
}

/* ============================================================
 * Packet Handling
 * ============================================================ */

int network_send_packet(network_interface_t* iface, void* data, size_t len) {
    if (!iface || !data || !len) return -1;
    
    if (!iface->up || !iface->send) {
        return -1;
    }
    
    iface->send(iface, data, len);
    net_stats.tx_packets++;
    net_stats.tx_bytes += len;
    
    return len;
}

int network_recv_packet(network_interface_t* iface, void* buf, size_t len) {
    if (!iface || !buf) return -1;
    return 0;  /* Would be implemented with actual driver */
}

void network_process_packet(network_interface_t* iface, void* data, size_t len) {
    if (!iface || !data || len < sizeof(eth_header_t)) return;
    
    eth_header_t* eth = (eth_header_t*)data;
    
    /* Handle by EtherType */
    switch (ntohs(eth->type)) {
        case ETH_TYPE_IP:
            /* Process IP packet */
            break;
        case ETH_TYPE_ARP:
            /* Process ARP packet */
            break;
        default:
            break;
    }
    
    net_stats.rx_packets++;
    net_stats.rx_bytes += len;
}

/* ============================================================
 * IP Utilities
 * ============================================================ */

uint16_t ip_checksum(void* data, size_t len) {
    uint16_t* ptr = (uint16_t*)data;
    uint32_t sum = 0;
    
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    
    if (len) {
        sum += *(uint8_t*)ptr;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

const char* ip_to_string(uint32_t ip) {
    static char buf[16];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d",
             (ip >> 0) & 0xFF,
             (ip >> 8) & 0xFF,
             (ip >> 16) & 0xFF,
             (ip >> 24) & 0xFF);
    return buf;
}

uint32_t string_to_ip(const char* str) {
    uint32_t ip = 0;
    int parts[4] = {0};
    
    if (sscanf(str, "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]) == 4) {
        ip = (parts[0] | (parts[1] << 8) | (parts[2] << 16) | (parts[3] << 24));
    }
    
    return ip;
}

/* ============================================================
 * Socket API
 * ============================================================ */

int socket_init(void) {
    socket_list = NULL;
    next_fd = 1;
    return 0;
}

socket_t* socket(int domain, int type, int protocol) {
    if (domain != AF_INET) return NULL;
    
    socket_t* sock = ((socket_t*)malloc(sizeof(socket_t)));
    if (!sock) return NULL;
    
    memset(sock, 0, sizeof(socket_t));
    sock->fd = next_fd++;
    sock->family = domain;
    sock->type = type;
    sock->protocol = protocol;
    sock->state = SOCK_STATE_CLOSED;
    
    /* Add to socket list */
    sock->next = socket_list;
    socket_list = sock;
    
    return sock;
}

int bind(socket_t* sock, uint32_t ip, uint16_t port) {
    if (!sock) return -1;
    
    sock->local_ip = ip;
    sock->local_port = port;
    
    return 0;
}

int close(socket_t* sock) {
    if (!sock) return -1;
    
    /* Remove from list */
    socket_t** ptr = &socket_list;
    while (*ptr) {
        if (*ptr == sock) {
            *ptr = sock->next;
            break;
        }
        ptr = &(*ptr)->next;
    }
    
    free(sock);
    return 0;
}

socket_t* socket_get_by_fd(int fd) {
    socket_t* sock = socket_list;
    while (sock) {
        if (sock->fd == fd) {
            return sock;
        }
        sock = sock->next;
    }
    return NULL;
}

int socket_get_free_fd(void) {
    return next_fd++;
}

/* UDP stubs */
int sendto(socket_t* sock, const void* buf, size_t len, uint32_t dst_ip, uint16_t dst_port) {
    (void)sock; (void)buf; (void)len; (void)dst_ip; (void)dst_port;
    return len;
}

int recvfrom(socket_t* sock, void* buf, size_t len, uint32_t* src_ip, uint16_t* src_port) {
    (void)sock; (void)buf; (void)len; (void)src_ip; (void)src_port;
    return 0;
}

/* TCP stubs */
int listen(socket_t* sock, int backlog) {
    (void)sock; (void)backlog;
    return 0;
}

socket_t* accept(socket_t* sock) {
    (void)sock;
    return NULL;
}

int connect(socket_t* sock, uint32_t ip, uint16_t port) {
    if (!sock) return -1;
    sock->remote_ip = ip;
    sock->remote_port = port;
    sock->state = SOCK_STATE_ESTABLISHED;
    return 0;
}

int send(socket_t* sock, const void* buf, size_t len) {
    (void)sock; (void)buf;
    return len;
}

int recv(socket_t* sock, void* buf, size_t len) {
    (void)sock; (void)buf;
    return 0;
}

/* ============================================================
 * Statistics
 * ============================================================ */

void network_get_stats(network_stats_t* stats) {
    if (stats) {
        memcpy(stats, &net_stats, sizeof(network_stats_t));
    }
}

void network_reset_stats(void) {
    memset(&net_stats, 0, sizeof(network_stats_t));
}

/* ARP stubs */
int arp_init(void) { return 0; }
int arp_resolve(network_interface_t* iface, uint32_t ip, uint8_t* mac) {
    (void)iface; (void)ip; (void)mac;
    return -1;
}
int arp_add_entry(uint32_t ip, const uint8_t* mac) {
    (void)ip; (void)mac;
    return 0;
}

/* IP send stub */
int ip_send(network_interface_t* iface, uint32_t dst_ip, uint8_t proto, void* data, size_t len) {
    (void)iface; (void)dst_ip; (void)proto; (void)data; (void)len;
    return 0;
}

/* ICMP stubs */
int icmp_init(void) { return 0; }
int icmp_send_echo_reply(network_interface_t* iface, uint32_t src_ip, uint16_t id, uint16_t seq) {
    (void)iface; (void)src_ip; (void)id; (void)seq;
    return 0;
}
