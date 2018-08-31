#pragma once

#ifndef WIN32
#include <arpa/inet.h>
#include <pthread.h>
#define MUTEXTYPE pthread_mutex_t
#define CREATEMUTEX(mux) mux = PTHREAD_MUTEX_INITIALIZER
#define DESTROYMUTEX(mux) pthread_mutex_destroy(&mux)
#define IS_MUTEX_VALID(mux) mux != NULL
#define ACQUIRE_MUTEX(mux) pthread_mutex_lock(&mux);
#define RELEASE_MUTEX(mux) pthread_mutex_unlock(&mux)
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
#include <Windows.h>
#define MUTEXTYPE HANDLE
#define IS_MUTEX_VALID(mux) ((mux != NULL) && (mux != INVALID_HANDLE_VALUE))
#define CREATEMUTEX(mux) mux = CreateMutex(NULL, FALSE, NULL)
#define DESTROYMUTEX(mux) if(IS_MUTEX_VALID(mux)) CloseHandle(mux)
#define ACQUIRE_MUTEX(mux) WaitForSingleObject(mux, INFINITE);
#define RELEASE_MUTEX(mux) ReleaseMutex(mux)
#define SOCKETTYPE SOCKET
#define CLOSESOCK(sock) if(sock > 0) closesocket(sock)

static inline int TIMEOUT_MAKRO(SOCKET sock, uint32_t* timeout) {
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, ((const char*)timeout), sizeof(uint32_t));
}
#define SETTIMEOUT(sock, timeout) TIMEOUT_MAKRO(sock, timeout)
#define MAKETIMEOUTSTRUCT(ms) ms
#endif

#ifndef WIN32
#define PACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#define PACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif