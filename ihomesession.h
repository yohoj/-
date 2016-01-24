#ifndef IHOMESESSION_H
#define IHOMESESSION_H
#include "protocol.h"
#include "ierror.h"
#include "md5.h"
#include "ssocket.h"
#include "video.h"
#include <iostream>
#include <xpath_static.h>
using namespace std;

class IHomeSession
{
public:
    IHomeSession();
    bool StartSession(int sock);
    bool GetRequest(Request *req,int sock);
    bool DealRequest(Request *req,int sock);
private:
    bool Auth(Request *req,int sock);
    bool GetTW(Request *req,int sock);
    bool LedControl(Request *req,int sock);
    bool LedState(Request *req,int sock);
    bool Video(Request *req,int sock);
    bool VideoEnd(Request *req,int sock);
    bool VideoStream(Request *req,int sock);
    bool Voice(Request *req,int sock);
    bool VoiceEnd(Request *req,int sock);
    bool VoiceStream(Request *req,int sock);
    bool Quit(Request *req,int sock);
    bool SendResponse(Request *req,int code,int sock);
private:

    bool bQuit;
    bool   authOK;
    TiXmlDocument *document;
    TiXmlElement *root;
    SSocket serverSock;
    //ZigbeeCom zigbee;
    video cam;
    bool stopped;

};

#endif // IHOMESESSION_H
