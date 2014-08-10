
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Computer Vision
#include "opticflow/optic_flow_gdc.h"
#include "opticflow/trig.h"
#include "opticflow/fastRosten.h"

// Own Header
#include "opticflow_code.h"

//DSP include
extern int segid;
#include <std.h>
#include <tsk.h>
#include <rms.h>
#include <node.h>
#include <bcache.h>

// Communication
//#include "video_message_structs.h"
//struct gst2ppz_message_struct gst2ppz;
//struct ppz2gst_message_struct ppz2gst;

// Downlink
//#include "messages.h"
//#include "subsystems/datalink/downlink.h"

//#include "paparazzi.h"

// Settable by pluging
unsigned short imgWidth, imgHeight;
unsigned int verbose = 0;

// Local variables
//static unsigned char * img_uncertainty;
unsigned char *prev_frame, *gray_frame;

int old_img_init;
int old_pitch, old_roll, old_alt;
int opt_trans_x;
int opt_trans_y;
int opt_angle_x_raw;
int opt_angle_y_raw;

int x_buf[24]={0};
int y_buf[24]={0};
int diff_roll_buf[24]={0};
int diff_pitch_buf[24]={0};

int opt_trans_x_buf[32]={0};
int opt_trans_y_buf[32]={0};

unsigned int buf_point = 0;
unsigned int buf_imu_point;
unsigned int buf_opt_trans_point;

short *x, *y, *new_x, *new_y, *status, *dx, *dy, *dx_scaled, *dy_scaled;
int *n_inlier_minu, *n_inlier_minv, *active;
float divergence;
float FPS = 60;
int error_corner, error_opticflow, mark_points;

// Corner Detection
short count = 0;
short max_count = 25;
int flow_point_size = 0;
#define MAX_COUNT 150	// Maximum number of flow points
flowPoint *flow_points;//[150];
detectedPoint *detected_points;//[150];

#define ALWAYS_NEW_POINTS	0

extern int segid;

// Called by plugin
void my_plugin_init(void)
{
	// Init variables
	//ppz2gst.pitch = 0;
	//ppz2gst.roll = 0;
	gray_frame = (unsigned char *)MEM_calloc(segid,imgWidth*imgHeight*sizeof(unsigned char),0);
	prev_frame = (unsigned char *)MEM_calloc(segid,imgWidth*imgHeight*2*sizeof(unsigned char),0);
	old_img_init = 1;

	old_pitch = 0;
	old_roll = 0;
	old_alt = 0;
	opt_trans_x = 0;
	opt_trans_y = 0;

	opt_angle_x_raw = 0;
	opt_angle_y_raw = 0;

	//gst2ppz.counter = 0;

	mark_points = 0;

	x = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	new_x = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	y = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	new_y = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	status = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	dx = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	dy = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	dx_scaled = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	dy_scaled = (short *) MEM_calloc(segid,MAX_COUNT*sizeof(short),0);
	n_inlier_minu = (int *) MEM_calloc(segid,sizeof(int),0);
	n_inlier_minv = (int *) MEM_calloc(segid,sizeof(int),0);
	
	flow_points = (flowPoint*)MEM_calloc(segid,MAX_COUNT*sizeof(flowPoint),0);
	detected_points = (detectedPoint*)MEM_calloc(segid,MAX_COUNT*sizeof(detectedPoint),0);
}

void my_plugin_stop(void)
{
	free(x);
	free(new_x);
	free(y);
	free(new_y);
	free(status);
	free(dx);
	free(dy);
	free(dx_scaled);
	free(dy_scaled);
	free(n_inlier_minu);
	free(n_inlier_minv);
}

void my_plugin_run(unsigned char *frame)
{

	short threshold_n_points = 25;
	
	int tot_x=0;
	int tot_y=0;
	int x_avg = 0;
	int y_avg = 0;

	//magical scaling needed in order to calibrate opt flow angles to imu angles
	short scalex = 1024; //1024*(1/0.75)
	short scaley = 1024; //1024*(1/0.76)
		
	int i;

	if(old_img_init == 1)
	{
		memcpy(prev_frame,frame,imgHeight*imgWidth*2);
		old_img_init = 0;
	}

	// ***********************************************************************************************************************
	// (1) possibly find new points - keeping possible old ones (normal cv methods / efficient point finding / active corners)
	// ***********************************************************************************************************************

    if(ALWAYS_NEW_POINTS)
    {
    	// Clear corners
    	memset(flow_points,0,sizeof(flowPoint)*flow_point_size);
    	findPoints(gray_frame, frame, imgWidth, imgHeight, &count, max_count, MAX_COUNT, flow_points, &flow_point_size, detected_points);
    }
    else
    {
    	if(flow_point_size < threshold_n_points)
    	{
    		findPoints(gray_frame, frame, imgWidth, imgHeight, &count, max_count, MAX_COUNT, flow_points, &flow_point_size, detected_points);
    	}
    }

	// **********************************************************************************************************************
	// (2) track the points to the new image, possibly using external information (TTC, known lateral / rotational movements)
	// **********************************************************************************************************************
    if(count)
    {
		trackPoints(frame, prev_frame, imgWidth, imgHeight, &count, max_count, MAX_COUNT, flow_points, &flow_point_size, detected_points, x, y, new_x, new_y, dx, dy, status);
		//showFlow(frame, x, y, status, count, new_x, new_y, imgWidth, imgHeight);

		for (i=0; i<count;i++)
		{
//			dx[i] = (new_x[i]-x[i]);
//			dy[i] = (new_y[i]-y[i]);
			dx[i] = flow_points[i].dx;
			dy[i] = flow_points[i].dy;

			tot_x = tot_x + dx[i];
			tot_y = tot_y + dy[i];
		}
		// using moving average to filter out the noise
		if(count)
		{
			x_buf[buf_point] = (tot_x*scalex)/count;
			y_buf[buf_point] = (tot_y*scaley)/count;
			buf_point = (buf_point+1) %5;
		}

		for (i=0;i<5;i++) {
			x_avg+=x_buf[i];
			y_avg+=y_buf[i];
		}

		//raw optic flow (for telemetry purpose)
		opt_angle_x_raw = x_avg;
		opt_angle_y_raw = y_avg;

		//tele purpose
//int diff_roll, diff_pitch, mean_alt;
//float mean_tti, median_tti, d_heading, d_pitch, pu[3], pv[3], divergence_error;

		/*int USE_FITTING = 0;

		if(USE_FITTING == 1)
		{
			analyseTTI(&divergence, x, y, dx, dy, n_inlier_minu, n_inlier_minv, count, imgWidth, imgHeight);
		}*/

		memcpy(prev_frame,frame,imgHeight*imgWidth*2);
		showFlow(frame, x, y, status, count, new_x, new_y, imgWidth, imgHeight);

		//DOWNLINK_SEND_OPTIC_FLOW(DefaultChannel, DefaultDevice, &FPS, &opt_angle_x_raw, &opt_angle_y_raw, &opt_trans_x, &opt_trans_y, &diff_roll, &diff_pitch, &mean_alt, &count, &count, &divergence, &mean_tti, &median_tti, &d_heading, &d_pitch, &pu[2], &pv[2], &divergence_error, n_inlier_minu, n_inlier_minv);
    }

	//DOWNLINK_SEND_OF_ERROR(DefaultChannel, DefaultDevice, &error_corner, &error_opticflow);

	// Send to paparazzi
	//gst2ppz.ID = 0x0001;
	//gst2ppz.counter++; // to keep track of data through ppz communication

}


