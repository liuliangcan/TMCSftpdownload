#pragma once

#ifndef _MACRO_H_
#define _MACRO_H_

//多线程处理模块宏定义
#include <stdio.h>
#ifdef _WIN32
#include <winsock.h>
#define socklen_t int
#else
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <linux/unistd.h>
#define closesocket(_x) close(_x)	
#endif

#ifdef _WIN32
#define THREAD_PROC	WINAPI
#define THREAD_RETURN	DWORD
#define THREAD_PARAM	void *
#define THREAD_ID	HANDLE
#define CPO		CRITICAL_SECTION
#define	CPO_Init(_x)	InitializeCriticalSection(&_x)
#define CPO_Dele(_x)	DeleteCriticalSection(&_x)
#define CPO_Enter(_x)	EnterCriticalSection(&_x)
#define CPO_Leave(_x)	LeaveCriticalSection(&_x)
#define gettid() GetCurrentThreadId()
#else
#define gettid() syscall(__NR_gettid)
struct CPO { pthread_mutex_t m; pthread_t t;};
#define THREAD_PROC	
#define THREAD_RETURN	void *
#define THREAD_PARAM	void *
#define THREAD_ID	pthread_t
#define CPO_Init(_x)	 { _x.t=0; pthread_mutex_init(&_x.m, NULL); }
#define CPO_Dele(_x)	{ pthread_mutex_destroy(&_x.m); }
#define CPO_Enter(_x)	while(true) \
{ \
	if(_x.t==0) \
{ \
	pthread_mutex_lock(&_x.m); \
	_x.t=pthread_self(); \
	break;\
}\
	if(pthread_self()==_x.t)\
	break; \
	pthread_mutex_lock(&_x.m); \
	_x.t=pthread_self();\
	break; \
}
#define CPO_Leave(_x)	{ pthread_mutex_unlock(&_x.m); _x.t=0; }
#define RENAME(_x, _y) if(rename(_x, _y) != 0)\
{\
char s_Cmd[4098] = "";\
sprintf(s_Cmd, "mv -f %s %s", _x, _y);\
system(s_Cmd);\
}

#endif

typedef THREAD_RETURN THREAD_PROC THREAD_FUNCTION(THREAD_PARAM thread_param);

#ifdef _WIN32
#else

#endif

THREAD_ID bcreate_thread(THREAD_FUNCTION pfun, void * pparam);

#endif