#ifndef _CAMERA_H_
#define _CAMERA_H_
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>            
#include <fcntl.h>             
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <asm/types.h>         
#include <linux/videodev2.h>
//#include <linux/videodev.h>
#include <linux/fb.h>
#include <time.h>

#define VIDEO_DEV "/dev/video0"
#define VIDEO_WIDTH  	480//320
#define VIDEO_HEIGHT 	320//240
#define VIDEO_FORMAT 	V4L2_PIX_FMT_RGB24
#define CLEAR(x)       memset(&x,0,sizeof(x));
using namespace std;
typedef struct _video_info 
{	
	unsigned int length;	// 图像的大小	
	void *start;		// 内存映射的起始地址
}FRAME_BUFFER_S;
class camera
{
	public:
		camera();
		~camera();
		void* jpeg_p;			//图片指针
		int size;		//图片大小
        int fd;
	private:
                        //摄像头描述符
		int cont;					//缓冲区个数
		struct v4l2_capability cap; 		//摄像头能力
		struct v4l2_format fmt;  	// 视频设备支持的视频格式
		struct v4l2_buffer buffer;	// 摄像头缓冲区
		struct v4l2_control ctl;
        //struct v4l2_buffer buf;
		FRAME_BUFFER_S *frames;
		int width;							//宽
		int height;							//高
		int format;							//
		
		bool open_camera();//打开摄像头
		void close_camera();// 关闭摄像头
		bool get_camea_data();//获取摄像头参数
		bool set_camea_data();//设置摄像头参数
		bool get_camea_cap();//获取摄像头能力
		bool camea_video_start();//开启视频捕捉
		bool camea_video_close();//关闭视频捕捉
		bool RequestBuffers();//内存映射
		bool ReleaseFrame();//解除映射
	public:
        bool cam_start();//开启摄像头
        bool cam_stop();//关闭摄像头
        bool get_stream();//获取一张图片
        bool get_stream(char*in_p,int&size);
        bool rep_mmap();

        void operator++();
        void operator++(int);
};
#endif


