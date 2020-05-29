#pragma once

#include "config.h"
#if (OS_TARGET == OS_WIN)
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  // Winsock library
#elif (OS_TARGET == OS_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#endif

typedef struct
{
#if (OS_TARGET == OS_LINUX)
    int udp_sockfd, tcp_sockfd;
#elif (OS_TARGET == OS_WIN)
    SOCKET udp_sockfd, tcp_sockfd;
    WSADATA wsa;
#endif
    struct sockaddr_in tcp_serv_addr, udp_serv_addr;
    struct timeval timeout_tv;
    char server_ip_address[16];
    uint16_t tcp_port;
    uint16_t udp_port;
} networking_state_t;

int networking_start(
    networking_state_t *state,
    char *ipv4_addr,
    uint16_t tcp_port,
    uint16_t udp_port);

int networking_tcp_write(
    networking_state_t *state,
    char *message,
    size_t message_len);

int networking_udp_write(
    networking_state_t *state,
    char *message,
    size_t message_len);

int networking_udp_read(
    networking_state_t *state,
    char *buffer,
    size_t buffer_len);

int networking_end(
    networking_state_t *state,
    int exit_value);

void networking_print_error();

#define SERVER_TIMEOUT_SECONDS 0           // Depends on network performance. NOTE: may want to ping and estimate on startup
#define SERVER_TIMEOUT_MICROSECONDS 500000 // 500ms