#ifndef PROTOCOL_H
#define PROTOCOL_H

#define HEAD_SIZE 16
#define AUTH_REQ 0x000001
#define AUTH_RSP 0x900001
#define VIDEO_REQ 0x000002
#define VIDEO_RSP 0x900002
#define VIDEO_STM 0x000003
#define VIDEO_END_REQ 0x000004
#define VIDEO_END_RSP 0x900004
#define QUIT_REQ 0x000005
#define QUIT_RSP 0x900005

struct Request{
    unsigned int version;
    unsigned int cmdNo;
    unsigned int seqNo;
    unsigned int bodyLen;
    void *body;
};

#endif // PROTOCOL_H
