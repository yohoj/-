#ifndef SCONFIG_H
#define SCONFIG_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "slog.h"
using namespace std;

#define ConfigPath "set.init"


struct ConfigNode {
     char key[32];
     char value[32];
     struct ConfigNode *next;
};


class SConfig
{
public:
    SConfig();
    ~SConfig();
    //1. 解析配置文件函数
        bool ParseConfigFile(const char *path);
    //2. 获取某个关键字的值,当没有该关键字时，返回NULL
        const char *GetKeyValue(const char *key);
    //3. 修改配置项
        bool ModifyKey(const char *key, const char *value);
    //4. 保存配置
        bool SaveConfig(const char *path);
        //创建链表
        int CreateNode();
        //添加链表
        int AddList(const char *key,const char *value);
        int display();
        int FreeList();
        int DestroyNode();
private:
        ConfigNode *head;
        ConfigNode *tail;


};

#endif // SCONFIG_H
