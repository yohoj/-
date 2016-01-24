#include "ihomesession.h"
#include "ihome.h"

IHomeSession::IHomeSession()
{
    bQuit = false;
    authOK = false;
    stopped = false;

}
bool IHomeSession::StartSession(int sock)
{
    Request *req;
    req = new Request;
    while(!bQuit)
    {
        if(!GetRequest(req,sock))
        {
            break;
        }
        DealRequest(req,sock);
       if(req->bodyLen > 0)
        {
            delete []req->body;
        }
    }
    return true;
}

bool  IHomeSession::GetRequest(Request *req,int sock)
{
    int size;
    size = serverSock.ReadN(req,HEAD_SIZE,sock);
    if(size != HEAD_SIZE)
    {
        return false;
    }
  if(req->bodyLen == 0)
  {
    return true;
  }
    req->body = new char[req->bodyLen];
   if(serverSock.ReadN(req->body,req->bodyLen,sock) != req->bodyLen)
   {
        delete []req->body;
       return false;
   }
    ((char *)req->body)[req->bodyLen] = '\0';
   log.Log("SES",DEBUG,"Req: cmdNo:%u, seq:%u, bodylen:%u",req->cmdNo,req->seqNo,req->bodyLen);
    return true;
}

bool  IHomeSession::DealRequest(Request *req,int sock)
{

    if(!authOK && req->cmdNo != AUTH_REQ)
    {
         log.Log("SES",DEBUG,"Req: cmdNo:%u,login failed auth false",req->cmdNo);
        return false;
    }
    document = new  TiXmlDocument;
    if(req->bodyLen > 0)
    {
    document->Parse((char *)req->body);
    root = document->RootElement();
    }
    switch(req->cmdNo)
    {
    case AUTH_REQ :     //认证请求
         log.Log("SES",DEBUG,"Req: cmdNo:%u,start login %d",req->cmdNo,sock);
        Auth(req,sock);
        break;
    case VIDEO_REQ:         //视频请求
         log.Log("SES",DEBUG,"Req: cmdNo:%u,start video %d",req->cmdNo,sock);
        Video(req,sock);
        break;
    case VIDEO_END_REQ:         //结束视频请求
        VideoEnd(req,sock);
        break;
    case VIDEO_STM:             //视频流请求
        VideoStream(req,sock);
        break;
    case  QUIT_REQ:             //退出请求
        Quit(req,sock);
        break;
    default:
        break;
    }
    return true;
}
bool IHomeSession:: Auth(Request *req,int sock)
{

    TiXmlString user;
    TiXmlString pass;
    char passwd[33];
    if(!TinyXPath::o_xpath_string(root,"/REQUEST/USER/text()",user) || !TinyXPath::o_xpath_string(root,"/REQUEST/PASS/text()",pass))
    {
        SendResponse(req,ERR_USER_PASS,sock);
             return false;
    }
   const char *ptr = config.GetKeyValue("password");
   int len = strlen(ptr);
   MD5Encode((void *)ptr,len,passwd);
   if(strcmp(user.c_str(),config.GetKeyValue("username")) == 0 && strcmp(pass.c_str(),passwd) == 0)
    {

            authOK = true;
            SendResponse(req,RESPONSE_OK,sock);
            log.Log("SES",DEBUG,"User %s login success %d",user.c_str(),sock);
            return true;
    //发送一个成功的应答
    }
    //发送一个认证失败
   SendResponse(req,1,sock);
    log.Log("SES",DEBUG,"User %s login failed",user.c_str());
    return true;
}

bool IHomeSession:: Video(Request *req,int sock)
{

//    TiXmlString video_id;
//    if(!TinyXPath::o_xpath_string(root,"/REQUEST/VIDEO_ID/text()",video_id))
//    {
//        SendResponse(req, ERR_ACCESS_FILE,sock);
//        return false;
//    }
     req->cmdNo |= VIDEO_RSP;
    FRAME_BUFFER_S frame; //帧结构体，用于保存抓取到的图像
    int frameSize;

    //1. 打开摄像头
    if(cam.__VIDEO_Open("/dev/video0", 320, 240, V4L2_PIX_FMT_YUYV) < 0) {
        return false;
    }

    //分配内存，图像的颜色RGB，可以使用3个字节来表示，
    frame.start = (char *)malloc(320 * 240 * 3);

    int size, count;
//    SendResponse(req,RESPONSE_OK,sock);
   while(!stopped) {
        //2. 抓取一张图像
        frameSize = cam.__VIDEO_GrabFrame(&frame);

//		printf("Grab a image, size:%d\n", frameSize);

        //3. 发送给客户端
         req->bodyLen = frameSize;
        if(serverSock.Write(req,HEAD_SIZE,sock) < 0) {
              log.Log("VIDEO_SEND",ERROR,"send req");
            break;
        }
        count = 0;
        while(count < frameSize) {
            if(serverSock.Write(frame.start+count, frameSize-count, sock) <0) {
                log.Log("VIDEO_SEND",ERROR,"send img");
                break;
            }
            count = count + frameSize-count;
        }
        if(count != frameSize) {
           break;
        }
        //4. 回到第2步继续运行
    }
    // 释放分配的内存
    free(frame.start);
    // 5. 关闭摄像头
    cam.__VIDEO_Close();
    return true;
}
bool IHomeSession:: VideoEnd(Request *req,int sock)
{
    cout<<"end"<<endl;
    stopped = true;
    req->cmdNo = VIDEO_END_RSP;
    req->bodyLen = HEAD_SIZE;
    serverSock.Write(req,req->bodyLen,sock);
    return true;
}
bool IHomeSession:: VideoStream(Request *req,int sock)
{
    TiXmlString video_id;
    if(!TinyXPath::o_xpath_string(root,"/REQUEST/VIDEO_ID/text()",video_id))
    {

        SendResponse(req, ERR_ACCESS_FILE,sock);
             return false;
    }
    char buf[200];
    int len = sprintf(buf,"<?xml version=”1.0”>"
                   "<RESPONSE>"
                    "  <RESULT_CODE>0</RESULT_CODE>"
                        "  <VIDEO_ID>1</VIDEO_ID>"
                        "  <SESSION>1389</SESSION>"
                   " </RESPONSE>"
                   );
     req->cmdNo |= 0x900000;
     req->bodyLen = len;
     serverSock.Write(req,HEAD_SIZE,sock);
     serverSock.Write(buf,len,sock);
    return true;
}

bool IHomeSession:: Quit(Request *req,int sock)
{
    return true;
}

bool IHomeSession::SendResponse(Request *req,int code,int sock)
{
    int len;
    char *resultString[] = {
   " success",
    "username or passwd incorrect",
    "open file failed",
    "Database access failed",
    "open zigbee module failed",
    };
    char buf[200];
    len = sprintf(buf,"<?xml version=\"1.0\"?>"
               "<RESPONSE>"
                "<CODE>%d</CODE>"
                "<DESC>%s</DESC>"
                "</RESPONSE>",
                code,resultString[code]);
    req->cmdNo |= 0x900000;
    req->bodyLen = len;
    serverSock.Write(req,HEAD_SIZE,sock);
    serverSock.Write(buf,len,sock);
}
