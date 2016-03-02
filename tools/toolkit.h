#ifndef _toolkit_h
#define _toolkit_h
#ifndef local
    #define local static
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>   
#ifdef _WIN32
    #include <io.h>
    #include <time.h>
    #include <stdio.h>
    #include <fcntl.h>
    #include <direct.h>
    #include <windows.h>
#else
	#include <unistd.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <signal.h>
	#include <semaphore.h>
	#include <pthread.h>
    #include <fcntl.h>
	#include <sys/types.h>
    #include <sys/stat.h>
	#include <sys/time.h>
    #include <sys/ipc.h>
    #include <sys/sem.h>
    #include <sys/wait.h>
#endif

#ifdef _WIN32
    #define SLEEP Sleep
    #define F_mkdir(_x) mkdir((_x))
#else
    #define SLEEP usleep
    #define F_mkdir(_x) mkdir(_x, 0755)
    #define stricmp strcasecmp
    #define strnicmp strncasecmp
#endif

/* 判断标志符 */
#define IS_VALID_CHAR(_x) ( ((unsigned char)_x >= 'a' && (unsigned char)_x <= 'z') || ((unsigned char)_x >= 'A' && (unsigned char)_x <= 'Z') || ((unsigned char)_x >= '0' && (unsigned char)_x <= '9') || ((unsigned char)_x == '_') || ((unsigned char)_x == '-') || ((unsigned char)_x == '.') )
#define IS_LOWER_CHAR(_x) ( ((unsigned char)_x >= 'a' && (unsigned char)_x <= 'z') )
#define IS_UPPER_CHAR(_x) ( ((unsigned char)_x >= 'A' && (unsigned char)_x <= 'Z') )

#include "dir_info.h"
#ifdef _WIN32
    #include "sem_win32.h"
#else
    #include "sem_unix.h"
#endif

/* 定义文件读写缓冲区大小 */
#define MAX_BUFFER_LEN 4096

/* 文件(文件夹)列表 */
struct XInfoList
{
	char* Name;			    /* 文件名 */
	long FileSize;		    /* 文件大小(文件夹的大小为0) */
	long CreateTime;		/* 文件(文件夹)创建时间 */
	struct XInfoList* next;
};

/* 文件列表结构 */
struct XDirList
{
	struct XInfoList* ptrList;	/* 指向文件(文件夹)列表的指针 */
	int Size;			        /* 列表大小 */
};

#if defined (__cplusplus)
extern "C"
{
#endif
/* 检查文件夹名字是否是'.','..' */
int F_IsNotDot(const char* sSrcDir);

/* 文件/文件夹 */
int F_IsPathExist(char *sSrcPath);
int F_IsDirExist(char *sSrcDir);
int F_IsFileExist(char *SrcFile);

/* 创建文件夹 */
int F_MakeDirs(char* sSrcDir, int bSelf);

/* 复制文件(单个文件) */
int F_CopyFile(char* sSrcFile, char* sDstFile);

/* 复制文件(多个文件,包括子目录) */
int F_CopyFiles(char* sSrcDir, char* sDstDir);

/* 删除文件(单个文件) */
int F_RemoveFile(char* sSrcFile);

/* 删除文件夹 */
int F_RemoveDir(char *sSrcDir);

/* 删除文件(多个文件,包括子目录) */
int F_RemoveFiles(char* sSrcDir);

/* 创建链表 */
struct XDirList* F_CreateXDirList();

/* 释放链表 */
void F_FreeXDirList(struct XDirList* _objXList);

/* 获取文件列表 */
struct XDirList* F_GetXDirList(struct XDirList* _objXList, char* sSrcDir);

/* 检查模板名称 */
int F_isValidModelName(const char* sModelName);

/* 获取目录创建时间 */
int F_GetDirCreateTime(char *sDirName, char *sTime, int iLen);

/* 获取时间 */
int F_GetTime(char* sBuffer, int nSize);

/* 大写转换 */
char* F_ToUpperCase(char *sBuffer);

/* 大写转换 */
char* F_ToLowerCase(char *sBuffer);

/* 字符串比较(忽略前后空格) */
int F_CompareString(char *s, char *t);

/* 字符串比较(忽略大小写, 前后空格) */
int F_CompareNoCaseString(char *s, char *t);

/* 判断中文字符串 */
int F_IsChinese(char *sWords);

/* 判断英文字符串 */
int F_IsEnglish(char *sWords);

/* 判断中文串 */
int F_IsHasChinese(char *sChinese);

/* 获取当前程序工作目录 */
int F_GetAppHomePath(char* sBuffer, int nSize);

/* 获取基本名(不含路径) */
int F_GetBaseFileName(char *sSrcDir);

/* 判断BigEndian或LittleEndian */
int F_IsBigEndian();

/* 写入整数 */
int F_WriteInt(FILE *hFile, int n);

/* 读取整数 */
int F_ReadInt(FILE *hFile, int *n);

#if defined (__cplusplus)
}
#endif
#endif
