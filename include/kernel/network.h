/**
 * Alpha OS - Network Stack Header
 * 
 * TCP/IP Networking Implementation
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "../kernel/types.h"

/* ============================================================
 * Network Configuration
 * ============================================================ */

#define NETWORK_ENABLED        1
#define NETWORK_BUFFER_SIZE   2048
#define MAX_NETWORK_IFACES   4

/* ============================================================
 * Ethernet
 * ============================================================ */

#define ETH_TYPE_IP    0x0800
#define ETH_TYPE_ARP   0x0806

/* Ethernet header */
typedef struct {
    uint8_t  dst[6];
    uint8_t  src[6];
    uint16_t type;
} __attribute__((packed)) eth_header_t;

/* ============================================================
 * ARP
 * ============================================================ */

#define ARP_OP_REQUEST  1
#define ARP_OP_REPLY    2

#define ARP_TYPE_ETH    0x0100

typedef struct {
    uint16_t htype;       /* Hardware type */
    uint16_t ptype;       /* Protocol type */
    uint8_t  hlen;        /* Hardware address length */
    uint8_t  plen;        /* Protocol address length */
    uint16_t oper;        /* Operation */
    uint8_t  sha[6];      /* Sender hardware address */
    uint8_t  spa[4];      /* Sender protocol address */
    uint8_t  tha[6];      /* Target hardware address */
    uint8_t  tpa[4];      /* Target protocol address */
} __attribute__((packed)) arp_packet_t;

/* ============================================================
 * IP
 * ============================================================ */

#define IP_VERSION_4    4
#define IP_PROTO_ICMP   1
#define IP_PROTO_TCP    6
#define IP_PROTO_UDP    17

typedef struct {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t length;
    uint16_t id;
    uint16_t flags_frag;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed)) ip_header_t;

/* ============================================================
 * ICMP
 * ============================================================ */

#define ICMP_TYPE_ECHO_REPLY   0
#define ICMP_TYPE_ECHO_REQUEST 8

typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint32_t rest;
} __attribute__((packed)) icmp_header_t;

/* ============================================================
 * UDP
 * ============================================================ */

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;

/* ============================================================
 * TCP
 * ============================================================ */

#define TCP_FLAG_FIN  0x01
#define TCP_FLAG_SYN  0x02
#define TCP_FLAG_RST  0x04
#define TCP_FLAG_PSH  0x08
#define TCP_FLAG_ACK  0x10
#define TCP_FLAG_URG  0x20

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t  data_offset;
    uint8_t  flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
} __attribute__((packed)) tcp_header_t;

/* ============================================================
 * Network Interface
 * ============================================================ */

typedef struct network_interface {
    char     name[16];
    uint8_t  mac[6];
    uint32_t ip;
    uint32_t netmask;
    uint32_t gateway;
    bool     enabled;
    bool     up;
    
    /* Callbacks */
    void (*send)(struct network_interface* iface, void* packet, size_t len);
    void (*recv)(struct network_interface* iface, void* packet, size_t len);
    
    struct network_interface* next;
} network_interface_t;

/* ============================================================
 * Socket Types
 * ============================================================ */

#define SOCK_STREAM  1  /* TCP */
#define SOCK_DGRAM   2  /* UDP */
#define SOCK_RAW     3  /* Raw */

#define AF_INET      2   /* IPv4 */

/* Socket states */
#define SOCK_STATE_CLOSED      0
#define SOCK_STATE_LISTEN      1
#define SOCK_STATE_ESTABLISHED 2
#define SOCK_STATE_CLOSE_WAIT  3

typedef struct socket {
    int      fd;
    int      family;
    int      type;
    int      protocol;
    int      state;
    
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;
    
    uint32_t seq_num;
    uint32_t ack_num;
    
    void*    buffer;
    size_t   buffer_len;
    
    struct socket* next;
} socket_t;

/* ============================================================
 * Network Statistics
 * ============================================================ */

typedef struct {
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_errors;
    uint64_t tx_errors;
    uint64_t dropped;
} network_stats_t;

/* ============================================================
 * API Functions
 * ============================================================ */

/* Initialization */
int network_init(void);
int network_shutdown(void);

/* Interface management */
network_interface_t* network_interface_create(const char* name);
int network_interface_destroy(network_interface_t* iface);
int network_interface_set_mac(network_interface_t* iface, const uint8_t* mac);
int network_interface_set_ip(network_interface_t* iface, uint32_t ip);
int network_interface_set_netmask(network_interface_t* iface, uint32_t mask);
int network_interface_set_gateway(network_interface_t* iface, uint32_t gw);
int network_interface_up(network_interface_t* iface);
int network_interface_down(network_interface_t* iface);
network_interface_t* network_interface_get_by_name(const char* name);
network_interface_t* network_interface_get_by_ip(uint32_t ip);

/* Packet handling */
int network_send_packet(network_interface_t* iface, void* data, size_t len);
int network_recv_packet(network_interface_t* iface, void* buf, size_t len);
void network_process_packet(network_interface_t* iface, void* data, size_t len);

/* ARP */
int arp_init(void);
int arp_resolve(network_interface_t* iface, uint32_t ip, uint8_t* mac);
int arp_add_entry(uint32_t ip, const uint8_t* mac);

/* IP */
uint16_t ip_checksum(void* data, size_t len);
int ip_send(network_interface_t* iface, uint32_t dst_ip, uint8_t proto, void* data, size_t len);

/* ICMP */
int icmp_init(void);
int icmp_send_echo_reply(network_interface_t* iface, uint32_t src_ip, uint16_t id, uint16_t seq);

/* UDP */
socket_t* socket(int domain, int type, int protocol);
int bind(socket_t* sock, uint32_t ip, uint16_t port);
int sendto(socket_t* sock, const void* buf, size_t len, uint32_t dst_ip, uint16_t dst_port);
int recvfrom(socket_t* sock, void* buf, size_t len, uint32_t* src_ip, uint16_t* src_port);
int close(socket_t* sock);

/* TCP */
int listen(socket_t* sock, int backlog);
socket_t* accept(socket_t* sock);
int connect(socket_t* sock, uint32_t ip, uint16_t port);
int send(socket_t* sock, const void* buf, size_t len);
int recv(socket_t* sock, void* buf, size_t len);

/* Socket API */
int socket_init(void);
socket_t* socket_get_by_fd(int fd);
int socket_get_free_fd(void);

/* Statistics */
void network_get_stats(network_stats_t* stats);
void network_reset_stats(void);

/* Utility */
const char* ip_to_string(uint32_t ip);
uint32_t string_to_ip(const char* str);

#endif /* NETWORK_H */
