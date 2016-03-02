
#include <stdio.h>
#include <list>
#include <string>
#include <string.h>
#include "FTPDriver.h"
#include "tools/macro.h"
#include "tools/log.h"
#include "GlobalConfiger.h"

/* define this to switch off the use of ssh-agent in this program */
#undef DISABLE_SSH_AGENT

using namespace std;

int SpliteLogFile(const char* logTag)
{
    char logfile[PATH_MAX] = "";
    char dstfile[PATH_MAX] = "";
    sprintf(logfile, "%s/%s.access", GlobalConfiger::GetInstance()->GetLogDir().c_str(), logTag);
    time_t now = time(0);
    int dnow = now;
    tm tm1 = {0};
    localtime_r(&now, &tm1);
    char day[16];
    strftime(day, sizeof(day) ,"%Y%m%d", &tm1);
    sprintf(dstfile, "%s/%s_%s_%d.access", GlobalConfiger::GetInstance()->GetLogDir().c_str(), logTag, day, dnow);
    RENAME(logfile, dstfile);    
    
    sprintf(logfile, "%s/%s.error", GlobalConfiger::GetInstance()->GetLogDir().c_str(), logTag);
    sprintf(dstfile, "%s/%s_%s_%d.error", GlobalConfiger::GetInstance()->GetLogDir().c_str(), logTag, day, dnow);
    RENAME(logfile, dstfile);  
    return 0;
}
int f_main(int argc, char** argv)
{
    LOG_NOTICE("程序启动");
    char configXML[PATH_MAX] = "./ConfigFTP.xml";
    if (argc > 1)
    {
        strcpy(configXML, argv[1]);
    }
    GlobalConfiger *gc = GlobalConfiger::GetInstance();
    if (gc->ReadConfig(configXML) < 0)
        return -1;
    
    const char* logTag = "FTPDownload";
    bool flag = log_init(LL_NOTICE, logTag, gc->GetLogDir().c_str());
    
    time_t referSplitLogTime =  (time(0) + 28800) / 86400 * 86400 - 28800 + gc->GetUpdateListHour() * 3600;
   
    while(flag)
    {
        LOG_NOTICE("启动扫描下载");
        fprintf(stdout, "启动扫描下载\n");
        FTPDriver fd;
        fd.init();

        fd.ScanDir("");
        fd.Download();        
        LOG_NOTICE("结束扫描下载");
        fprintf(stdout, "结束扫描下载\n");
        fflush(stdout);
        sleep(60);
        
        if(time(0) > referSplitLogTime)
        {
            
                SpliteLogFile(logTag);
                referSplitLogTime += 86400;
            
        }
    }


    //    log_init(LL_TRACE, "mysql", "./log/");
    //    LOG_NOTICE("%s [time:%d]", "test calling log", time(NULL));
    //    LOG_DEBUG("debug msg,only write to log when the loglevel bigger than or equal to debug [time:%d]", time(NULL));
    //    LOG_WARN("warnning msg will be writing to the error files [time:%d]", time(NULL));
    //    LOG_ERROR("you also can change  number of output files by rewrite the macro_define.h");
    return 0;
}

int main(int argc, char **argv)
{
    return f_main(argc, argv);
}
