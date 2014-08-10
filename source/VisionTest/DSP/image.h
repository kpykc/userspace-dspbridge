#ifndef __IMAGE_H__
#define __IMAGE_H__

struct img_struct
{
	unsigned short width;
	unsigned short height;
	unsigned short widthStep;
	unsigned char* imageData;
};

#endif
