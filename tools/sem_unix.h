#if !defined(_WIN32) && !defined(_WIN64)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

/* 信号量 */
#if defined(_WIN32) || defined(_WIN64)
    #define CriticalSection CRITICAL_SECTION
    #define CriticalSection_init(_x, _y) { InitializeCriticalSection(&(_x)); }
    #define CriticalSection_destroy(_x)  { DeleteCriticalSection(&(_x)); }
    #define CriticalSection_lock(_x)     { EnterCriticalSection(&(_x));}
    #define CriticalSection_unlock(_x)   { LeaveCriticalSection(&(_x)); }
#else
    #define CriticalSection pthread_mutex_t
    #define CriticalSection_init(_x, _y) { pthread_mutex_init(&(_x), NULL); }
    #define CriticalSection_destroy(_x)  { pthread_mutex_destroy(&(_x)); }
    #define CriticalSection_lock(_x)     { pthread_mutex_lock(&(_x));}
    #define CriticalSection_unlock(_x)   { pthread_mutex_unlock(&(_x)); }
#endif

/* 私有信号量 */
struct Semaphore
{
    CriticalSection     semLock;
    pthread_mutex_t     mutex;
    pthread_cond_t      condition;
    int                 semCount;
    int                 semMax;	
};

/* 初始化信号灯 */
int Semaphore_Init(struct Semaphore* cpm, int iMaxSem);

/* 等待信号灯 */
int Semaphore_Wait(struct Semaphore* cpm);
int Semaphore_WaitAll(struct Semaphore* cpm);

/* 释放信号灯 */
int Semaphore_Post(struct Semaphore* cpm);
int Semaphore_PostAll(struct Semaphore* cpm);

/* 删除信号灯 */
int Semaphore_Delete(struct Semaphore* cpm);

#endif
