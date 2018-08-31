//
// Created by petr on 29.5.18.
//

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#else
#include "unistd.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sys/types.h>

WSADATA wsaData;
bool WSDataInitialized = false;
#define INITWSDATA() if(!WSDataInitialized) { WSAStartup(MAKEWORD(2, 2), &wsaData);  WSDataInitialized = true; }

#endif
#include <string.h>
#include <katherine/udp.h>

/**
 * Initialize new UDP session.
 * @param u UDP session to initialize
 * @param local_port Local port number
 * @param remote_addr Remote IP address
 * @param remote_port Remote port number
 * @param timeout_ms Communication timeout in milliseconds (zero if disabled)
 * @return Error code.
 */
int
katherine_udp_init(katherine_udp_t *u, uint16_t local_port, const char *remote_addr, uint16_t remote_port, uint32_t timeout_ms)
{
	INITWSDATA();
    // Create socket.
    if ((u->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
        goto err_socket;
    }

    // Setup and bind the socket address.
    u->addr_local.sin_family = AF_INET;
    u->addr_local.sin_port = htons(local_port);
    u->addr_local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(u->sock, (struct sockaddr*) &u->addr_local, sizeof(u->addr_local)) == -1) {
        goto err_bind;
    }

    if (timeout_ms > 0) {
        // Set socket timeout.
        if (SETTIMEOUT(u->sock, &MAKETIMEOUTSTRUCT(timeout_ms)) < 0) {
            goto err_timeout;
        }
    }

    // Set remote socket address.
    u->addr_remote.sin_family = AF_INET;
    u->addr_remote.sin_port = htons(remote_port);
    if (inet_pton(AF_INET, remote_addr, &u->addr_remote.sin_addr) <= 0) {
        goto err_remote;
    }

	CREATEMUTEX(u->mutex);
    if (!IS_MUTEX_VALID(u->mutex)) {
        goto err_mutex;
    }

    return 0;

err_mutex:
err_remote:
err_timeout:
err_bind:
    CLOSESOCK(u->sock);
err_socket:
	u->sock = -1;
    return 1;
}

/**
 * Finalize UDP session.
 * @param u UDP session to finalize
 */
void
katherine_udp_fini(katherine_udp_t *u)
{
	CLOSESOCK(u->sock);

	// Ignoring return code below.

//	DESTROYMUTEX(u->mutex);
}

/**
 * Send a message (unreliable).
 * @param u UDP session
 * @param data Message start
 * @param count Message length in bytes
 * @return Error code.
 */
int
katherine_udp_send_exact(katherine_udp_t* u, const void* data, size_t count)
{
    ssize_t sent;
    size_t total = 0;
    do {
        sent = sendto(u->sock, ((const char*)data) + total, count - total, 0, (struct sockaddr*) &u->addr_remote, sizeof(u->addr_remote));
        if (sent == -1) {
            return 1;
        }

        total += sent;
    } while (total < count);

    return 0;
}

/**
 * Receive a message (unreliable).
 * @param u UDP session
 * @param data Inbound buffer start
 * @param count Inbound buffer size in bytes
 * @return Error code.
 */
int
katherine_udp_recv_exact(katherine_udp_t* u, void* data, size_t count)
{
    ssize_t received;
    size_t total = 0;
    socklen_t addr_len = sizeof(u->addr_remote);

    while (total < count) {
        received = recvfrom(u->sock, ((char*)data) + total, count - total, 0, (struct sockaddr *) &u->addr_remote, &addr_len);
        if (received == -1) {
            return 1;
        }

        total += received;
    }

    return 0;
}

/**
 * Receive a portion of a message (unreliable).
 * @param u UDP session
 * @param data Inbound buffer start
 * @param count Inbound buffer size in bytes
 * @return Error code.
 */
int
katherine_udp_recv(katherine_udp_t* u, void* data, size_t* count)
{
    socklen_t addr_len = sizeof(u->addr_remote);
    ssize_t received = recvfrom(u->sock, data, *count, 0, (struct sockaddr *) &u->addr_remote, &addr_len);

    if (received == -1) {
        return 1;
    }

    *count = (size_t) received;
    return 0;
}

/**
 * Lock mutual exclusion synchronization primitive.
 * @param u UDP session
 * @return Error code.
 */
int
katherine_udp_mutex_lock(katherine_udp_t *u)
{
    return ACQUIRE_MUTEX(u->mutex);
}

/**
 * Unlock mutual exclusion synchronization primitive.
 * @param u UDP session
 * @return Error code.
 */
int
katherine_udp_mutex_unlock(katherine_udp_t *u)
{
    return RELEASE_MUTEX(u->mutex);
}
