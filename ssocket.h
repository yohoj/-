#ifndef SSOCKET_H
#define SSOCKET_H
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include "slog.h"
#include <iostream>


using namespace std;


class SSocket
{
public:
    SSocket();

    bool Connect(const char *ip, int port);//    1. 创建一个TCP连接的套接字

    bool CreateTcpServer(int port);//    2. 创建一个TCP的服务套接字，并且绑定指定的端口

    int Accept(char *remoteIp);   //    3. 接收一个客户端连接，返回一个套接字

    int SetTimeout(int ms); //    4. 设置超时时间

    int Read(void *buf, int size,int sock);  //        5. 接收数据，当timeout不为0时，要求超时接收

    int ReadN(void *buf, int nBytes,int sock); //        6. 接收指定字节数的数据

    int Write(void *buf, int size,int sock); //        7. 发送数据

    void Close(); //        8. 关闭套接字

    int output();

private:
    struct sockaddr_in clientaddr;
    struct sockaddr_in servaddr;
    int clientsock;
    int servsock;
    struct timeval timeout;
};
#endif // SSOCKET_H
