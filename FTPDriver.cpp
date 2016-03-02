/* 
 * File:   FTPDriver.cpp
 * Author: LIU
 * 
 * Created on 2016年2月17日, 下午2:50
 */

#include "FTPDriver.h"
#include "Callback_functions_curl.h"
#include "GlobalConfiger.h"
#include "tools/log.h"
using namespace std;

#define END_TMP_STRING "__TMP__"

FTPDriver::FTPDriver()
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_ftp_scan_handle = curl_easy_init();
    m_ftp_download_handle = curl_easy_init();
    m_doneFiles = DoneFiles::GetInstance();
}

FTPDriver::FTPDriver(const FTPDriver& orig)
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_ftp_scan_handle = curl_easy_init();
    m_ftp_download_handle = curl_easy_init();
    m_doneFiles = DoneFiles::GetInstance();
}

FTPDriver::~FTPDriver()
{
    m_doneFiles->EndCollect();
    curl_easy_cleanup(m_ftp_download_handle);
    curl_easy_cleanup(m_ftp_scan_handle);
    curl_global_cleanup();
}

int FTPDriver::init()
{

    m_sFtpUrl = GlobalConfiger::GetInstance()->GetSFtpUrl();
    m_sUserAuthen = GlobalConfiger::GetInstance()->GetSUserAuthen();
    m_output_dir = GlobalConfiger::GetInstance()->GetOutputDir();
    m_sExtension = GlobalConfiger::GetInstance()->GetSExtension();
    m_sPrefix = GlobalConfiger::GetInstance()->GetSPrefix();
    memset(&m_ftp_file, 0, sizeof (m_ftp_file));
    m_ftp_file.file_handler = 0x0;

    curl_easy_setopt(m_ftp_download_handle, CURLOPT_USERPWD, m_sUserAuthen.c_str());

    //    curl_easy_setopt(m_ftp_download_handle, CURLOPT_WILDCARDMATCH, 1L);
    //    curl_easy_setopt(m_ftp_download_handle, CURLOPT_CHUNK_BGN_FUNCTION, Callback_functions_curl::file_is_coming);
    //    curl_easy_setopt(m_ftp_download_handle, CURLOPT_CHUNK_END_FUNCTION, Callback_functions_curl::file_is_downloaded);
    curl_easy_setopt(m_ftp_download_handle, CURLOPT_WRITEFUNCTION, Callback_functions_curl::data_handle);
    //  curl_easy_setopt(m_ftp_download_handle, CURLOPT_CHUNK_DATA, this);
    curl_easy_setopt(m_ftp_download_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_ftp_download_handle, CURLOPT_TIMEOUT, 1024);

    curl_easy_setopt(m_ftp_scan_handle, CURLOPT_CUSTOMREQUEST, "NLST");
    curl_easy_setopt(m_ftp_scan_handle, CURLOPT_USERPWD, m_sUserAuthen.c_str());
    //printf("auth:%s\n",m_sUserAuthen.c_str());
    //curl_easy_setopt(m_ftp_scan_handle, CURLOPT_FTPLISTONLY , 1L);    
    curl_easy_setopt(m_ftp_scan_handle, CURLOPT_WRITEFUNCTION, Callback_functions_curl::ScanDirDataFunction);
    curl_easy_setopt(m_ftp_scan_handle, CURLOPT_TIMEOUT, 1024);
    curl_easy_setopt(m_ftp_scan_handle, CURLOPT_WRITEDATA, this);
    return 0;
}

size_t FTPDriver::data_handle(char *buffer, size_t size, size_t nmemb)
{
    size_t written = 0;
    if (m_ftp_file.file_handler)
    {
        written = fwrite(buffer, size, nmemb, m_ftp_file.file_handler);
    }
    else
    {
        /* listing output */
        written = fwrite(buffer, size, nmemb, stdout);
    }
    //LOG_NOTICE("写入[ %u BYTE][%s]", written, m_ftp_file.local_filename);
    m_ftp_file.m_ullFileSize += written;
    return written;
}

int FTPDriver::Open_local_file()
{
    char local_filename[PATH_MAX] = "";    
    if(m_ftp_file.remote_file_dir[0] == '/')
    {
        strcpy(local_filename, m_ftp_file.remote_file_dir + 1);
    }
    else
    {
        strcpy(local_filename, m_ftp_file.remote_file_dir);
    }
    char * p = local_filename;
    while (*p)
    {
        if (*p == '/')
            *p = '_';
        ++p;
    }
    sprintf(m_ftp_file.local_filename, "%s/%s%s", m_output_dir.c_str(), local_filename, END_TMP_STRING);

    LOG_NOTICE("准备下载文件[%s]", m_ftp_file.remote_file_dir);
    m_ftp_file.m_ullFileSize = 0;
    m_ftp_file.file_handler = fopen(m_ftp_file.local_filename, "wb");
    if (!m_ftp_file.file_handler)
    {
        LOG_ERROR("本地文件打开失败！[%s]", m_ftp_file.local_filename);
        return -1;
    }
    return 0;
}

int FTPDriver::Close_local_file()
{
    if (m_ftp_file.file_handler)
    {
        fclose(m_ftp_file.file_handler);
        char rename_name[PATH_MAX] = "";
        int lenth = strlen(m_ftp_file.local_filename) - strlen(END_TMP_STRING);
        strncpy(rename_name, m_ftp_file.local_filename, lenth);
        rename_name[lenth] = 0;
        
        RENAME(m_ftp_file.local_filename, rename_name);
        LOG_NOTICE("文件下载成功[%llu][%s]", m_ftp_file.m_ullFileSize, rename_name);
        m_ftp_file.file_handler = 0x0;
    }
    return 0;
}

size_t FTPDriver::ScanDirDataFunction(void *buffer, size_t size, size_t nmemb)
{
    char is_dir = 0;
    size_t lenth = size * nmemb;

    if (lenth <= 0)
    {
        return lenth;
    }
    char * p = (char*) buffer + lenth - 1;

    while (p != buffer)
    {
        if (*p == ' ')
        {
            ++p;
            break;
        }
        --p;
    }
    
    if (*p == '.')return lenth;
    
    if (((char*) buffer)[0] == 'd')
    {
        is_dir = 1;
    }

    char part_url[PATH_MAX] = "";
    sprintf(part_url, "%s/", m_ftp_file.remote_file_dir);

    int part_lenth = strlen(part_url);
    int p_lenth = (char*) buffer + lenth - p - 1;
    memcpy(part_url + part_lenth, p, p_lenth);
    part_url[part_lenth + p_lenth] = 0;
    string one_dir;
    one_dir.assign(part_url);
    if (is_dir)
    {
        if(p_lenth == 8)
        {
            if(*p == '2' && *(p + 1) == '0')
            {
                char dir_name[16] = "";
                memcpy(dir_name, p, 8);
                dir_name[8] = 0;
                tm dir_time_tm = {0};
                if(strptime(dir_name, "%Y%m%d", &dir_time_tm) != NULL)
                {
                    time_t dir_time_tt = mktime(&dir_time_tm);
                    time_t time_now_tt = time(0);
                    if(time_now_tt - dir_time_tt > 172800)
                    {
                        return lenth;
                    }
                }
            }
        }
        else if(p_lenth == 10)
        {
            if(*p == '2' && *(p + 1) == '0')
            {
                char dir_name[16] = "";
                memcpy(dir_name, p, 10);
                dir_name[10] = 0;
                tm dir_time_tm = {0};
                if(strptime(dir_name, "%Y-%m-%d", &dir_time_tm) != NULL)
                {
                    time_t dir_time_tt = mktime(&dir_time_tm);
                    time_t time_now_tt = time(0);
                    if(time_now_tt - dir_time_tt > 172800)
                    {
                        return lenth;
                    }
                }
            }
        }
        m_qFilelist.push(std::make_pair(one_dir, is_dir));
    }
    else
    {
      //  printf("ondir=%s\none_dir.length()=%d\nm_sExtension=%s\nm_sExtension.length()=%d\nm_sPrefix=%s\nm_sPrefix.length()=%d\none_dir.substr(part_lenth, m_sPrefix.length())=%s\n",
    //           one_dir.c_str(), one_dir.length(), m_sExtension.c_str(), m_sExtension.length(),m_sPrefix.c_str(), m_sPrefix.length(),one_dir.substr(part_lenth, m_sPrefix.length()).c_str());
        if (one_dir.length() > m_sExtension.length()
                && one_dir.substr(one_dir.length() - m_sExtension.length()) == m_sExtension
                && one_dir.length() > m_sPrefix.length()
                && one_dir.substr(part_lenth, m_sPrefix.length()) == m_sPrefix
                )
        {
            
            if (m_doneFiles->InsertOneFile(one_dir))
            {
                m_qFilelist.push(std::make_pair(one_dir, is_dir));
            }
        }
    }

    return lenth;
}

int FTPDriver::ScanDir(const char* tar_dir)
{
    char url[PATH_MAX] = "";
    sprintf(url, "%s/%s/", m_sFtpUrl.c_str(), tar_dir);
    curl_easy_setopt(m_ftp_scan_handle, CURLOPT_URL, url);
    int rc = curl_easy_perform(m_ftp_scan_handle);
    if (rc != CURLE_OK)
    {
        LOG_ERROR("文件扫描动作失败[%s]", url);
        error_format(rc);
    }
    return 0;
}

int FTPDriver::Download()
{
    int rc = CURLE_OK;
    while (!m_qFilelist.empty())
    {
        strcpy(m_ftp_file.remote_file_dir, m_qFilelist.front().first.c_str());

        if (m_qFilelist.front().second == 1)
        {
            ScanDir(m_ftp_file.remote_file_dir);
        }
        else
        {
            char url[PATH_MAX] = "";
            sprintf(url, "%s/%s", m_sFtpUrl.c_str(), m_ftp_file.remote_file_dir);
            Open_local_file();
            curl_easy_setopt(m_ftp_download_handle, CURLOPT_URL, url);
            rc = curl_easy_perform(m_ftp_download_handle);
            if (rc != CURLE_OK)
            {
                LOG_ERROR("文件下载动作失败[%s]", url);
                error_format(rc);
                m_doneFiles->EraseOneFile(string(url));
            }
            else
            {

            }
            Close_local_file();
        }

        m_qFilelist.pop();
    }
    return 0;
}

void FTPDriver::error_format(const int errn)
{
    switch (errn)
    {
    case CURLE_OK:
        LOG_ERROR("URLERROR【%d】成功\n", errn);
        break;
    case CURLE_UNSUPPORTED_PROTOCOL:
        LOG_ERROR("URLERROR【%d】您传送给 libcurl 的网址使用了此 libcurl 不支持的协议\n", errn);
        break;
    case CURLE_FAILED_INIT:
        LOG_ERROR("URLERROR【%d】可能是内部错误或问题\n", errn);
        break;
    case CURLE_URL_MALFORMAT:
        LOG_ERROR("URLERROR【%d】网址格式不正确\n", errn);
        break;
    case CURLE_COULDNT_RESOLVE_PROXY:
        LOG_ERROR("URLERROR【%d】指定的代理服务器主机无法解析\n", errn);
        break;
    case CURLE_COULDNT_RESOLVE_HOST:
        LOG_ERROR("URLERROR【%d】指定的远程主机无法解析\n", errn);
        break;
    case CURLE_COULDNT_CONNECT:
        LOG_ERROR("URLERROR【%d】无法通过 connect() 连接至主机或代理服务器\n", errn);
        break;
    case CURLE_FTP_WEIRD_SERVER_REPLY:
        LOG_ERROR("URLERROR【%d】指定的远程服务器可能不是正确的 FTP 服务器\n", errn);
        break;
    case CURLE_REMOTE_ACCESS_DENIED:
        LOG_ERROR("URLERROR【%d】我们无法访问网址中指定的资源\n", errn);
        break;
    case CURLE_FTP_WEIRD_PASS_REPLY:
        LOG_ERROR("URLERROR【%d】在将 FTP 密码发送到服务器后，libcurl 需要收到正确的回复。 此错误代码表示返回的是意外的代码\n", errn);
        break;
    case CURLE_FTP_WEIRD_PASV_REPLY:
        LOG_ERROR("URLERROR【%d】libcurl 无法从服务器端收到有用的结果，作为对 PASV 或 EPSV 命令的响应。 服务器有问题\n", errn);
        break;
    case CURLE_FTP_WEIRD_227_FORMAT:
        LOG_ERROR("URLERROR【%d】 FTP 服务器返回 227 行作为对 PASV 命令的响应。 如果 libcurl 无法解析此行，就会返回此代码\n", errn);
        break;
    case CURLE_FTP_CANT_GET_HOST:
        LOG_ERROR("URLERROR【%d】在查找用于新连接的主机时出现内部错误\n", errn);
        break;
    case CURLE_FTP_COULDNT_SET_TYPE:
        LOG_ERROR("URLERROR【%d】在尝试将传输模式设置为二进制或 ascii 时发生错误\n", errn);
        break;
    case CURLE_PARTIAL_FILE:
        LOG_ERROR("URLERROR【%d】文件传输尺寸小于或大于预期\n", errn);
        break;
    case CURLE_FTP_COULDNT_RETR_FILE:
        LOG_ERROR("URLERROR【%d】 'RETR' 命令收到了不正常的回复，或完成的传输尺寸为零字节\n", errn);
        break;
    case CURLE_QUOTE_ERROR:
        LOG_ERROR("URLERROR【%d】在向远程服务器发送自定义 \"QUOTE\" 命令时，其中一个命令返回的错误代码为 400 或更大的数字（对于 FTP），或以其他方式表明命令无法成功完成\n", errn);
        break;
    case CURLE_HTTP_RETURNED_ERROR:
        LOG_ERROR("URLERROR【%d】如果 CURLOPT_FAILONERROR 设置为 TRUE，且 HTTP 服务器返回 >= 400 的错误代码，就会返回此代码\n", errn);
        break;
    case CURLE_WRITE_ERROR:
        LOG_ERROR("URLERROR【%d】在向本地文件写入所收到的数据时发生错误，或由写入回调 (write callback) 向 libcurl 返回了一个错误\n", errn);
        break;
    case CURLE_UPLOAD_FAILED:
        LOG_ERROR("URLERROR【%d】无法开始上传。 对于 FTP，服务器通常会拒绝执行 STOR 命令。 错误缓冲区通常会提供服务器对此问题的说明\n", errn);
        break;
    case CURLE_READ_ERROR:
        LOG_ERROR("URLERROR【%d】读取本地文件时遇到问题，或由读取回调 (read callback) 返回了一个错误\n", errn);
        break;
    case CURLE_OUT_OF_MEMORY:
        LOG_ERROR("URLERROR【%d】内存分配请求失败。 此错误比较严重，若发生此错误，则表明出现了非常严重的问题\n", errn);
        break;
    case CURLE_OPERATION_TIMEDOUT:
        LOG_ERROR("URLERROR【%d】操作超时。 已达到根据相应情况指定的超时时间\n", errn);
        break;
    case CURLE_FTP_PORT_FAILED:
        LOG_ERROR("URLERROR【%d】FTP PORT 命令返回错误,在没有为 libcurl 指定适当的地址使用时，最有可能发生此问题\n", errn);
        break;
    case CURLE_FTP_COULDNT_USE_REST:
        LOG_ERROR("URLERROR【%d】 FTP REST 命令返回错误。 如果服务器正常，则应当不会发生这种情况\n", errn);
        break;
    case CURLE_RANGE_ERROR:
        LOG_ERROR("URLERROR【%d】服务器不支持或不接受范围请求\n", errn);
        break;
    case CURLE_HTTP_POST_ERROR:
        LOG_ERROR("URLERROR【%d】此问题比较少见，主要由内部混乱引发\n", errn);
        break;
    case CURLE_SSL_CONNECT_ERROR:
        LOG_ERROR("URLERROR【%d】同时使用 SSL/TLS 时可能会发生此错误。 您可以访问错误缓冲区查看相应信息，其中会对此问题进行更详细的介绍\n", errn);
        break;
    case CURLE_BAD_DOWNLOAD_RESUME:
        LOG_ERROR("URLERROR【%d】尝试恢复超过文件大小限制的 FTP 连接\n", errn);
        break;
    case CURLE_FILE_COULDNT_READ_FILE:
        LOG_ERROR("URLERROR【%d】无法打开 FILE:// 路径下的文件。 原因很可能是文件路径无法识别现有文件。 建议您检查文件的访问权限\n", errn);
        break;
    case CURLE_LDAP_CANNOT_BIND:
        LOG_ERROR("URLERROR【%d】LDAP 绑定操作失败\n", errn);
        break;
    case CURLE_LDAP_SEARCH_FAILED:
        LOG_ERROR("URLERROR【%d】LDAP 搜索无法进行\n", errn);
        break;
    case CURLE_FUNCTION_NOT_FOUND:
        LOG_ERROR("URLERROR【%d】找不到必要的 zlib 函数\n", errn);
        break;
    case CURLE_ABORTED_BY_CALLBACK:
        LOG_ERROR("URLERROR【%d】由回调中止,回调向 libcurl 返回了 \"abort\"\n", errn);
        break;
    case CURLE_BAD_FUNCTION_ARGUMENT:
        LOG_ERROR("URLERROR【%d】内部错误。 使用了不正确的参数调用函数\n", errn);
        break;
    case CURLE_INTERFACE_FAILED:
        LOG_ERROR("URLERROR【%d】界面错误。 指定的外部界面无法使用。 请通过 CURLOPT_INTERFACE 设置要使用哪个界面来处理外部连接的来源 IP 地址\n", errn);
        break;
    case CURLE_TOO_MANY_REDIRECTS:
        LOG_ERROR("URLERROR【%d】重定向过多。 进行重定向时，libcurl 达到了网页点击上限。 请使用 CURLOPT_MAXREDIRS 设置上限\n", errn);
        break;
    case CURLE_TELNET_OPTION_SYNTAX:
        LOG_ERROR("URLERROR【%d】 telnet 选项字符串的格式不正确\n", errn);
        break;
    case CURLE_PEER_FAILED_VERIFICATION:
        LOG_ERROR("URLERROR【%d】远程服务器的 SSL 证书或 SSH md5 指纹不正确\n", errn);
        break;
    case CURLE_GOT_NOTHING:
        LOG_ERROR("URLERROR【%d】服务器未返回任何数据，在相应情况下，未返回任何数据就属于出现错误\n", errn);
        break;
    case CURLE_SSL_ENGINE_NOTFOUND:
        LOG_ERROR("URLERROR【%d】找不到指定的加密引擎\n", errn);
        break;
    case CURLE_SSL_ENGINE_SETFAILED:
        LOG_ERROR("URLERROR【%d】无法将选定的 SSL 加密引擎设为默认选项\n", errn);
        break;
    case CURLE_SEND_ERROR:
        LOG_ERROR("URLERROR【%d】无法发送网络数据\n", errn);
        break;
    case CURLE_RECV_ERROR:
        LOG_ERROR("URLERROR【%d】接收网络数据失败\n", errn);
        break;
    case CURLE_SSL_CERTPROBLEM:
        LOG_ERROR("URLERROR【%d】本地客户端证书有问题 \n", errn);
        break;
    case CURLE_SSL_CIPHER:
        LOG_ERROR("URLERROR【%d】无法使用指定的密钥\n", errn);
        break;
    case CURLE_SSL_CACERT:
        LOG_ERROR("URLERROR【%d】无法使用已知的 CA 证书验证对等证书\n", errn);
        break;
    case CURLE_BAD_CONTENT_ENCODING:
        LOG_ERROR("URLERROR【%d】无法识别传输编码\n", errn);
        break;
    case CURLE_LDAP_INVALID_URL:
        LOG_ERROR("URLERROR【%d】 LDAP 网址无效\n", errn);
        break;
    case CURLE_FILESIZE_EXCEEDED:
        LOG_ERROR("URLERROR【%d】超过了文件大小上限 \n", errn);
        break;
    case CURLE_USE_SSL_FAILED:
        LOG_ERROR("URLERROR【%d】请求的 FTP SSL 级别失败\n", errn);
        break;
    case CURLE_SEND_FAIL_REWIND:
        LOG_ERROR("URLERROR【%d】进行发送操作时，curl 必须回转数据以便重新传输，但回转操作未能成功\n", errn);
        break;
    case CURLE_SSL_ENGINE_INITFAILED:
        LOG_ERROR("URLERROR【%d】SSL 引擎初始化失败\n", errn);
        break;
    case CURLE_LOGIN_DENIED:
        LOG_ERROR("URLERROR【%d】远程服务器拒绝 curl 登录\n", errn);
        break;
    case CURLE_TFTP_NOTFOUND:
        LOG_ERROR("URLERROR【%d】在 TFTP 服务器上找不到文件\n", errn);
        break;
    case CURLE_TFTP_PERM:
        LOG_ERROR("URLERROR【%d】在 TFTP 服务器上遇到权限问题 \n", errn);
        break;
    case CURLE_REMOTE_DISK_FULL:
        LOG_ERROR("URLERROR【%d】服务器磁盘空间不足\n", errn);
        break;
    case CURLE_TFTP_ILLEGAL:
        LOG_ERROR("URLERROR【%d】TFTP 操作非法\n", errn);
        break;
    case CURLE_TFTP_UNKNOWNID:
        LOG_ERROR("URLERROR【%d】TFTP 传输 ID 未知\n", errn);
        break;
    case CURLE_REMOTE_FILE_EXISTS:
        LOG_ERROR("URLERROR【%d】文件已存在，无法覆盖\n", errn);
        break;
    case CURLE_TFTP_NOSUCHUSER:
        LOG_ERROR("URLERROR【%d】运行正常的 TFTP 服务器不会返回此错误 \n", errn);
        break;
    case CURLE_CONV_FAILED:
        LOG_ERROR("URLERROR【%d】字符转换失败\n", errn);
        break;
    case CURLE_CONV_REQD:
        LOG_ERROR("URLERROR【%d】调用方必须注册转换回调 \n", errn);
        break;
    case CURLE_SSL_CACERT_BADFILE:
        LOG_ERROR("URLERROR【%d】读取 SSL CA 证书时遇到问题（可能是路径错误或访问权限问题）\n", errn);
        break;
    case CURLE_REMOTE_FILE_NOT_FOUND:
        LOG_ERROR("URLERROR【%d】网址中引用的资源不存在\n", errn);
        break;
    case CURLE_SSH:
        LOG_ERROR("URLERROR【%d】SSH 会话中发生无法识别的错误\n", errn);
        break;
    case CURLE_SSL_SHUTDOWN_FAILED:
        LOG_ERROR("URLERROR【%d】无法终止 SSL 连接 \n", errn);
        break;
    case CURLE_AGAIN:
        LOG_ERROR("URLERROR【%d】Socket is not ready for send/recv wait till it's ready and try again\n", errn);
        break;
    case CURLE_SSL_CRL_BADFILE:
        LOG_ERROR("URLERROR【%d】Failed to load CRL file \n", errn);
        break;
    case CURLE_SSL_ISSUER_ERROR:
        LOG_ERROR("URLERROR【%d】Issuer check failed \n", errn);
        break;
    case CURLE_FTP_PRET_FAILED:
        LOG_ERROR("URLERROR【%d】PRET command failed\n", errn);
        break;
    case CURLE_RTSP_CSEQ_ERROR:
        LOG_ERROR("URLERROR【%d】Mismatch of RTSP CSeq numbers\n", errn);
        break;
    case CURLE_RTSP_SESSION_ERROR:
        LOG_ERROR("URLERROR【%d】Mismatch of RTSP Session Identifiers\n", errn);
        break;
    case CURLE_FTP_BAD_FILE_LIST:
        LOG_ERROR("URLERROR【%d】Unable to parse FTP file list \n", errn);
        break;
    case CURLE_CHUNK_FAILED:
        LOG_ERROR("URLERROR【%d】Chunk callback reported error\n", errn);
        break;
    default:
        LOG_ERROR("URLERROR【%d】未知错误\n", errn);
        break;
    }
}