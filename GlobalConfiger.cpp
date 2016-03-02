/* 
 * File:   GlobalConfiger.cpp
 * Author: LIU
 * 
 * Created on 2016年2月22日, 下午4:21
 */

#include "GlobalConfiger.h"
#include "tools/Markup.h"
#include "tools/toolkit.h"
GlobalConfiger* GlobalConfiger::m_instance_gc = NULL;
GlobalConfiger::GlobalConfiger()
{
}

GlobalConfiger::GlobalConfiger(const GlobalConfiger& orig)
{
}

GlobalConfiger::~GlobalConfiger()
{
}

int GlobalConfiger::GetUpdateListHour() const
{
    return m_updateListHour;
}

std::string GlobalConfiger::GetDoneListFile() const
{
    return m_doneListFile;
}

std::string GlobalConfiger::GetOutputDir() const
{
    return m_outputDir;
}

std::string GlobalConfiger::GetLogDir() const
{
    return m_logDir;
}

std::string GlobalConfiger::GetSFtpUrl() const
{
    return m_sFtpUrl;
}

std::string GlobalConfiger::GetSUserAuthen() const
{
    return m_sUserAuthen;
}

int GlobalConfiger::ReadConfig(std::string configFile)
{
    m_logDir = "LOG/";                                                           //日志目录
    m_outputDir = "OUTPUT/";                                                    //输出目录
    m_doneListFile = "FTPDoneFiles";                                                 //已下载列表文件
    m_updateListHour = 2;                                                      //几点更新文件
    m_sUserAuthen = "";                                               //用户名密码("root:tmcsoft@dc"))
    m_sFtpUrl = "";       
    m_sExtension = "";
    m_sPrefix = "";
    CMarkup cmxml;
    bool b = cmxml.Load(configFile.c_str());  
    if(!b)
    {
        fprintf(stderr, "[GlobalConfiger]配置文件读取失败！%s\n",configFile.c_str());
        return -1;
    }
    if (cmxml.FindChildElem("FTPUserAuthen"))
    {
         m_sUserAuthen = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FtpUrl"))
    {
         m_sFtpUrl = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FTPLogDir"))
    {
         m_logDir = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FTPOutputDir"))
    {
         m_outputDir = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FTPDdoneListFile"))
    {
         m_doneListFile = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FTPUpdateListHour"))
    {
         m_updateListHour = atoi(cmxml.GetChildAttrib("name").c_str());
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FTPFileExtension"))
    {
         m_sExtension = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("FTPFilePrefix"))
    {
         m_sPrefix = cmxml.GetChildAttrib("name");
    }
    
    int ret = Check();
    if(ret < 0)
    {
        return ret;
    }
    return 0;
}

std::string GlobalConfiger::GetSExtension() const
{
    return m_sExtension;
}

std::string GlobalConfiger::GetSPrefix() const
{
    return m_sPrefix;
}

int GlobalConfiger::DirCheck(std::string dir, std::string discribe)
{
    if(dir.length() == 0)
    {
        fprintf(stderr, "%s目录\t : [%s]为空，请检查配置文件。\n", discribe.c_str(), dir.c_str());
        return -1;
    }
    else
    {
        if(!F_DirIsExist((char*)dir.c_str()))
        {
            fprintf(stdout,"%s目录\t : [%s]不存在，", discribe.c_str(), dir.c_str());
            int ret = F_MakeDirs((char*)dir.c_str(), 1);
            if(ret != 0)
            {
                fprintf(stdout,"创建失败， 请检查！errno = %d\n", ret);
                return -1;
            }
            else
            {
                fprintf(stdout,"创建成功。\n");
            }
        }
        else
        {
            fprintf(stdout,"%s目录\t : [%s]检测到存在。\n", discribe.c_str(), dir.c_str());
        }
    }
    return 0;
}
int GlobalConfiger::StringCheck(std::string str, std::string discribe, int flag = 1)
{
    if(flag == 1 && str.length() == 0)
    {
        fprintf(stderr, "%s配置为空，请检查配置文件。\n", discribe.c_str());
        return -1;
    }
    else
    {
        fprintf(stdout, "%s\t : [%s]\n", discribe.c_str(), str.c_str());
    }
    return 0;
}
int GlobalConfiger::Check()
{
    int ret = 0;
    ret = StringCheck(m_sFtpUrl, "ftp连接地址");
    if(ret < 0) return ret;    
    ret = StringCheck(m_sUserAuthen, "ftp认证信息");
    if(ret < 0) return ret;        
    ret = StringCheck(m_doneListFile, "已下载文件列表");
    if(ret < 0) return ret;     
    ret = StringCheck(m_sExtension, "文件扩展名");
    if(ret < 0) return ret;     
    ret = StringCheck(m_sPrefix, "文件前缀", 0);
    if(ret < 0) return ret;     
    
    ret = DirCheck(m_logDir, "日志");
    if(ret < 0) return ret;
    ret = DirCheck(m_outputDir, "输出");
    if(ret < 0) return ret;    
    if(m_updateListHour < 0 || m_updateListHour > 23)
    {
       fprintf(stderr, "已下载文件列表更新时间设置错误:\t [%d]点\n", m_updateListHour);
       return -1;
    }
    else
    {        
       fprintf(stderr, "已下载文件列表更新时间:\t [%d]点\n", m_updateListHour);
    }
    return 0;
}