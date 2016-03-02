#include "dir_info.h"

/* 打开目录 */
struct DIR_INFO* F_DirOpen(char *sDirName)
{
	struct DIR_INFO *pResult = (struct DIR_INFO *)malloc(sizeof(struct DIR_INFO));
#ifndef _WIN32
	DIR *hDir;

	if (pResult == NULL)
	{
		return NULL;
	}
    
    /* 打开文件夹 */
	hDir = opendir(sDirName);
	pResult->hHandle = NULL;
    if (hDir != NULL)
	{
		pResult->hHandle = hDir;
	}
#else
	struct _finddata_t hFile;
	long hDir;
	char sTempName[256];
	
	if (pResult == NULL)
		return NULL;
    
    /* 打开文件夹 */
	sprintf(sTempName, "%s/*.*", sDirName);
	hDir = _findfirst(sTempName, &hFile);
	pResult->hHandle = 0;
	if (hDir != -1L)
	{
		pResult->hHandle = hDir;
	}
#endif
	
	/* 存储根文件夹,Unix/Linux获取文件状态时需要用到它 */
	strcpy(pResult->sPath, sDirName);
	if (!(pResult->hHandle))
	{
		free(pResult);
		pResult =  NULL;
	}
	return pResult;
}

/* 查找文件(子目录) */
int F_DirNextItem(struct DIR_INFO *hDir, struct FILE_INFO *hFileInfo)
{
#ifndef _WIN32
	char sFullPath[256];
	struct stat statInfo;
	struct dirent *hInfo;
    
    if (hDir == NULL || hFileInfo == NULL || hDir->hHandle == NULL)
	{
		return -1;
	}
	
	/* 查找下一个文件 */
	hInfo = readdir(hDir->hHandle);
    if (hInfo == NULL)
    {
        return -1;
    }

	/* 获取文件信息 */
	sprintf(sFullPath, "%s/%s", hDir->sPath, hInfo->d_name);
	stat(sFullPath, &statInfo);
	
	/* 存储文件信息 */
	strcpy(hFileInfo->sName, hInfo->d_name);
	hFileInfo->bDir = (S_ISDIR(statInfo.st_mode)) ? 1: 0;
	hFileInfo->tCreate = statInfo.st_ctime;
          hFileInfo->tAccess = statInfo.st_atime;
          hFileInfo->tWrite  = statInfo.st_mtime;
          hFileInfo->lSize   = statInfo.st_size;
#else
	struct _finddata_t hInfo;

	/* 查找下一个文件 */
	if (_findnext(hDir->hHandle, &hInfo) != 0)
	{
		return -1;
	}

	/* 存储文件信息 */
	strcpy(hFileInfo->sName, hInfo.name);
	hFileInfo->bDir	= (hInfo.attrib & _A_SUBDIR) ? 1 : 0;
	hFileInfo->tCreate = hInfo.time_create;
	hFileInfo->tAccess = hInfo.time_access;
	hFileInfo->tWrite  = hInfo.time_write;
	hFileInfo->lSize   = hInfo.size;
#endif
	return 0;
}

/* 关闭目录 */
int F_DirClose(struct DIR_INFO *hDir)
{
	if (hDir != NULL && hDir->hHandle)
	{
#ifndef _WIN32
        closedir(hDir->hHandle);
#else
        _findclose(hDir->hHandle);
#endif
        free(hDir);
	}
	return 0;
}

/* 判断目录是否存在 */
int F_DirIsExist(char *sDirName)
{
	struct DIR_INFO *hDirInfo = F_DirOpen(sDirName);
	if (hDirInfo != NULL)
	{
		F_DirClose(hDirInfo);
		return 1;
	}
	else
	{
		return 0;
	}
}

/* 目录判空(目录不存在其它文件) */
int F_DirIsEmpty(char *sDirName)
{
	int bFlag;
	struct DIR_INFO *hDirInfo;
	struct FILE_INFO hFileInfo;

	/* 初始化 */
	bFlag = 1;

	/* 打开文件夹 */
	hDirInfo = F_DirOpen(sDirName);
	if (hDirInfo != NULL)
	{
		while (F_DirNextItem(hDirInfo, &hFileInfo) != -1)
		{
			if (hFileInfo.sName[0] != '.')
			{
				bFlag = 0;
				break;
			}
		}

		/* 关闭文件夹 */
		F_DirClose(hDirInfo);
	}
	return bFlag;
}
