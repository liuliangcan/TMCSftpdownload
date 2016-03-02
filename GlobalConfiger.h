/* 
 * File:   GlobalConfiger.h
 * Author: LIU
 *
 * Created on 2016年2月22日, 下午4:21
 */

#ifndef GLOBALCONFIGER_H
#define	GLOBALCONFIGER_H
#include <string>
class GlobalConfiger {
public:
    //单例
    static GlobalConfiger *GetInstance()
    {
            if(m_instance_gc == NULL)
            {
                    m_instance_gc = new GlobalConfiger();
            }
            return m_instance_gc;
    }
    GlobalConfiger();
    GlobalConfiger(const GlobalConfiger& orig);
    virtual ~GlobalConfiger();
    int GetUpdateListHour() const;
    std::string GetDoneListFile() const;
    std::string GetOutputDir() const;
    std::string GetLogDir() const;
    std::string GetSFtpUrl() const;
    std::string GetSUserAuthen() const;
    
    
    int ReadConfig(std::string configFile);
    std::string GetSExtension() const;
    std::string GetSPrefix() const;
private:
    std::string m_logDir;                                                           //日志目录
    std::string m_outputDir;                                                    //输出目录
    std::string m_doneListFile;                                                 //已下载列表文件
    int m_updateListHour;                                                      //几点更新文件
    std::string m_sUserAuthen;                                               //用户名密码("root:tmcsoft@dc"))
    std::string m_sFtpUrl;                                                        //连接地址("ftp://192.168.2.103:22/tmcdata/lsl/ftpdir/HZ/12/"))
    std::string m_sExtension;                                                   //要下载的文件扩展名
    std::string m_sPrefix;                                                         //要下载的文件前缀
    int Check();
    int DirCheck(std::string dir, std::string discribe);
    int StringCheck(std::string str, std::string discribe, int flag);
    //单例
    static GlobalConfiger *m_instance_gc;
    
};

#endif	/* GLOBALCONFIGER_H */

