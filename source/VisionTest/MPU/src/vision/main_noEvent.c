#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
//DSP
#include <dbapi.h>
#include "dsp.h"

// UDP RTP Images
#include "udp/socket.h"
// Threaded computer vision
#include <pthread.h>
// Video
#include "v4l/video.h"
#include "cv/resize.h"

#include "encoding/jpeg.h"
#include "encoding/rtp.h"

#define VIDEO_IN_W	1280
#define VIDEO_IN_H	720

#define DOWNSIZE_FACTOR   4

#define VIDEO_OUT_W	VIDEO_IN_W
#define VIDEO_OUT_H	VIDEO_IN_H

#define SIZEBUFF2DSP	VIDEO_IN_W*VIDEO_IN_H*2
#define SIZEBUFF2MPU	VIDEO_OUT_W*VIDEO_OUT_H*2

int millisleep = 250;
int visionRun = 1;

struct sigaction old_action;

void sigint_handler(int sig_no)
{
    if(sig_no == SIGINT)
	{
		visionRun = 0;
	}
}

extern int DSPVisionOpen(struct VISION_TASK *task, struct img_struct *imgIn, struct img_struct *imgOut);
extern void DSPVisionClose(struct VISION_TASK *task);

int main(int argc, char **argv)
{	

	int i,j;

	int flags = fcntl(0, F_GETFL, 0); /* get current file status flags */
	flags |= O_NONBLOCK; /* turn off blocking flag */
	fcntl(0, F_SETFL, flags);

	int status;
	struct VISION_TASK visionTask;
	
	unsigned long nb = 0;

	/* Messaging used for GPP/DSP synchronization */
	struct DSP_MSG msgToDsp;
	struct DSP_MSG msgFromDsp;

	system("sysctl -w kernel.shmmax=134217728");
	system("sysctl -w kernel.shmall=134217728");
	system("./cexec.out -T ddspbase_tiomap3430.dof64P");

	//Image structures initailization
	struct img_struct* img_new = malloc(sizeof(struct img_struct));
	img_new->w = VIDEO_IN_W;
	img_new->h = VIDEO_IN_H;

	struct img_struct* img_dsp = malloc(sizeof(struct img_struct));
	img_dsp->w = VIDEO_OUT_W;
	img_dsp->h = VIDEO_OUT_H;

	if(DSPVisionOpen(&visionTask,img_new, img_dsp)!=0)
	{
		printf("Fail to open DSP Vision\n");
		return EXIT_FAILURE;
	}

	//Video streaming initilization
	// Video Input
	struct vid_struct vid;
	vid.device = (char*)"/dev/video1";
	vid.w=VIDEO_IN_W;
	vid.h=VIDEO_IN_H;
	vid.n_buffers = 4;
	if (video_init(&vid)<0) 
	{
		printf("Error initialising video\n");
		return EXIT_FAILURE;
	}

	// Video Resizing
	uint8_t quality_factor = 50; // From 0 to 99 (99=high)
	uint8_t dri_jpeg_header = 0;

	int millisleep = 10;

	struct img_struct small;
	small.w = vid.w / DOWNSIZE_FACTOR;
	small.h = vid.h / DOWNSIZE_FACTOR;
	small.buf = (uint8_t*)malloc(small.w*small.h*2);


	// Video Compression
	uint8_t* jpegbuf = (uint8_t*)malloc(vid.h*vid.w*2);

	// Network Transmit
	struct UdpSocket* vsock;
	vsock = udp_socket("192.168.1.255", 5000, 5001, FMS_BROADCAST);
	
	struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);
    
    struct timespec start,end;
	unsigned long diffTime;

	printf("Vision started...\n");

	while(visionRun)
	{
		//usleep(1000 * millisleep);
		diffTime = 0;
		clock_gettime(CLOCK_MONOTONIC,&start);
		while(diffTime<100000)
		{
			clock_gettime(CLOCK_MONOTONIC, &end);
			diffTime = ((end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec))/1000;
		}

		//Get picture
		video_grab_image(&vid, img_new);

		printf("d... ");

		clock_gettime(CLOCK_MONOTONIC, &start);
		//Send it to the DSP
		DSPProcessor_FlushMemory(visionTask.hProcessor, (PVOID)(visionTask.bufferSend),SIZEBUFF2DSP,0);
		msgToDsp.dwCmd = VISION_WRITEREADY;
		msgToDsp.dwArg1 = (DWORD)SIZEBUFF2DSP / g_dwDSPWordSize;
		status = DSPNode_PutMessage(visionTask.hNode, &msgToDsp,  DSP_FOREVER);
		if (DSP_FAILED(status)) {
			fprintf(stdout, "DSPProcessor_PutMessage failed. Status = 0x%x\n", (UINT)status);
		}

		//Read back
		status = DSPNode_GetMessage(visionTask.hNode, &msgFromDsp, DSP_FOREVER);
		if (DSP_FAILED(status)) {
			fprintf(stdout, "DSPProcessor_GetMessage failed. Status = 0x%x\n", (UINT)status);
		}
		// Go ahead and flush here
		DSPProcessor_InvalidateMemory(visionTask.hProcessor, (PVOID)(visionTask.bufferReceive),SIZEBUFF2MPU);
		
		clock_gettime(CLOCK_MONOTONIC, &end);
		diffTime = ((end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec))/1000;
		printf("%d us\n", diffTime);
		
		printf("Send picture %d\n", nb++);
		send_rtp_frame(
			vsock,            // UDP
			img_dsp->buf,msgFromDsp.dwArg1,     // JPEG
			img_dsp->w/DOWNSIZE_FACTOR, img_dsp->h/DOWNSIZE_FACTOR, // Img Size
			0,                // Format 422
			quality_factor,               // Jpeg-Quality
			dri_jpeg_header,                // DRI Header
			0              // 90kHz time increment
		);
	}

	sigaction(SIGINT, &old_action, NULL);
	video_close(&vid);
	DSPVisionClose(&visionTask);
	return EXIT_SUCCESS;
}

/** DSP Functions **/


