#include "toolkit.h"

/* 联合体 */
static int __s_bBigEndian = -1;
union __s_unEndian
{
    int a;
    short b[2];
};

/* 判断BigEndian或LittleEndian */
int F_IsBigEndian()
{
    union __s_unEndian _unEndian;
    if (__s_bBigEndian != -1)
        return __s_bBigEndian;
    
    /* 通过union判断字节顺序 */
    __s_bBigEndian = 0;
    _unEndian.a = 0x00010002;
    if (_unEndian.b[0] == 1 && _unEndian.b[1] == 2)
    {
        __s_bBigEndian = 1;
    }
    
    /* 返回字节顺序 */
    return __s_bBigEndian;
}

/* 写入整数 */
int F_WriteInt(FILE *hFile, int n)
{
    unsigned char p[4];
    
    /* BigEndian平台 */
    if (F_IsBigEndian() != 0)
    {
        p[0] = (unsigned char)((n >> 0) & 0xFF);
        p[1] = (unsigned char)((n >> 8) & 0xFF);
        p[2] = (unsigned char)((n >> 16) & 0xFF);
        p[3] = (unsigned char)((n >> 24) & 0xFF);
        fwrite(p, sizeof(unsigned char), 4, hFile);
    }
    else
    {
        fwrite(&n, sizeof(int), 1, hFile);
    }
    return 1;
}

/* 读取整数 */
int F_ReadInt(FILE *hFile, int *n)
{
    unsigned char *p;
    unsigned int l;
    
    fread((void *)n, sizeof(unsigned char), 4, hFile);
    
    /* BigEndian平台 */
    if (F_IsBigEndian() != 0)
    {
        p = (unsigned char *)n;
        l =  ((unsigned int)p[0] << 0) | ((unsigned int)p[1] << 8) | ((unsigned int)p[2] << 16) | ((unsigned int)p[3] << 24);
        (*n) = l;
    }
    
    return (*n);
}

/* 检查文件夹名字是否是'.','..' */
int F_IsNotDot(const char *sSrcDir)
{
    /* 循环变量 */
    int i;

    /* 参数判空 */
    if (sSrcDir == NULL)
    {
        return 0;
    }

    /* 判断是否包含. */
    for (i = 0; (i < 2) && sSrcDir[i]; i++)
    {
        if (sSrcDir[i] != '.')
        {
            return 1;
        }
    }

    return 0;
}

/* 创建文件夹 */
int F_MakeDirs(char *sSrcDir, int bSelf)
{
    int i, n;
    char sFullPath[256];

    /* 参数判空 */
    if (sSrcDir == NULL)
    {
        return -1;
    }

	/* 初始化错误号 */
	n = 0;

    /* 逐层创建子文件夹 */
    for (i = 0; sSrcDir[i]; i++)
    {
        if (sSrcDir[i] == '\\' || sSrcDir[i] == '/')
        {
            strncpy(sFullPath, sSrcDir, i);
            sFullPath[i] = '\0';
            n = F_mkdir(sFullPath);
        }
    }

    /* 创建目录本身 */
	if (bSelf != 0)
	{
		n = F_mkdir(sSrcDir);
	}

    /* 获取错误号 */
    if (n != 0)
    {
        n = errno;
    }

    return n;
}

/* 判断路径是否存在 */
int F_IsPathExist(char *sSrcPath)
{
	struct stat buf;
	int result;
	
	result = stat(sSrcPath, &buf);
	
	return (result == 0) ? 1 : 0;
}

/* 判断目录是否存在 */
int F_IsDirExist(char *sSrcDir)
{
	struct stat buf;
	int result;
	
	result = stat(sSrcDir, &buf);
	if (result < 0)
		return 0;
	
#ifdef _WIN32
	result = (buf.st_mode & _S_IFDIR);
#else
	result = S_ISDIR(buf.st_mode);
#endif
	
	return (result != 0) ? 1 : 0;
}

/* 判断文件是否存在 */
int F_IsFileExist(char *SrcFile)
{
	struct stat buf;
	int result;
	
	result = stat(SrcFile, &buf);
	if (result < 0)
		return 0;
	
#ifdef _WIN32
	result = (buf.st_mode & _S_IFDIR);
#else
	result = S_ISDIR(buf.st_mode);
#endif
	
	return (result != 0) ? 0 : 1;
}


/* 复制单个文件 */
int F_CopyFile(char *sSrcFile, char *sDstFile)
{
    /* 错误代码 */
    int m_errno = 0;

    /* 复制文件 */
    FILE *fSource, *fDest;
    char sBuffer[MAX_BUFFER_LEN];
    int iBufferLen = 0;
    int iWriteBytes = 0;

    /* 参数判空 */
    if (sSrcFile == NULL || sDstFile == NULL)
    {
        return -1;
    }

    /* 按二进制读源文件 */
    fSource = fopen(sSrcFile, "rb");
    if (fSource == NULL)
    {
        return -1;
    }

    /* 按二进制写目标文件 */
    fDest = fopen(sDstFile, "wb");
    if (fDest == NULL)
    {
        /* 关闭源文件 */
        fclose(fSource);
        return -1;
    }

    /* 设置文件头指针 */
    fseek(fSource, 0, 0);

    /* 复制文件内容(二进制) */
    while (!feof(fSource))
    {
        /* 从源文件读取数据 */
        iBufferLen = fread(sBuffer, sizeof(char), MAX_BUFFER_LEN, fSource);

        /* 将数据写入目标文件 */
        iWriteBytes = fwrite(sBuffer, sizeof(char), iBufferLen, fDest);

        /* 写入失败 */
        if (iWriteBytes != iBufferLen)
        {
            m_errno = -311;
            break;
        }
    }

    /* 关闭源文件 */
    fclose(fSource);

    /* 关闭目标文件 */
    fclose(fDest);

    /* 返回 */
    return m_errno;
}

/* 复制文件(多个文件,包括子目录) */
int F_CopyFiles(char *sSrcDir, char *sDstDir)
{
    /* 错误代码 */
    int m_errno = 0;

    /* 源文件名 */
    char sSrcFileFullPath[256];

    /* 目标文件名 */
    char sDstFileFullPath[256];

    /* 文件信息 */
    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;

    /* 参数判空 */
    if (sSrcDir == NULL || sDstDir == NULL || sSrcDir == sDstDir)
    {
        return -1;
    }

    /* 打开文件夹 */
    hDirInfo = F_DirOpen(sSrcDir);
    if (hDirInfo != NULL)
    {
        /* 建立目标文件夹 */
	    F_MakeDirs(sDstDir, 1);

        /* 遍历文件 */
        m_errno = 0;
        while (m_errno == 0 && F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;

            /* 初始化参数 */
            sprintf(sSrcFileFullPath, "%s/%s", sSrcDir, hFileInfo.sName);
            sprintf(sDstFileFullPath, "%s/%s", sDstDir, hFileInfo.sName);

            /* 复制文件/文件夹 */
            if (hFileInfo.bDir == 0)
            {
                /* 文件 */
                m_errno = F_CopyFile(sSrcFileFullPath, sDstFileFullPath);
                
            }
            else
            {
                /* 文件夹 */
                m_errno = F_CopyFiles(sSrcFileFullPath, sDstFileFullPath);
            }
        }
    }

    /* 关闭文件夹 */
    F_DirClose(hDirInfo);

    return m_errno;
}

/* 删除文件(单个文件) */
int F_RemoveFile(char *sSrcFile)
{
	if (F_IsFileExist(sSrcFile))
	{
		return remove(sSrcFile);
	}

	return -1;
}

/* 删除文件夹 */
int F_RemoveDir(char *sSrcDir)
{
	if (F_IsDirExist(sSrcDir))
	{
		return rmdir(sSrcDir);
	}

    return -1;
}

/* 删除文件(多个文件,包括子目录) */
int F_RemoveFiles(char *sSrcDir)
{
    /* 错误代码 */
    int m_errno = 0;
    
    /* 源文件名 */
    char sSrcFileFullPath[256];
    
    /* 文件信息 */
    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;
    
    /* 参数判空 */
    if (sSrcDir == NULL)
    {
        return -1;
    }
    
    /* 打开文件夹 */
    hDirInfo = F_DirOpen(sSrcDir);
    if (hDirInfo != NULL)
    {
        /* 遍历文件 */
        m_errno = 0;
        while (m_errno == 0 && F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;
            
            /* 初始化参数 */
            sprintf(sSrcFileFullPath, "%s/%s", sSrcDir, hFileInfo.sName);
            
            /* 删除文件/文件夹 */
            if (hFileInfo.bDir == 0)
            {
                /* 文件 */
                m_errno = F_RemoveFile(sSrcFileFullPath);
                
            }
            else
            {
                /* 文件夹 */
                m_errno = F_RemoveFiles(sSrcFileFullPath);
            }
        }
    }
    
    /* 关闭文件夹 */
    F_DirClose(hDirInfo);

    /* 删除目录自身 */
    m_errno = F_RemoveDir(sSrcDir);
    
    return m_errno;
}

/* 创建链表 */
struct XDirList * F_CreateXDirList()
{
    struct XDirList *pNode;

    /* 申请内存 */
    pNode = (struct XDirList *) malloc(sizeof(struct XDirList));

    /* 初始化节点 */
    if (pNode != NULL)
    {
        pNode->ptrList = NULL;
        pNode->Size = 0;
    }

    return pNode;
}

/* 释放链表 */
void F_FreeXDirList(struct XDirList *_objXList)
{
    /* 节点信息 */
    struct XInfoList *m_XInfoList = NULL;

    /* 参数判空 */
    if (_objXList == NULL)
    {
        return;
    }

    /* 遍历节点 */
    while ((_objXList != NULL) && (_objXList->ptrList != NULL))
    {
        m_XInfoList = _objXList->ptrList;
        _objXList->ptrList = (_objXList->ptrList)->next;

        /* 释放节点 */
        if (m_XInfoList->Name)
        {
            free(m_XInfoList->Name);
        }

        free(m_XInfoList);
    }

    _objXList->ptrList = NULL;
    _objXList->Size = 0;

    /* 释放头指针 */
    free(_objXList);

    _objXList = NULL;
}

/* 获取文件列表 */
struct XDirList * F_GetXDirList(struct XDirList *_objXList, char *sSrcDir)
{
    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;
    struct XInfoList *qNode = NULL;

    hDirInfo = F_DirOpen(sSrcDir);
    if (hDirInfo != NULL)
    {
        while (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;
            
            if (hFileInfo.bDir != 0)
            {
                /* 添加新节点 */
                qNode = (struct XInfoList *) malloc(sizeof(struct XInfoList));
                qNode->Name = (char *) malloc(256);
                strcpy(qNode->Name, hFileInfo.sName);
                qNode->CreateTime = hFileInfo.tCreate;
                qNode->FileSize   = hFileInfo.lSize;
                qNode->next = NULL;
                qNode->next = _objXList->ptrList;
                _objXList->ptrList = qNode;
                _objXList->Size++;
            }
        }

        F_DirClose(hDirInfo);
    }

    return _objXList;
}

/* 检查模板名称(长度小于32位的字符串,由大小写字母/数字等组成) */
int F_isValidModelName(const char *sModelName)
{
    /* 临时变量 */
    int i;

    /* 参数判空 */
    if (sModelName == NULL || sModelName[0] == '\0')
    {
        return -1;
    }
    
    /* 字符判断 */
    for (i = 0; sModelName[i]; i++)
    {
        if (!IS_VALID_CHAR(sModelName[i]))
        {
            return -2;
        }
    }

    return (i <= 32) ? 0 : -3;
}
int F_isValidString(const char *sQueryString)
{
    /* 临时变量 */
    int i;
	
    /* 参数判空 */
    if (sQueryString == NULL || sQueryString[0] == '\0')
    {
        return -1;
    }
    
    /* 字符判断 */
    for (i = 0; sQueryString[i]; i++)
    {
        if (!IS_VALID_CHAR(sQueryString[i]))
        {
            return -1;
        }
    }
    return 0;
}
/* 获取时间 */
int F_GetTime(char *sBuffer, int nSize)
{
    time_t tTime;
    struct tm *pTime;

    /* 获取当前时间 */
    time(&tTime);
    pTime = localtime(&tTime);
    //mktime(&pTime);
    /* 格式化时间 */
    sprintf(sBuffer, "%04d%02d%02d", (1900 + pTime->tm_year), (1 + pTime->tm_mon), pTime->tm_mday);
    sprintf(sBuffer + 8, "%02d%02d%02d", pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

    return 0;
}

/* 大写转换 */
char * F_ToUpperCase(char *sBuffer)
{
    unsigned c1, c2;
    int i;
    if (sBuffer != NULL)
    {
        for (i = 0; sBuffer[i]; i++)
        {
            /* 忽略汉字 */
            c1 = (unsigned char)(sBuffer[i]);
            c2 = (unsigned char)(sBuffer[i+1]);
            if ((c1 >= 0x80) && (c2 >= 0x30))
            {                
                i++;
            }
            
            /* 小写转换为大写 */
            if (sBuffer[i] >= 'a' && sBuffer[i] <= 'z')
            {
                sBuffer[i] = sBuffer[i] + ('A' - 'a');
            }
        }
    }
    
    return sBuffer;
}

/* 大写转换 */
char * F_ToLowerCase(char *sBuffer)
{
    unsigned c1, c2;
    int i;
    
    if (sBuffer != NULL)
    {
        for (i = 0; sBuffer[i]; i++)
        {
            /* 忽略汉字 */
            c1 = (unsigned char)(sBuffer[i]);
            c2 = (unsigned char)(sBuffer[i+1]);
            if ((c1 >= 0x80) && (c2 >= 0x30))
            {                
                i++;
            }
            
            /* 大写转换为小写*/
            if (sBuffer[i] >= 'A' && sBuffer[i] <= 'Z')
            {
                sBuffer[i] = sBuffer[i] + ('a' - 'A');
            }
        }
    }

    return sBuffer;
}

static int F_CompareBlankString(char *s, char *t, int bCase)
{
    int i, n;
    int s_n0, s_n1, t_n0, t_n1;
    
    /* 参数判空 */
    if (s == NULL)
        return -1;
    if (t == NULL)
        return 1;

    /* 非空格位置 */
    s_n0 = s_n1 = t_n0 = t_n1 = -1;
    for (i = 0; s[i]; i++)
    {
        if (s[i] != ' ' && s[i] != '\t')
        {
            if (s_n0 <= -1)
            {
                s_n0 = i;
            }
            else
            {
                s_n1 = i;
            }
        }
    }
    for (i = 0; t[i]; i++)
    {
        if (t[i] != ' ' && t[i] != '\t')
        {
            if (t_n0 <= -1)
            {
                t_n0 = i;
            }
            else
            {
                t_n1 = i;
            }
        }
    }
    
    /* 参数判空 */
    if (s_n0 == -1 || s_n1 == -1)
        return -1;
    
    if (t_n0 == -1 || t_n1 == -1)
        return 1;
    
    /* 比较字符串大小 */
    if (t_n1 - t_n0 <= s_n1 - s_n0)
    {
        n = t_n1 - t_n0 + 1;
    }
    else
    {
        n = s_n1 - s_n0 + 1;
    }

    if (bCase == 0)
        return strncmp(s + s_n0, t + t_n0, n);
    else
        return strnicmp(s + s_n0, t + t_n0, n);
}

/* 字符串比较(忽略前后空格) */
int F_CompareString(char *s, char *t)
{
    return F_CompareBlankString(s, t, 0);
}

/* 字符串比较(忽略大小写, 前后空格) */
int F_CompareNoCaseString(char *s, char *t)
{
    return F_CompareBlankString(s, t, 1);
}

/* 判断中文字符串 */
int F_IsChinese(char *sWords)
{
    unsigned c1, c2;
    int i;
    
    /* 参数判空 */
    if (sWords == NULL)
        return -1;

    /* 判断汉字 */
    for (i = 0; sWords[i]; i++)
    {
        c1 = (unsigned char)(sWords[i]);
        c2 = (unsigned char)(sWords[i+1]);
        if ((c1 < 0x80) || (c2 < 0x30))
        {
            return -1;
        }
        i++;
    }

    return 0;
}

/* 判断英文字符串 */
int F_IsEnglish(char *sWords)
{
    int i;
    
    /* 参数判空 */
    if (sWords == NULL)
        return -1;

    /* 判断字符 */
    for (i = 0; sWords[i]; i++)
    {
        if (sWords[i] & 0x80)
            return -1;
    }

    return 0;
}

/* 判断是否包含中文串 */
int F_IsHasChinese(char *sChinese)
{
    int i;
    
    /* 参数判空 */
    if (sChinese == NULL)
        return -1;
    
    /* 判断中文字符 */
    for (i = 0; sChinese[i]; i++)
    {
        if (sChinese[i] & 0x80)
            return 0;
    }

    return -1;
}

/* 获取目录创建时间 */
int F_GetDirCreateTime(char *sDirName, char *sTime, int iLen)
{
    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;
    struct tm *tTime;
    
    /* 参数判空 */
    if (sDirName == NULL)
        return 0;
    
    /* 初始化 */
    sTime[0] = '\0';

    hDirInfo = F_DirOpen(sDirName);
    if (hDirInfo != NULL)
    {
        if (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            /* 转换本地时间 */
            tTime = localtime(&hFileInfo.tCreate);
            
            /* 格式化时间串 */
            sprintf(sTime, "%04d/%02d/%02d %02d:%02d:%02d", tTime->tm_year + 1900, tTime->tm_mon + 1, tTime->tm_mday, tTime->tm_hour, tTime->tm_min, tTime->tm_sec);
        }
        
        F_DirClose(hDirInfo);
    }
    
    return 0;
}

/* 获取当前程序工作目录 */
int F_GetAppHomePath(char *sBuffer, int nSize)
{
    int i;
    
	/* 初始化 */
    sBuffer[0] = '\0';

	/* AIX/HPUX/SUNOS */
	#if !defined(WIN32) && !defined(LINUX)
    {
        getcwd(sBuffer, nSize);

		return 0;
    }
	#endif
	
	/* WIN32/LINUX */
	#if defined(LINUX)
		nSize = readlink("/proc/self/exe", sBuffer, nSize);
	#else
		nSize = GetModuleFileName(NULL, sBuffer, nSize);
	#endif
		sBuffer[nSize] = '\0';

	/* 获取文件夹路径 */
	for (i = nSize; i >= 0; i--)
	{
		if (sBuffer[i] == '\\' || sBuffer[i] == '/')
		{
			sBuffer[i] = '\0';
			break;
		}
	}

	return 0;
}

/* 获取文件名 */
int F_GetBaseFileName(char *sSrcDir)
{
    char *sLastPos, sBaseName[32];

    /* 设置缓冲区 */
#ifdef _WIN32
    sLastPos = strrchr(sSrcDir, '\\');
#else
    sLastPos = strrchr(sSrcDir, '/');
#endif
    if (sLastPos == NULL)
        sLastPos = sSrcDir;

    /* 复制缓冲区 */
    strncpy(sBaseName, sLastPos + 1, 31);
    sBaseName[31] = '\0';
    strcpy(sSrcDir, sBaseName);

    return 0;
}


