#if !defined(_WIN32) && !defined(_WIN64)

#include "sem_unix.h"

/* 常量定义 */
#define RC_OBJECT_NOT_CREATED   -11
#define RC_SEM_POST_ERROR       -12
#define RC_SEM_WAIT_ERROR       -13
#define RC_COND_WAIT_ERROR      -14

/* 初始化信号灯 */
int Semaphore_Init(struct Semaphore* token, int iMaxSem)
{
    int rc;
    
    if (iMaxSem <= 0)
    {
        iMaxSem = 64;
    }
    token->semCount = iMaxSem;
    token->semMax = iMaxSem;

    if(rc = pthread_mutex_init(&(token->mutex), NULL))
    {
        return RC_OBJECT_NOT_CREATED;
    }
    
    if(rc = pthread_cond_init(&(token->condition), NULL))
    {
        pthread_mutex_destroy( &(token->mutex) );
        return RC_OBJECT_NOT_CREATED;
    }

    /* 初始化共享锁 */
    CriticalSection_init(token->semLock, NULL);
    
    return 0;
}

/* 等待信号灯 */
int Semaphore_Wait(struct Semaphore* token)
{
    int rc;

    if (rc = pthread_mutex_lock(&(token->mutex)))
        return RC_SEM_WAIT_ERROR;
    
    while (token->semCount <= 0)
    {
        rc = pthread_cond_wait(&(token->condition), &(token->mutex));
        if (rc &&errno != EINTR )
        {
            return RC_COND_WAIT_ERROR;           
			//break;
        }
    }
    token->semCount--;
    
    if (rc = pthread_mutex_unlock(&(token->mutex)))
        return RC_SEM_WAIT_ERROR; 

    return 0;
}
int Semaphore_WaitAll(struct Semaphore* token)
{
    int i,rc;

    CriticalSection_lock(token->semLock);
    for (i = 0; i < token->semMax; i++)
    {
        Semaphore_Wait(token);
    }
    CriticalSection_unlock(token->semLock);

	return 0;
}


/* 释放信号灯 */
int Semaphore_Post(struct Semaphore* token)
{
    int rc;

    if (rc = pthread_mutex_lock(&(token->mutex)))
        return RC_SEM_POST_ERROR;
    
    token->semCount ++;

	if (rc = pthread_cond_signal(&(token->condition)))
	{
               
		if (rc = pthread_mutex_unlock(&(token->mutex)))
           return RC_SEM_POST_ERROR;
		return RC_SEM_POST_ERROR;

	}    
    if (rc = pthread_mutex_unlock(&(token->mutex)))
        return RC_SEM_POST_ERROR;
    
    

    return 0;
}
int Semaphore_PostAll(struct Semaphore* token)
{
    int i,rc;
    
    for (i = 0; i < token->semMax; i++)
    {
        Semaphore_Post(token);
    }

	return 0;
}

/* 删除信号灯 */
int Semaphore_Delete(struct Semaphore* token)
{
    Semaphore_WaitAll(token);
    Semaphore_PostAll(token);
    pthread_mutex_destroy(&(token->mutex));
    pthread_cond_destroy(&(token->condition));

    CriticalSection_lock(token->semLock);
    CriticalSection_unlock(token->semLock);
    CriticalSection_destroy(token->semLock);

	return 0;
}

#endif
