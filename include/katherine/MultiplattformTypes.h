#pragma once

#ifndef WIN32
#include <pthread.h>
#define MUTEXTYPE pthread_mutex_t
#define CREATEMUTEX(mux) mux = PTHREAD_MUTEX_INITIALIZER
#define DESTROYMUTEX(mux) pthread_mutex_destroy(&mux)
#define IS_MUTEX_VALID(mux) mux != NULL
#define ACQUIRE_MUTEX(mux) pthread_mutex_lock(&mux);
#define RELEASE_MUTEX(mux) pthread_mutex_unlock(&mux)

#define PACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#include <Windows.h>
#define MUTEXTYPE HANDLE
#define IS_MUTEX_VALID(mux) ((mux != NULL) && (mux != INVALID_HANDLE_VALUE))
#define CREATEMUTEX(mux) mux = CreateMutex(NULL, FALSE, NULL)
#define DESTROYMUTEX(mux) if(IS_MUTEX_VALID(mux)) CloseHandle(mux)
#define ACQUIRE_MUTEX(mux) WaitForSingleObject(mux, INFINITE);
#define RELEASE_MUTEX(mux) ReleaseMutex(mux)

#define PACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif