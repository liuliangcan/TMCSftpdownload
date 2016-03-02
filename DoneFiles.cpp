/* 
 * File:   DoneFiles.cpp
 * Author: LIU
 * 
 * Created on 2016年2月22日, 上午10:57
 */

#include "DoneFiles.h"
#include <fstream>
#include <limits.h>
#include "GlobalConfiger.h"
#include "tools/log.h"

using namespace std;

DoneFiles* DoneFiles::m_df_instance = NULL;
DoneFiles::DoneFiles()
{
    m_configFile = GlobalConfiger::GetInstance()->GetDoneListFile();
    m_flag_erase = false;
    ReadFromConfig();
    m_eraseHour = GlobalConfiger::GetInstance()->GetUpdateListHour();
    m_lastEraseTime = (time(0) + 28800) / 86400 * 86400 - 28800 + m_eraseHour * 3600;
    
}

DoneFiles::DoneFiles(const DoneFiles& orig)
{
}

DoneFiles::~DoneFiles()
{
}

int DoneFiles::ReadFromConfig()
{
    m_files.clear();
    ifstream infile(m_configFile.c_str());
    if (infile.fail())
    {
        LOG_ERROR("读取已下载列表文件失败！[%s]", m_configFile.c_str());
    }
    else
    {
        char line[PATH_MAX];
        while (!infile.eof())
        {
            infile.getline(line, PATH_MAX);
            if (line[0] != 0)
                m_files.insert(string(line));
        }

        infile.close();
    }

    return 0;
}

int DoneFiles::SaveFiles()
{
    ofstream outfile(m_configFile.c_str());
    if (outfile.fail())
    {
        LOG_ERROR("更新已下载列表文件失败！[%s]", m_configFile.c_str());
        return -1;
    }
    else
    {
        UNORDERED_SET<string>::iterator it_us = m_files.begin();
        UNORDERED_SET<string>::iterator it_us_end = m_files.end();
        for (; it_us != it_us_end; it_us++)
        {
            outfile << *it_us << "\n";
        }

        outfile.close();
    }
    return 0;
}

int DoneFiles::SaveOneFile(std::string file)
{
    ofstream outfile(m_configFile.c_str(), ios::app);
    if (outfile.fail())
    {
        LOG_ERROR("写入已下载列表文件失败！[%s]", m_configFile.c_str());
        return -1;
    }
    else
    {
        outfile << file << "\n";

        outfile.close();
    }
    return 0;
}
int DoneFiles::EndCollect()
{    
    if(!m_flag_erase) 
    {
        time_t nowTime = time(0);
        if(nowTime > m_lastEraseTime)
        {
                m_flag_erase = true;            
        }
         return 0;
    }
    if(!m_files_swap.empty())
    {
        LOG_NOTICE("已下载列表更新，本地列表%d项，更新为%d项。", m_files.size(), m_files_swap.size());
        m_files.swap(m_files_swap);
        SaveFiles();
        //m_lastEraseTime = time(0);
        m_lastEraseTime += 86400;
        m_files_swap.clear();
    }
    m_flag_erase = false;
    
    return 0;
}
bool DoneFiles::InsertOneFile(std::string file)
{
    if(m_flag_erase)
    {
        m_files_swap.insert(file);
    }
    std::UNORDERED_SET<std::string>::iterator it_find= m_files.find(file);
    if(it_find != m_files.end())
    {//已存在不需要插入
        return false;
    }
    m_files.insert(file);
    SaveOneFile(file);
    if(m_flag_erase)
    {
        m_files_swap.insert(file);
    }
    return true;
}

bool DoneFiles::EraseOneFile(std::string file)
{
    if(m_flag_erase)
    {
        m_files_swap.erase(file);
    }
    m_files.erase(file);
    return true;
}