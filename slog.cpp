#include "slog.h"
#include<fstream>

SLog::SLog()
{
    fstream file;
    file.open("logs",ios::in);
    if(!file)
    {
        system("mkdir ./logs");
    }
    fd = -1;
}

SLog::~SLog()
{
   close(fd);
}

char *SLog::GetTime()
{
    time_t rawtime;

    struct tm * timeinfo;

    time( &rawtime );

    timeinfo = localtime ( &rawtime );

    return asctime (timeinfo);

}

bool SLog::Init(int level)
{
    static int month = -1;
    time_t t;
    struct tm tm;
    t = time(NULL);
    localtime_r(&t,&tm);
    if(level <0)
    {
        return false;
    }

    if(month != tm.tm_mon)
    {
        if(fd != -1)
        {
            close(fd);
            fd == -1;
        }
        sprintf(filename,"%s/ihome_%04d%02d.log",logPath,tm.tm_year+1900,tm.tm_mon+1);
       fd = open(filename,O_RDWR|O_CREAT,0666);
       if(fd == -1)
       {
           perror("error");
           return false;
       }
    }

   month = tm.tm_mon;
    return true;
}
int SLog::SetOutputStd(bool b)
{

    if(b == true)
    {
        cout<<log<<endl;
    }

    return 0;
}
//    3. 写日志
// modename - 模块名称
// leve – 日志级别
// fmt – 格式化的字符串（日志内容)
bool SLog::Log(const char *modname, int level,  const char *fmt, ...)
{

    bool b = false;
    if(Init(level) == false)
    {
        return false;
    }
    time_t t;
    struct tm tm;
    t = time(NULL);
    localtime_r(&t,&tm);
    char curtime[32];
    int len = 0;
    va_list ap;
    strcpy(curtime,GetTime());
    curtime[strlen(curtime)-1] = '\0';
    char LEVEL[8];
    switch(level)
    {
    case DEBUG:
        strcpy(LEVEL,"DEBUG");
        break;
    case COMMON:
        strcpy(LEVEL,"COMMON");
        break;
    case WARNING:
        strcpy(LEVEL,"WARNING");
        break;
    case ERROR:
        strcpy(LEVEL,"ERROR");
        break;
    }
    len = snprintf(log,MAX_LOG_LENGTH,"[%04d-%02d-%02d %02d:%02d:%02d][%s][%s]:",
                   tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,modname,LEVEL);
    va_start(ap,fmt);
    len = vsnprintf(log+len,MAX_LOG_LENGTH-len,fmt,ap);
    va_end(ap);
    if(level == DEBUG)
    {
        cout<<log<<endl;
        return true;
    }
    if(level >= 2)
    {
        cout<<log<<endl;
    }
   if(lseek(fd,0,SEEK_END) == -1)
   {
       fd = open(filename,O_RDWR|O_CREAT,0666);
       lseek(fd,0,SEEK_END);
   }
   len = write(fd,log,strlen(log));
   if(len <= 0)
   {
       perror("write");
       return false;
   }
   write(fd,"\n",strlen("\n"));

   //  SetOutputStd(b);
    return true;
}


