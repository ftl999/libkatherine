#pragma once

#ifndef WIN32
#include <arpa/inet.h>
#define SOCKETTYPE int
#define CLOSESOCK(sock) close(sock)
#define INITWSDATA()
static inline int TIMEOUT_MAKRO(int sock, const struct timeval * timeout) {
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, timeout, sizeof(*timeout));
}
#define SETTIMEOUT(sock, timeout) TIMEOUT_MAKRO(sock, timeout)
static inline struct timeval MAKETIMEOUTSTRUCT_MARCO(uint32_t ms) {
	struct timeval val;
	val.tv_sec = ms / 1000.f;
	val.tv_usec = 1000 * (ms % 1000);
	return val;
}
#define MAKETIMEOUTSTRUCT(ms)  MAKETIMEOUTSTRUCT_MARCO(ms)

#else

#include "Winsock2.h"
#define SOCKETTYPE SOCKET
#define CLOSESOCK(sock) if(sock > 0) closesocket(sock)

static inline int TIMEOUT_MAKRO(SOCKET sock, uint32_t* timeout) {
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, ((const char*)timeout), sizeof(uint32_t));
}
#define SETTIMEOUT(sock, timeout) TIMEOUT_MAKRO(sock, timeout)
#define MAKETIMEOUTSTRUCT(ms) ms

//Overwritten in udp.c
#define INITWSDATA()
#endif