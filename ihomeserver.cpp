#include "ihomeserver.h"
#include "ihome.h"



IHomeServer::IHomeServer()
{
    log.Log("IHomeServer::IHomeServer",COMMON,"start");
    bQuit = false;
}

bool IHomeServer::InitServer()  //初始化
{
    int i;
    pthread_attr_t attr;
    pthread_t tid;
    maxThreadNUM = atoi(config.GetKeyValue("max_thread"));
    if(maxThreadNUM <= 0)
    {
        maxThreadNUM = 10;
    }
    //创建TCP服务套接字
    int port = atoi(config.GetKeyValue("listen_port"));
    if(!serverSock.CreateTcpServer(port))
    {
        log.Log("NET",ERROR,"create tcp error");
        return false;
    }

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    //2.创建maxThreadNum个线程
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr,2*1024*1024);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    for(i = 0; i < maxThreadNUM;i++)
    {
        pthread_create(&tid,&attr,IHomeServer::ServerThread,this);

    }

   pause();
   return true;
}
bool IHomeServer::StartServer()	//开始服务
{
   char ip[32];
    int sock = 0;
    while(!bQuit)
    {

        //1.上锁
        pthread_mutex_lock(&mutex);
        //2.accept
        sock =  serverSock.Accept(ip);
        log.Log("IHomeServe::StartServer",DEBUG,ip);
        if(sock < 0)
        {
             pthread_mutex_unlock(&mutex);
            log.Log("IHomeServer::StartServer",ERROR,"accept");
        }
         //3.释放锁
       pthread_mutex_unlock(&mutex);
        //4.创建一个回话处理对象，处理会话
        IHomeSession *session = new IHomeSession;
        session->StartSession(sock);
        delete session;
    }

}
void *IHomeServer::ServerThread(void *arg)   	//线程函数
{
    IHomeServer *server = (IHomeServer *)arg;
    server->StartServer();
    return NULL;
}
