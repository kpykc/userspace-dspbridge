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


#define VIDEO_IN_W	320
#define VIDEO_IN_H	240

#define DOWNSIZE_FACTOR   1

#define VIDEO_OUT_W	VIDEO_IN_W
#define VIDEO_OUT_H	VIDEO_IN_H

#define SIZEBUFF2DSP	VIDEO_IN_W*VIDEO_IN_H*2
#define SIZEBUFF2MPU	VIDEO_OUT_W*VIDEO_OUT_H*2

int CalcOF3_2(unsigned int *act, unsigned int *prev, unsigned int length);

int millisleep = 250;
int visionRun = 1;

//=== returns the number of usecs of (t2 - t1)
// FPS
volatile float FPS;
volatile long timestamp;
#define USEC_PER_SEC 1000000L
volatile long time_elapsed (struct timeval *t1, struct timeval *t2) {
	long sec, usec;
	sec = t2->tv_sec - t1->tv_sec;
	usec = t2->tv_usec - t1->tv_usec;
	if (usec < 0) {
	--sec;
	usec = usec + USEC_PER_SEC;
	}
	return sec*USEC_PER_SEC + usec;
}

struct timeval start_time;
struct timeval end_time;

volatile void start_timer() {
gettimeofday (&start_time, NULL);
}
volatile long end_timer() {
gettimeofday (&end_time, NULL);
return time_elapsed(&start_time, &end_time);
}

struct timeval start_time2;
struct timeval end_time2;

void start_timer2() {
gettimeofday (&start_time2, NULL);
}
volatile long end_timer2() {
gettimeofday (&end_time2, NULL);
return time_elapsed(&start_time2, &end_time2);
}

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

int cnt = 0;

//Handler called when DSP send an image back
void ImgFromDSP(void)
{
	printf("DSP HANDLER!\n");
}

int main(int argc, char **argv)
{	
	int flags = fcntl(0, F_GETFL, 0); /* get current file status flags */
	flags |= O_NONBLOCK; /* turn off blocking flag */
	fcntl(0, F_SETFL, flags);

	int status;
	struct VISION_TASK visionTask;
	
	unsigned long nb = 0;

	/* Messaging used for GPP/DSP synchronization */
	struct DSP_MSG msgToDsp;
	struct DSP_MSG msgFromDsp;
	struct DSP_NOTIFICATION notifyFromDSP;
	notifyFromDSP.handle = &ImgFromDSP;

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
	vid.device = (char*)"/dev/video2";
	vid.w=VIDEO_IN_W;
	vid.h=VIDEO_IN_H;
	vid.n_buffers = 4;
	if (video_init(&vid)<0) 
	{
		printf("Error initialising video\n");
		return EXIT_FAILURE;
	}

	// Network Transmit
	struct UdpSocket* vsock;
	vsock = udp_socket("192.168.1.255", 5000, 5001, FMS_BROADCAST);
	
	struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = &sigint_handler;
    sigaction(SIGINT, &action, &old_action);

	if(DSP_FAILED(DSPNode_RegisterNotify(visionTask.hNode, DSP_NODEMESSAGEREADY, DSP_SIGNALEVENT, &notifyFromDSP)))
	{
		printf("Register Notify Failed\n");
	}

	printf("Vision started...\n");

	int Tx,Ty;
	unsigned int cntLog = 0;
	FILE *file = fopen("/data/video/dsp/log", "w");

	while(visionRun)
	{
		start_timer();

		//Get picture
		video_grab_image(&vid, img_new);

		start_timer2();
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
		/*
		unsigned int *profileX = (unsigned int*)(visionTask.bufferReceive);
		unsigned int *prevProfileX = (unsigned int*)(visionTask.bufferReceive+320*sizeof(unsigned int));
		unsigned int *profileY = (unsigned int*)(visionTask.bufferReceive+2*320*sizeof(unsigned int));
		unsigned int *prevProfileY = (unsigned int*)(visionTask.bufferReceive+(240+2*320)*sizeof(unsigned int));

		Tx = CalcOF3_2(profileX, prevProfileX, 320);
		Ty = CalcOF3_2(profileY, prevProfileY, 240);*/
		
		timestamp = end_timer2();
		
		if(cntLog>300 && cntLog<600)
		{
			
			unsigned char *base = visionTask.bufferSend+2*120*320*sizeof(unsigned char);
			int i;
			for(i=0;i<320;i++)
			{
				fwrite(base,1,1,file);
				base += 2;
			}
			if(cntLog++==600)
				printf("Fin LOG\n");
		}
		else if (cntLog<=300)
			cntLog++;
		
		if(cnt++==5)
		{
			printf("X_DSP:%d Y_DSP:%d, Z_DSP:%d th:%d\n",*((int*)(visionTask.bufferReceive)), *((int*)(visionTask.bufferReceive+sizeof(int))), *((int*)(visionTask.bufferReceive+2*sizeof(int))), msgFromDsp.dwArg2);
			//printf("X_ARM:%d Y_ARM:%d, th:%d\n", Tx, Ty, msgFromDsp.dwArg2);
			printf("LOG: %d\n", cntLog);
			printf("dt = %d, FPS = %f\n\n",timestamp, FPS);
			cnt = 0;
			
		}
		
		timestamp = end_timer();
		FPS = (float) 1000000/(float)timestamp;
	}
	printf("\n");
	sigaction(SIGINT, &old_action, NULL);
	fclose(file);
	video_close(&vid);
	DSPVisionClose(&visionTask);
	return EXIT_SUCCESS;
}

