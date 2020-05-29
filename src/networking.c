// Client can't quite tell if the db is still online. should ping now and then to check

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include "networking.h"
#if (OS_TARGET == OS_LINUX)
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

int networking_start(networking_state_t *state, char *ipv4_addr, uint16_t tcp_port, uint16_t udp_port)
{
    // Load config
    strcpy(state->server_ip_address, ipv4_addr);
    state->tcp_port = tcp_port;
    state->udp_port = udp_port;
    state->timeout_tv.tv_sec = SERVER_TIMEOUT_SECONDS;
    state->timeout_tv.tv_usec = SERVER_TIMEOUT_MICROSECONDS;

    // Setup tcp_serv_addr
    memset((char *)&state->tcp_serv_addr, 0, sizeof(state->tcp_serv_addr));
    state->tcp_serv_addr.sin_family = AF_INET;
    state->tcp_serv_addr.sin_addr.s_addr = inet_addr(state->server_ip_address);
    state->tcp_serv_addr.sin_port = htons(state->tcp_port);

    // Setup udp_serv_addr
    memset((char *)&state->udp_serv_addr, 0, sizeof(state->udp_serv_addr));
    state->udp_serv_addr.sin_family = AF_INET;
    state->udp_serv_addr.sin_addr.s_addr = inet_addr(state->server_ip_address);
    state->udp_serv_addr.sin_port = htons(state->udp_port);

// Initialise winsock (udp only?)
#if (OS_TARGET == OS_WIN)
    if (WSAStartup(MAKEWORD(2, 2), &state->wsa) != 0)
    {
        printf("Winsock startup failed, ");
        networking_print_error();
        return EXIT_FAILURE;
    }
#endif

// Create UDP socket ------------------------------------------------
#if (OS_TARGET == OS_LINUX)
    if ((state->udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
#elif (OS_TARGET == OS_WIN)
    if ((state->udp_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
#endif
    {
        printf("Failed to create UDP socket, ");
        networking_print_error();
        return networking_end(state, EXIT_FAILURE);
    }

// Set UDP timeout
#if (OS_TARGET == OS_LINUX)
    setsockopt(state->udp_sockfd, SOL_SOCKET, SO_RCVTIMEO, &state->timeout_tv, sizeof(state->timeout_tv));
#elif (OS_TARGET == OS_WIN)
    int timeout_win = state->timeout_tv.tv_sec * 1000 + state->timeout_tv.tv_usec / 1000;
    setsockopt(state->udp_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout_win, sizeof(timeout_win));
#endif

    return EXIT_SUCCESS;
}

/*
 * message_len does not include the final 0x00 byte
 * the behaviour of this function is atomic, no real connection is kept,
 * but rather tcp is used to make sure start and end messages arrive to the server
 */
int networking_tcp_write(networking_state_t *state, char *message, size_t message_len)
{
// Create TCP socket ------------------------------------------------
#if (OS_TARGET == OS_LINUX)
    if ((state->tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
#elif (OS_TARGET == OS_WIN)
    if ((state->tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
#endif
    {
        printf("Failed to create TCP socket, ");
        networking_print_error();
        return networking_end(state, EXIT_FAILURE);
    }

    // Connect socket
    if (connect(state->tcp_sockfd, (struct sockaddr *)&state->tcp_serv_addr, sizeof(state->tcp_serv_addr)) < 0) {
        printf("\nTCP connection failed, ");
        networking_print_error();
        return EXIT_FAILURE;
    }

// Send command
#if (OS_TARGET == OS_LINUX)
    if (write(state->tcp_sockfd, message, message_len) < 0)
#elif (OS_TARGET == OS_WIN)
    if (send(state->tcp_sockfd, message, (int)message_len, 0) < 0)
#endif
    {
        printf("Error writing to tcp socket, ");
        networking_print_error();
        return EXIT_FAILURE;
    }

// Close socket
#if (OS_TARGET == OS_LINUX)
    shutdown(state->tcp_sockfd, SHUT_RDWR);
    close(state->tcp_sockfd);
#elif (OS_TARGET == OS_WIN)
    closesocket(state->tcp_sockfd);
#endif

    return EXIT_SUCCESS;
}

/*
 * message_len does not include the final 0x00 byte
 */
int networking_udp_write(networking_state_t *state, char *message, size_t message_len)
{
#if (OS_TARGET == OS_LINUX)
    if (sendto(state->udp_sockfd, message, message_len, 0, (struct sockaddr *)&state->udp_serv_addr, sizeof(state->udp_serv_addr)) < 0)
#elif (OS_TARGET == OS_WIN)
    if (sendto(state->udp_sockfd, message, (int)message_len, 0, (struct sockaddr *)&state->udp_serv_addr, (int)sizeof(state->udp_serv_addr)) == SOCKET_ERROR)
#endif
    {
        printf("Error writing to socket, ");
        networking_print_error();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int networking_udp_read(networking_state_t *state, char *buffer, size_t buffer_len)
{
    // NOTE: not sure if we really want MSG_WAITALL vs 0 vs MSG_DONTWAIT
    int n;
#if (OS_TARGET == OS_LINUX)
    if ((n = recvfrom(state->udp_sockfd, buffer, buffer_len, MSG_WAITALL, NULL, NULL)) < 0)
#elif (OS_TARGET == OS_WIN)
    if ((n = recvfrom(state->udp_sockfd, buffer, (int)buffer_len, MSG_WAITALL, NULL, NULL)) == SOCKET_ERROR)
#endif
    {
#if (OS_TARGET == OS_LINUX)
        if (errno == EWOULDBLOCK)
        {
#elif (OS_TARGET == OS_WIN)
        if (errno == WSAEWOULDBLOCK) {
#endif
            // Socket timeout
            printf("socket timeout, ");
            networking_print_error();
        } else {
            printf("recvfrom error, ");
            networking_print_error();
        }
    }
    return n;
}

int networking_end(networking_state_t *state, int exit_value)
{
#if (OS_TARGET == OS_LINUX)
    close(state->udp_sockfd);
#elif (OS_TARGET == OS_WIN)
    closesocket(state->udp_sockfd);
    WSACleanup();
#endif
    return exit_value;
}

void networking_print_error()
{
#if (OS_TARGET == OS_LINUX)
    printf("errno: %d\n", errno);
#elif (OS_TARGET == OS_WIN)
    printf("winsock err: %d\n", WSAGetLastError());
#endif
}