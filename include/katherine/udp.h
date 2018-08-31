//
// Created by petr on 29.5.18.
//

#ifndef THESIS_UDP_H
#define THESIS_UDP_H

/**
 * @file
 * @brief Functions related to the UDP communication layer.
 */

#include <stdio.h>
#ifndef WIN32
 #include <arpa/inet.h>
 #include <pthread.h>
#else
  #include "Winsock2.h"
  #include <Windows.h>
  #include <stdint.h>
  #pragma comment(lib, "Ws2_32.lib")  //Link against Ws2_32.lib
#endif

#include "MultiplattformNetwork.h"
#include "MultiplattformTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct katherine_udp {
	SOCKETTYPE sock;
    struct sockaddr_in addr_local;
    struct sockaddr_in addr_remote;

	MUTEXTYPE mutex;
} katherine_udp_t;

int
katherine_udp_init(katherine_udp_t *u, uint16_t local_port, const char *remote_addr, uint16_t remote_port, uint32_t timeout_ms);

void
katherine_udp_fini(katherine_udp_t * u);

int
katherine_udp_send_exact(katherine_udp_t* u, const void* data, size_t count);

int
katherine_udp_recv_exact(katherine_udp_t* u, void*, size_t count);

int
katherine_udp_recv(katherine_udp_t* u, void*, size_t* count);

int
katherine_udp_mutex_lock(katherine_udp_t* u);

int
katherine_udp_mutex_unlock(katherine_udp_t* u);

#ifdef __cplusplus
}
#endif

#endif //THESIS_UDP_H
