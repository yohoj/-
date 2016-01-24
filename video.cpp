#include "video.h"

int video::__VIDEO_Open(const char *devName, int width, int height, int fmt)
{
	struct v4l2_capability cap; 
	unsigned int i;
//	enum v4l2_buf_type type;
//	struct v4l2_fmtdesc fmt1;
	int cameraFd;
	int ret;

	//打开视频设备
	cameraFd = open (devName, O_RDWR);
	if (cameraFd == -1) {
		perror("Open");
		return VIPP_ERR;
	}

	ret = ioctl (cameraFd, VIDIOC_QUERYCAP, &cap);
	if(ret < 0){
		perror("QUERYCAP");
		goto OPEN_ERROR;
	}
    this->cameraFd = cameraFd;

	//设置视频格式
    format = fmt;
	if(VIPP_OK != __VIDEO_SetCamParam(width, height, fmt)) {
		perror("SET_CAM_PARAM");
		goto OPEN_ERROR;
	}

	//申请缓冲
	if(VIPP_OK != __VIDEO_RequestBuffers()) {
		perror("REQUEST_BUFFERS");
		goto OPEN_ERROR;
	}

	//获取当前视频的相关参数
    this->width = width;
    this->height = height;
    this->brightness = VIDEO_GetBrightness(cameraFd);
    this->contrast = VIDEO_GetContrast(cameraFd);
    this->hue = VIDEO_GetHue(cameraFd);
    this->saturation = VIDEO_GetSaturation(cameraFd);

	//开启视频捕捉
	__VIDEO_StreamOn();

	return cameraFd;

OPEN_ERROR:
	close(cameraFd);
    this->cameraFd = -1;
	return -1;
}

int video::__VIDEO_StreamOn()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl (cameraFd, VIDIOC_STREAMON, &type)) {
		return VIPP_ERR;
	}

	return VIPP_OK;
}

//关闭视频流
int video::__VIDEO_StreamOff()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == ioctl (cameraFd, VIDIOC_STREAMOFF, &type)) {
		return VIPP_ERR;
	}

	return VIPP_OK;
}

int video::__VIDEO_RequestBuffers()
{
	int i;
    int cameraFd = this->cameraFd;
	struct v4l2_requestbuffers req;
	FRAME_BUFFER_S *frames;

	CLEAR (req);
	req.count               = 1;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;
	if(ioctl (cameraFd, VIDIOC_REQBUFS, &req) < 0)
	{
		return VIPP_ERR;
	}

	if (req.count > 0)
	{
        frames = (FRAME_BUFFER_S*)calloc (req.count, sizeof (FRAME_BUFFER_S));
	}

	for (i= 0; i < req.count; ++i){
		struct v4l2_buffer buf;   
		CLEAR (buf);
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;

		if( -1 == ioctl (cameraFd, VIDIOC_QUERYBUF, &buf))
		{
			free(frames);
			return VIPP_ERR;
		}

		frames[i].length = buf.length;
		frames[i].start = mmap (NULL, buf.length, 
				PROT_READ | PROT_WRITE, MAP_SHARED, cameraFd, buf.m.offset);

		if (MAP_FAILED == frames[i].start){
			free(frames);
			return VIPP_ERR;
		}
		memset(frames[i].start, 0, buf.length); 
	}

	for (i = 0; i < req.count; ++i){
		struct v4l2_buffer buf;
		CLEAR (buf);
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;
		if (-1 == ioctl (cameraFd, VIDIOC_QBUF, &buf)) {
		}
	}

    buffers = frames;
	return VIPP_OK;
}

int video::VIDEO_ImageToJpeg(char *img, int w, int h, char *jpeg_data, int quality)
{
	struct jpeg_compress_struct cinfo;	
	struct jpeg_error_mgr jerr;	
	int i, size;	
	unsigned char *line;		

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_buff_dest(&cinfo);
	cinfo.image_width = w;	
	cinfo.image_height = h;
	cinfo.input_components = 3;	
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);	
	jpeg_start_compress(&cinfo, TRUE);
    for (i = 0, line =(u_char*) img; i < h; i++, line += w*3)
		jpeg_write_scanlines(&cinfo, &line, 1);	
	jpeg_finish_compress(&cinfo);
	size = get_jpeg_size(&cinfo);	
	
	/* this memory must be freed elsewhere */
	/*
	(*jpeg_data) = (char *)malloc(size);	
	if(!(*jpeg_data))	
	{	
		jpeg_destroy_compress(&cinfo);
		jpeg_buff_free(&cinfo);
		return -1;	
	}	
	memcpy(*jpeg_data, ((my_dest_ptr)cinfo.dest)->buffer, size);
	*/

	memcpy(jpeg_data, ((my_dest_ptr)cinfo.dest)->buffer, size);
	jpeg_destroy_compress(&cinfo);
	jpeg_buff_free(&cinfo);	

	return size;
}

int video::__VIDEO_GrabFrame(FRAME_BUFFER_S *frame)
{
	int imageSize = -1;
	FRAME_BUFFER_S *buf;
//	FILE *fp;
	unsigned char *rgb ;

    CLEAR (this->frame);
    this->frame.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    this->frame.memory = V4L2_MEMORY_MMAP;

	//从缓冲队列取一帧图像出来
	//printf("Grab\n");
    if( ioctl (cameraFd, VIDIOC_DQBUF, &this->frame) < 0)
	{
		return -1;
	}
	//printf("Grab ok\n");

    buf = &buffers[this->frame.index];
	
    if(format == V4L2_PIX_FMT_YUYV)
	{
        rgb = (unsigned char *)malloc(width * height * 3);
        YuvToRgb((u_char*)buf->start, rgb,width, height);

        imageSize = VIDEO_ImageToJpeg((char *)rgb, width, height, (char *)frame->start, 90);
		free(rgb);
//		fp = fopen("./temp.jpg", "w");
//		fwrite(frame->start, imageSize, 1, fp);
//		fclose(fp);
	}
	else
	{
        imageSize = this->frame.bytesused;
		memcpy(frame->start, buf->start, imageSize);
	}

	//printf("Grab image size:%d\n", imageSize);
    ioctl(cameraFd, VIDIOC_QBUF, &this->frame);

	return imageSize;
}

int video::__VIDEO_ReleaseFrame()
{
    if(-1 == ioctl (cameraFd, VIDIOC_QBUF, &frame))
	{
		printf("error: VIDIOC_QBUF %s\n", __func__);
		return VIPP_ERR;
	}

	return VIPP_OK;
}

//
int video::__VIDEO_SetCamParam(int width, int height, int format)
{
	struct v4l2_format fmt;

	CLEAR (fmt);
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = width; 
	fmt.fmt.pix.height      = height;
	fmt.fmt.pix.pixelformat = format;
	//	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//RGB24
	fmt.fmt.pix.field       = V4L2_FIELD_ANY;

    if(-1 == ioctl (cameraFd, VIDIOC_S_FMT, &fmt))
	{
		perror("VIDEOC_S_FMT");
		return VIPP_ERR;
	}

	return VIPP_OK;
}

int video::__VIDEO_Close()
{
	int i;

	__VIDEO_StreamOff();

	printf("VIDEO streamoff OK\n");
    if(NULL != buffers) {
		for(i=0; i<1; i++) {
            munmap(buffers[i].start, buffers[i].length);
		}
        free(buffers);
        buffers = NULL;
	}

    close(cameraFd);

	return 0;
}

int video::VIDEO_GetCamFd()
{
    return cameraFd;
}

int video::VIDEO_GetBrightness(int fd)
{
	struct v4l2_control ctl = {V4L2_CID_BRIGHTNESS, 0};

	if(-1 == ioctl(fd, VIDIOC_G_CTRL, &ctl)) {
		return -1;
	}

	return ctl.value;
}

int video::VIDEO_SetBrightness(int fd, int br)
{
	struct v4l2_control ctl = {V4L2_CID_BRIGHTNESS, br};

    return ioctl(cameraFd, VIDIOC_S_CTRL, &ctl);
	/*
	struct video_picture videopict;

	if (ioctl (g_VideoInfo.cameraFd, VIDIOCGPICT, &videopict) < 0)
		printf("VIDIOCGPICT error\n");
	videopict.brightness = br;

	if (ioctl (g_VideoInfo.cameraFd, VIDIOCSPICT, &videopict) < 0)
		printf("Couldnt set videopict params with VIDIOCSPICT\n");

	printf ("VIDIOCSPICT brightnes=%d hue=%d color=%d contrast=%d"
			" whiteness=%d depth=%d palette=%d\n", 
			videopict.brightness, videopict.hue, videopict.colour, 
			videopict.contrast,  videopict.whiteness, videopict.depth,
			videopict.palette);
	*/
}

int video::VIDEO_GetContrast(int fd)
{
	struct v4l2_control ctl = {V4L2_CID_CONTRAST, 0};

	if(-1 == ioctl(fd, VIDIOC_G_CTRL, &ctl)) 
	{
		return -1;
	}

	return ctl.value;
}

int video::VIDEO_SetContrast(int fd, int contrast)
{
	struct v4l2_control ctl = {V4L2_CID_CONTRAST, contrast};

	return ioctl(fd, VIDIOC_S_CTRL, &ctl);
}

int video::VIDEO_GetSaturation(int fd)
{
	struct v4l2_control ctl = {V4L2_CID_SATURATION, 0};

	if(-1 == ioctl(fd, VIDIOC_G_CTRL, &ctl)) {
		return -1;
	}

	return ctl.value;
}

int video::VIDEO_SetSaturation(int fd, int saturation)
{
	struct v4l2_control ctl = {V4L2_CID_SATURATION, saturation};

	return ioctl(fd, VIDIOC_S_CTRL, &ctl);
}

int video::VIDEO_PrintParam()
{
    printf("resolution :\t%d x %d\n", width, height);
    printf("brightness :\t%d\n", brightness);
    printf("contrast   :\t%d\n", contrast);
    printf("saturation :\t%d\n", saturation);
    printf("Hue        :\t%d\n", hue);

	return VIPP_OK;
}

int video::VIDEO_GetHue(int fd)
{
	struct v4l2_control ctl = {V4L2_CID_HUE, 0};

	if(-1 == ioctl(fd, VIDIOC_G_CTRL, &ctl)) {
		return -1;
	}

	return ctl.value;
}

int video::VIDEO_SetHue(int fd, int hue)
{
	struct v4l2_control ctl = {V4L2_CID_HUE, hue};

	return ioctl(fd, VIDIOC_S_CTRL, &ctl);
}


void video::startVideo(int sock)
{
   // struct ReqHead head;
    FRAME_BUFFER_S frame; //帧结构体，用于保存抓取到的图像
    int frameSize;

    //head.cmdId = SEND_IMAGE;
    //1. 打开摄像头
    if(__VIDEO_Open("/dev/video0", 320, 240, V4L2_PIX_FMT_YUYV) < 0) {
        return;
    }

    //分配内存，图像的颜色RGB，可以使用3个字节来表示，
    frame.start = (char *)malloc(320 * 240 * 3);
  //  int size, count;
    //while(1) {
        //2. 抓取一张图像
        frameSize = __VIDEO_GrabFrame(&frame);

//		printf("Grab a image, size:%d\n", frameSize);

        //3. 发送给客户端
//        head.bodyLen = frameSize;
//        if(send(sock, &head, sizeof(head), 0) <= 0) {
//            break;
//        }
//        count = 0;
//        while(count < frameSize) {
//            if((size = send(sock, frame.start+count, frameSize-count, 0)) <= 0) {
//                break;
//            }
//            count = count + size;
//        }
//        if(count != frameSize) {
//            break;
//        }
        //4. 回到第2步继续运行
    //}
        FILE *fp;
        fp = fopen("test.jpg", "w");
            fwrite(frame.start, frameSize, 1, fp);
            fclose(fp);
    // 释放分配的内存
    free(frame.start);
    // 5. 关闭摄像头
    __VIDEO_Close();
}
