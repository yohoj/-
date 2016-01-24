#include "yuv2rgb.h"

int sign3;

int pix_yuvtorgb(int y, int u, int v)
{
	unsigned int pixel24 = 0;
	unsigned char *pixel = (unsigned char *)&pixel24;
	int r, g, b;
	static long int ruv, guv, buv;
	if(sign3)
	{
		sign3 = 0;
		ruv = 1159*(v-128);
		guv = 380*(u-128) + 813*(v-128);
		buv = 2018*(u-128);
	}
	r = (1164*(y-16) + ruv) / 1000;
	g = (1164*(y-16) - guv) / 1000;
	b = (1164*(y-16) + buv) / 1000;

	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;

	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;

	return pixel24;
}

int YuvToRgb(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
	unsigned int in, out;
	int y0, u, y1, v;
	unsigned int pixel24;
	unsigned char *pixel = (unsigned char *)&pixel24;
	unsigned int size = width*height*2;

	for(in = 0, out = 0; in < size; in += 4, out += 6) {
		y0 = yuv[in+0];
		u  = yuv[in+1];
		y1 = yuv[in+2];
		v  = yuv[in+3];

		sign3 = 1;
		pixel24 = pix_yuvtorgb(y0, u, v);
		rgb[out+0] = pixel[0];    //for QT
		rgb[out+1] = pixel[1];
		rgb[out+2] = pixel[2];
		//rgb[out+0] = pixel[2];  //for iplimage
		//rgb[out+1] = pixel[1];
		//rgb[out+2] = pixel[0];
		//sign3 = true;
		pixel24 = pix_yuvtorgb(y1, u, v);
		rgb[out+3] = pixel[0];
		rgb[out+4] = pixel[1];
		rgb[out+5] = pixel[2];
		//rgb[out+3] = pixel[2];
		//rgb[out+4] = pixel[1];
		//rgb[out+5] = pixel[0];
	}

	return 0;
}


