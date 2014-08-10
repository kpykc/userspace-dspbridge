#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
// Video
#include "v4l/video.h"

#define VIDEO_IN_W	320
#define VIDEO_IN_H	240

#define NB	180

unsigned int cnt = 0;
unsigned char *photos[NB];
unsigned int i = 0,j,k;

unsigned int start = 0;

unsigned char *in,*out;

int main(int argc, char **argv)
{	
	//Image structures initailization
	struct img_struct* img_new = malloc(sizeof(struct img_struct));
	img_new->w = VIDEO_IN_W;
	img_new->h = VIDEO_IN_H;
	img_new->buf = (unsigned char*)malloc(2*VIDEO_IN_W*VIDEO_IN_H);

	//Video streaming initilization
	// Video Input
	struct vid_struct vid;
	vid.device = (char*)"/dev/video2";
	vid.w=VIDEO_IN_W;
	vid.h=VIDEO_IN_H;
	vid.n_buffers = 4;
	if (video_init(&vid)<0) 
	{
		printf("Error initialising video\n");
		return -1;
	}

	for(i=0;i<NB;i++)
	{
		photos[i] = (unsigned char*)malloc(VIDEO_IN_W*VIDEO_IN_H);
		if(photos[i]==0)
		{
			printf("NO MEMORY\n");
			return -1;
		}
	}

	while(cnt<NB)
	{
		//Get picture
		video_grab_image(&vid, img_new);
		
		if(start>60)
		{
			//Copy grayscale
			in = img_new->buf+1;
			out = photos[cnt];
			for(j=0;j<VIDEO_IN_H;j++)
			{
				for(i=0;i<VIDEO_IN_W;i++)
				{
					//photos[cnt][i+VIDEO_IN_W*j] = img_new->buf[2*(i+j*VIDEO_IN_W)];
					*(out++) = *(in+=2);
				}
			}

			cnt++;
		}
		else
		{
			start++;
		}
	}
	//Save to files
	printf("Saving...\n");
	char buff[10];
	FILE *file;
	unsigned char *img;
	for(i=0;i<cnt;i++)
	{
		sprintf(buff, "%d", i);
		file = fopen(buff,"w");
		img = photos[i];
		for(j=0;j<VIDEO_IN_H;j++)
		{
			for(k=0;k<VIDEO_IN_W;k++)
			{
				fwrite(&img[k+VIDEO_IN_W*j],1,1,file);
			}
		}
		fclose(file);
		free(photos[i]);
	}
	video_close(&vid);
	return 0;
}

