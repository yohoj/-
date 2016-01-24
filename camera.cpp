#include"camera.h"
/*******静态数据成员初始化********/
camera:: camera()
{
    jpeg_p = NULL;			//图片指针
    size = 0;
    fd = -1;
   // cam_start();
}
camera:: ~camera()
{
	if(!fd){
		close(fd);
		fd = 0;
	}
}
bool camera:: cam_start()
{
    open_camera();//打开摄像头
    get_camea_data();
    set_camea_data();//设置摄像头参数
    RequestBuffers(); //内存映射
    camea_video_start();
}
bool camera:: cam_stop()
{
    camea_video_close();//关闭视频捕捉流
    ReleaseFrame();//解除内存映射
    close(fd);
    fd = -1;
}
void camera::operator++()
{
    get_stream();
}
void camera:: operator++(int a)
{
    get_stream();
}
//打开摄像头
bool camera::open_camera()
{
    fd = open("/dev/video0",O_RDWR);
	if(fd<0){
		perror("open:");

		return false;
	}
	return true;
}
// 关闭摄像头
void camera::close_camera()
{
		close(fd);
}
//获取摄像头参数
bool camera::get_camea_data()
{
	if(ioctl(fd, VIDIOC_G_CTRL, &ctl)<0){
		return false;
	}
	if(ioctl(fd, VIDIOC_G_CTRL, &ctl)<0){
		return false;
	}
	if(ioctl(fd, VIDIOC_G_CTRL, &ctl)<0){
		return false;
	}
	return true;
}
//设置摄像头参数
bool camera::set_camea_data()
{
	struct v4l2_format fmt;

	CLEAR (fmt);
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 480;//width;
    fmt.fmt.pix.height      = 320;//height;
	fmt.fmt.pix.pixelformat = 90;//format;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;//RGB24
	fmt.fmt.pix.field       = V4L2_FIELD_ANY;

	if(ioctl (fd, VIDIOC_S_FMT, &fmt)<0){
		perror("设置摄像头参数:");
		return false;
	}
	return true;
}
//获取摄像头能力
bool camera::get_camea_cap()
{
	if(ioctl(fd, VIDIOC_QUERYCAP, &cap)<0){
		perror("摄像头能力:");
		return false;
	}
	return true;
}
//开启视频捕捉
bool camera::camea_video_start()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl (fd, VIDIOC_STREAMON, &type)<0) {
		perror("开启视频捕捉:");
		return false;
	}

	return true;
	
}
//关闭视频捕捉
bool camera::camea_video_close()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl (fd, VIDIOC_STREAMOFF, &type)<0) {
		perror("关闭视频捕捉:");
		return false;
	}

	return true;
	
}
//内存映射
bool camera:: RequestBuffers() 
{
	
	int i;
	struct v4l2_requestbuffers req;

	CLEAR (req);
    req.count               = 2;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;
	if(ioctl (fd, VIDIOC_REQBUFS, &req) < 0){
		perror("内存映射1:");
		return false;
	}
	frames = (FRAME_BUFFER_S *)calloc (req.count, sizeof (FRAME_BUFFER_S));
	for (i= 0; i < req.count; ++i){
		struct v4l2_buffer buf;   
		CLEAR (buf);
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;

		if(ioctl (fd, VIDIOC_QUERYBUF, &buf)<0){
			free(frames);
			perror("内存映射2:");
			return false;
		}
		frames[i].length = buf.length;
		frames[i].start = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		if (MAP_FAILED == frames[i].start){
			free(frames);
			perror("内存映射3:");
			return false;
		}
		memset(frames[i].start, 0, buf.length); 
	}
	for (i = 0; i < req.count; ++i){
		struct v4l2_buffer buf;
		CLEAR (buf);
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;
		if (ioctl (fd, VIDIOC_QBUF, &buf)<0) {
			perror("内存映射4:");
			return false;
		}
	}
	return true;
}
//解除映射
bool camera:: ReleaseFrame()
{
	if(ioctl (fd, VIDIOC_QBUF, frames)<0){
		return false;
	}
	return true;
}
//获取一张图片
bool camera:: get_stream()
{
    struct v4l2_buffer buf;
	CLEAR (buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	if( ioctl (fd, VIDIOC_DQBUF,&buf ) < 0){//从缓冲队列取一帧图像出来
		perror("获取一张图片:");
		return false;
	}

    ioctl(fd, VIDIOC_QBUF, &buf);//重新放入缓存队列
    jpeg_p = frames[buf.index].start;
    size = buf.bytesused;
    cout<<"start";
    FILE *fp = fopen("./h.jpg","w");
    fwrite(jpeg_p,size,1,fp);
    fclose(fp);
    ioctl(fd, VIDIOC_QBUF, &buf);//重新放入缓存队列
	return true;
}
bool camera:: get_stream(char*in_p,int&size)
{/*
    struct v4l2_buffer buf;
    CLEAR (buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if( ioctl (fd, VIDIOC_DQBUF,&buf ) < 0){//从缓冲队列取一帧图像出来
        perror("获取一张图片:");
        return false;
    }
    ioctl(fd, VIDIOC_QBUF, &buf);//重新放入缓存队列

    jpeg_p = frames[buf.index].start;
    size = buf.bytesused;
    memcpy(in_p,frames[buf.index].start,buf.bytesused);
    ioctl(fd, VIDIOC_QBUF, &buf);//重新放入缓存队列
    */
    //char *buf = new char[46620];
    FILE *p = fopen("/home/f/1.jpeg","r");

    fread(in_p,46620,1,p);
    fclose(p);
    return true;
}
bool camera::rep_mmap()
{
    //ioctl(fd, VIDIOC_QBUF, &buf);//重新放入缓存队列
}

