/*
 * HistImg.c
 *
 *  Created on: 3 juil. 2014
 *      Author: maxime
 */
#include <stdio.h>
#include <stdlib.h>

#include "image.h"

extern unsigned int *histTemp;

void ImgHist(struct img_struct *img, unsigned char *hist, unsigned char *histCum)
{
	unsigned int max = 0;
	int i,j;

	memset(histTemp, 0, 256*sizeof(unsigned int));

	//Histogramme
	for(j=0;j<img->height;j++)
	{
		for(i=0;i<img->width;i++)
		{
			histTemp[(unsigned char)(img->imageData[2*i+j*img->widthStep])]++;
		}
	}

	for(i=0;i<256;i++)
	{
		if(histTemp[i]>max)
			max = histTemp[i];
	}

	for(i=0;i<256;i++)
	{
		hist[i] = (255*histTemp[i])/max;
	}

	//Histogramme cumulé
	histTemp[0] = hist[0];
	for(i=1;i<256;i++)
	{
		histTemp[i] = hist[i] + histTemp[i-1];
	}

	for(i=0;i<256;i++)
	{
		histCum[i] = (255*histTemp[i])/histTemp[255];
	}
}

void EgaliserHist(struct img_struct *img, unsigned char *hist, unsigned char *histCum)
{
	int i,j;
	
	ImgHist(img,hist,histCum);

	for(j=0;j<img->height;j++)
	{
		for(i=0;i<img->width;i++)
		{
			img->imageData[2*i+j*img->widthStep] = histCum[(unsigned char)(img->imageData[2*i+j*img->widthStep])];
		}
	}
}


