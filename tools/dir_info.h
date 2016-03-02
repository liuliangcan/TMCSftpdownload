#ifndef _dir_info_h
#define _dir_info_h
#ifndef _WIN32
    #include <sys/types.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
    #include <time.h>
#else
    #include <windows.h>
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
    #include <direct.h>
    #include <time.h>
    #include <io.h>
#endif
#include <sys/stat.h>

/*目录句柄*/
struct DIR_INFO
{
#ifdef _WIN32
	long hHandle;		/*Windows句柄*/
#else
    DIR *hHandle;       /*Unix句柄*/
#endif
    void *hVoid;        /*void*/
	char sPath[260];	/*路径*/
};

/*文件信息*/
struct FILE_INFO
{
	 char sName[260];	/*文件(子目录)名*/
	 int bDir;			/*目录标记*/
	 time_t tCreate;	/*创建时间*/
	 time_t tAccess;	/*访问时间*/
	 time_t tWrite;		/*修改时间*/
	 long	lSize;		/*文件大小*/
};


#if defined (__cplusplus)
extern "C"
{
#endif
/*打开目录*/
struct DIR_INFO* F_DirOpen(char *sDirName);

/*查找文件(子目录)*/
int F_DirNextItem(struct DIR_INFO *hDir, struct FILE_INFO *hFileInfo);

/*关闭目录*/
int F_DirClose(struct DIR_INFO *hDir);

/*判断目录是否存在*/
int F_DirIsExist(char *sDirName);

/*目录判空*/
int F_DirIsEmpty(char *sDirName);
#if defined (__cplusplus)
}
#endif
#endif
