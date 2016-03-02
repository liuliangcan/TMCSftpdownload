#include "macro.h"
#ifdef _WIN32 
THREAD_ID bcreate_thread(THREAD_FUNCTION pfun, void *pparam) 
{ 
	DWORD tid; 
	HANDLE now_handle; 
	now_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pfun, (LPVOID)pparam, 0, &tid); 
	if(now_handle == NULL) 
		return NULL; 
	//return (THREAD_ID)-1; 
	return now_handle; 
} 
#else 
THREAD_ID bcreate_thread(THREAD_FUNCTION pfun, void *pparam) 
{ 
	THREAD_ID tid = (THREAD_ID)NULL; 
	int ret = pthread_create(&tid, NULL, (void * ( *)(void *))pfun, pparam);
	if(0 == ret) 
	{
		pthread_detach(tid); 
		return tid;
	}
	return (THREAD_ID)NULL; 
} 

#endif 