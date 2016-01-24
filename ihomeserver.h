#ifndef IHOMESERVER_H
#define IHOMESERVER_H
#include <pthread.h>
#include <iostream>
#include "slog.h"
#include"ihomesession.h"
using namespace std;

class IHomeServer
{
public:
    IHomeServer();
    bool InitServer();//初始化
    bool StartServer();	//开始服务
    static void *ServerThread(void *arg); 	//线程函数
    void Quit()
    {
        bQuit = true;
    }
private:
    int maxThreadNUM;
    pthread_t threaded;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool bQuit;
    SSocket serverSock;

};

#endif // IHOMESERVER_H
