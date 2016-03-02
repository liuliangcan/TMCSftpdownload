/* 
 * File:   DoneFiles.h
 * Author: LIU
 *
 * Created on 2016年2月22日, 上午10:57
 */

#ifndef DONEFILES_H
#define	DONEFILES_H

#if __cplusplus < 201103L
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#define UNORDERED_MAP tr1::unordered_map
#define UNORDERED_SET tr1::unordered_set
#include <time.h>
#include <string>
#else

#include <unordered_map>
#include <unordered_set>
#define UNORDERED_MAP unordered_map
#define UNORDERED_SET unordered_set
#endif

class DoneFiles {
public:
    DoneFiles();
    DoneFiles(const DoneFiles& orig);
    virtual ~DoneFiles();
    //单例
    static DoneFiles *GetInstance() {
        if (m_df_instance == NULL) {
            m_df_instance = new DoneFiles();
        }
        return m_df_instance;
    }
        
    int EndCollect();                               //结束一次遍历时，更新done_files,删除服务器上不存在的文件
    bool InsertOneFile(std::string file);   //查询插入一个文件，插入成功返回true;已存在不需要插入返回false
    bool EraseOneFile(std::string file);    //移除一个文件
private:

    int ReadFromConfig();                                       //读取本地列表储存文件
    int SaveFiles();                                                    //保存列表到本地
    int SaveOneFile(std::string file);                          //保存一个文件路径进入列表文件
    
    std::UNORDERED_SET<std::string> m_files;            //已下载文件列表
    std::string m_configFile;                                     //本地列表储存文件
    
    bool m_flag_erase;                                            //是否要进行列表更新
    std::UNORDERED_SET<std::string> m_files_swap;   //列表更新时用，储存服务器上存在的所有文件列表
    time_t m_lastEraseTime;                                         //上一次列表更新时间    
    int m_eraseHour;                                                    //几点进行列表更新(2)
        
    static DoneFiles* m_df_instance;
    
};

#endif	/* DONEFILES_H */

