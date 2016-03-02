/* 
 * File:   FTPDriver.h
 * Author: LIU
 * 
 *
 * Created on 2016年2月17日, 下午2:50
 */

#ifndef FTPDRIVER_H
#define	FTPDRIVER_H

#include <string>
#include <queue>
#include "tools/macro.h"
#include "DoneFiles.h"

#include <curl/curl.h>

typedef struct ftpfilestruct {
    char local_filename[PATH_MAX];          //本地文件名，完整路径
    char remote_file_dir[PATH_MAX];         //服务器上的文件名，带路径
    FILE* file_handler;                                 //本地文件句柄
    unsigned long long m_ullFileSize;
} ONEFTPFILE;

class FTPDriver {
public:
    FTPDriver();
    FTPDriver(const FTPDriver& orig);
    int init();
    int ScanDir(const char* tar_dir);
    int Download();
    virtual ~FTPDriver();
    
    size_t data_handle(char *buffer, size_t size, size_t nmemb);                //文件下载回调
    
    
    size_t ScanDirDataFunction(void *buffer, size_t size, size_t nmemb);    //列表扫描回调
private:
    std::string m_sUserAuthen;                                              //用户名密码("root:tmcsoft@dc"))
    std::string m_sFtpUrl;                                                      //连接地址("ftp://192.168.2.103:22/tmcdata/lsl/ftpdir/HZ/12/"))
    std::string m_output_dir;                                                   //输出目录
    std::string m_sExtension;                                                   //要下载的文件扩展名
    std::string m_sPrefix;                                                   //要下载的文件扩展名
    CURL *m_ftp_scan_handle;                                                //列表扫描句柄
    CURL *m_ftp_download_handle;                                        //文件下载句柄
    std::queue<std::pair<std::string, char> > m_qFilelist;          //宽搜队列
    ONEFTPFILE m_ftp_file;                                                      //当前处理文件信息
    int Open_local_file();                                                          //打开本地文件
    int Close_local_file();                                                             //关闭本地文件
    void error_format(const int errn);                                          //错误日志
    DoneFiles* m_doneFiles;                                                         //已下载文件列表    
    
};

#endif	/* FTPDRIVER_H */

