#include "ssocket.h"
#include "ihome.h"


SSocket::SSocket()
{
    clientsock = socket(AF_INET,SOCK_STREAM,0);
    servsock = socket(PF_INET,SOCK_STREAM,0);
}

bool SSocket::Connect(const char *ip, int port)//    1. 创建一个TCP连接的套接字
{
    log.Log("SSocket::Connect",COMMON,"创建TCP连接的套接字");
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(port);
    inet_pton(AF_INET,ip,&clientaddr.sin_addr);
    if(connect(clientsock,(struct sockaddr*)&clientaddr,sizeof(struct sockaddr)) != 0)
    {
        log.Log("SSocket::Connect",ERROR,"connect error");
        close(clientsock);
        return false;
    }
    return true;
}

bool SSocket::CreateTcpServer(int port)//    2. 创建一个TCP的服务套接字，并且绑定指定的端口
{
    int reuse;
    log.Log("SSocket::CreateTcpServer",COMMON,"创建TCP的服务套接字");
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(servsock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    if(bind(servsock,(struct sockaddr *)&servaddr,sizeof(struct sockaddr)) != 0)
    {
        close(servsock);
        log.Log("SSocket::CreateTcpServer",ERROR,"bind error");
        return false;
    }
    return true;
}

int SSocket::Accept(char *remoteIp)  //    3. 接收一个客户端连接，返回一个套接字
{
    log.Log("SSocket::Accept",COMMON," 接收一个客户端连接");
    listen(servsock,5);
    struct sockaddr_in peer;
    socklen_t len = sizeof(struct sockaddr);
    int acceptsock=accept(servsock,(struct sockaddr *)&peer,&len);
    if(acceptsock <= 0)
    {
        close(servsock);
        log.Log("SSocket::Accept",ERROR,"accept error");
        return 0;
    }
    strcpy(remoteIp,inet_ntoa(peer.sin_addr));
     log.Log("SSocket::Accept",COMMON, remoteIp);

    return acceptsock;
}

int SSocket::SetTimeout(int ms) //    4. 设置超时时间
{
    log.Log("SSocket::SetTimeout",COMMON," 设置超时时间");
    timeout = {ms,0};
    return 0;
}

int SSocket::Read(void *buf, int size,int sock)//        5. 接收数据，当timeout不为0时，要求超时接收
{
    log.Log("SSocket::Read",COMMON," 接收数据");
    SetTimeout(100);
    int ret;
    int len;
    ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
    len = recv(sock,buf,size,0);
    if(ret==0 && len < 0)
    {
        log.Log("SSocket::Read",WARNING,"recv timeout");
        return -1;
    }
     if(len <= 0)
    {
        log.Log("SSocket::Read",ERROR,"recv error");
        return -1;
    }
    return 0;
}

int SSocket::ReadN(void *buf, int nBytes,int sock) //        6. 接收指定字节数的数据
{
     log.Log("SSocket::ReadN",COMMON," 接收指定字节数的数据");
    int len;
    int count = 0;
    char *p = (char *)buf;
    while(1)
    {
        len = recv(sock,p,1,0);
         if(len <= 0)
        {
            return -1;
        }
        count += len;
        if(count == nBytes)
        {
            break;
        }
        p++;
    }
    return count;
}

int SSocket::Write(void *buf, int size,int sock) //        7. 发送数据
{
     log.Log("SSocket::Write",COMMON," 发送数据");
    int len;
    len = send(sock,buf,size,0);
    if(len <= 0)
    {
        log.Log("SSocket::Write",ERROR,"send error");
        return -1;
    }
    return 0;
}

void SSocket::Close()//        8. 关闭套接字
{
     log.Log("SSocket::Close",COMMON,"  关闭套接字");
    close(servsock);
    close(clientsock);
}

int SSocket::output()
{
    return clientsock;
}
