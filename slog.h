#ifndef SLOG_H
#define SLOG_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
using namespace std;

//调试日志(0)，一般日志(1)，警告日志(2)，错误日志(3)
#define DEBUG 0
#define COMMON 1
#define WARNING 2
#define ERROR 3
#define logPath "./logs"

#define MAX_LOG_LENGTH 1024

class SLog
{
public:
    SLog();
    ~SLog();
//    1. 初始化日志，判断日志目录./logs存不存在，不存在则创建目录./logs
        //level –  日志输出级别，低于该级别的日志不会记录
        bool Init(int level);
//    2. 设置日志是否需要输出到屏幕(stderr)，当设置了输出时，日志除了会写入到文件，还需要在stderr上输出
        //b – 为TRUE则输出到屏幕，否则不输出
        int SetOutputStd(bool b);
//    3. 写日志
        bool Log(const char *modname, int level,  const char *fmt, ...);
        char* GetTime();
private:
        int level;
        char log[1024];
        char filename[32];
        int fd;

};

#endif // SLOG_H
