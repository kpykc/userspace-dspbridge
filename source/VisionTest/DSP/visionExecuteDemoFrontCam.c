/*
 *  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  ======== dmmcopyExecute.c =========
 *  Sample DSP program to copy one dynamically mapped MPU buffer to another.
 *  This program accompanies the wdmmcopy.c DSP/BIOS Bridge sample program.
 *
 */

#include <std.h>
#include <tsk.h>
#include <rms.h>
#include <node.h>
/* BCACHE only exists for Joule/GEM */
#include <bcache.h>

#include "image.h"
#include "jpeg.h"

#define TRACE &trace

#define DMMMEMSEG          	    	0
#define VISION_SETUPBUFFERS     	0xABCD
#define VISION_SETUPTEMPBUFFERS     0xBCDE
#define VISION_SETUPIMAGESIZE		0xABCE
#define VISION_WRITEREADY       	0xADDD

void resize(unsigned char*, unsigned char*);

/*
 *  ======== DMMCOPY_TI_execute ========
 */
RMS_STATUS VISION_TI_execute(NODE_EnvPtr env)
{
    RMS_DSPMSG      msg;
    unsigned int  	size,imgInSize,imgOutSize;
    unsigned char  	*end;
    unsigned char 	*tempAddrOutA,*tempAddrOutB;
    unsigned char 	*tempAddrInA, *tempAddrInB;
    unsigned int temp1,temp2;
	unsigned int i,j;
	unsigned int downsizefactor;
	unsigned int quality_factor = 50; // From 0 to 99 (99=high)
	unsigned int dri_jpeg_header = 0;

	//unsigned char quality_factor = 50; // From 0 to 99 (99=high)
	//unsigned char dri_jpeg_header = 0;
	//unsigned char* end;

	struct img_struct imgIn;
	struct img_struct imgOut;
	struct img_struct imgTemp;

    /* Initialize msg structure. */
    msg.cmd = ~(RMS_EXIT);

    for (;;) {

        /* Get message. */
        NODE_getMsg(env, &msg, NODE_FOREVER);

        if (msg.cmd == VISION_WRITEREADY) 
		{
            /* How many MAUs to copy? */
            size  = (unsigned int)(msg.arg1);			
            BCACHE_invalidate((Ptr)(imgIn.buf),size);
			
			//Netherlands flag
			/*for(j=0;j<imgOut.h;j++)
			{
				for(i=0;i<imgOut.w;i+=2)	//Par 2 pixels
				{
					if(j<imgOut.h/3)
					{
						imgOut.buf[2*(i+j*imgOut.w)]=84;	//U
						imgOut.buf[2*(i+j*imgOut.w)+1]=76;	//Y
						imgOut.buf[2*(i+j*imgOut.w)+2]=255;	//V
						imgOut.buf[2*(i+j*imgOut.w)+3]=76;	//Y
					}
					else if(j<2*imgOut.h/3)
					{
						imgOut.buf[2*(i+j*imgOut.w)]=128;
						imgOut.buf[2*(i+j*imgOut.w)+1]=255;
						imgOut.buf[2*(i+j*imgOut.w)+2]=128;
						imgOut.buf[2*(i+j*imgOut.w)+3]=255;
					}
					else
					{
						imgOut.buf[2*(i+j*imgOut.w)]=255;
						imgOut.buf[2*(i+j*imgOut.w)+1]=29;
						imgOut.buf[2*(i+j*imgOut.w)+2]=107;
						imgOut.buf[2*(i+j*imgOut.w)+3]=29;
					}
				}
			}*/
			
			//French flag
			/*for(i=0;i<imgOut.w;i+=2)
			{
				for(j=0;j<imgOut.h;j++)	//Par 2 pixels
				{
					if(i<imgOut.w/3)
					{
						imgOut.buf[2*(i+j*imgOut.w)]=255;
						imgOut.buf[2*(i+j*imgOut.w)+1]=29;
						imgOut.buf[2*(i+j*imgOut.w)+2]=107;
						imgOut.buf[2*(i+j*imgOut.w)+3]=29;
					}
					else if(i<2*imgOut.w/3)
					{
						imgOut.buf[2*(i+j*imgOut.w)]=128;
						imgOut.buf[2*(i+j*imgOut.w)+1]=255;
						imgOut.buf[2*(i+j*imgOut.w)+2]=128;
						imgOut.buf[2*(i+j*imgOut.w)+3]=255;
					}
					else
					{
						imgOut.buf[2*(i+j*imgOut.w)]=84;
						imgOut.buf[2*(i+j*imgOut.w)+1]=76;
						imgOut.buf[2*(i+j*imgOut.w)+2]=255;
						imgOut.buf[2*(i+j*imgOut.w)+3]=76;
					}
				}
			}*/
			
			/*tempAddrOutA = imgOut.buf;
			for(j=0;j<(imgIn.h/4);j++)
			{
				for(i=0;i<(imgIn.w/4);i+=2)
				{
					*(tempAddrOutA++)=imgIn.buf[8*(i+j*imgIn.w)];
					*(tempAddrOutA++)=imgIn.buf[8*(i+j*imgIn.w)+1];
					*(tempAddrOutA++)=imgIn.buf[8*(i+j*imgIn.w)+2];
					*(tempAddrOutA++)=imgIn.buf[8*(i+j*imgIn.w)+3];
				}
			}*/
			
			tempAddrInA = imgIn.buf;	
			tempAddrInB = imgIn.buf+2*(imgIn.h/2)*imgIn.w;
			tempAddrOutA = imgTemp.buf;
			tempAddrOutB = imgTemp.buf+2*90*320;
			temp1 = 3*2*imgIn.w;	//To skip 3 lines
			
			for(j=0;j<90;j++)
			{
				for(i=0;i<320;i+=2)
				{	
					(*(unsigned int*)(tempAddrOutA)) = (unsigned int)(*(unsigned int*)(tempAddrInA));
					tempAddrOutA += 4; tempAddrInA+=16;
					
					
					(*(unsigned int*)(tempAddrOutB)) = (unsigned int)(*(unsigned int*)(tempAddrInB));
					tempAddrOutB += 4; tempAddrInB+=16;
				}
				tempAddrInA += temp1;
				tempAddrInB += temp1;
			}

			end = encode_image (imgTemp.buf, imgOut.buf, quality_factor, FOUR_TWO_TWO, (imgIn.w/4), (imgIn.h/4), dri_jpeg_header);
			size = end-(imgOut.buf);

			msg.arg1 = size;

            BCACHE_writebackInvalidate((Ptr)(imgOut.buf), size);

            /* Tell GPP that we're done copying */
            NODE_putMsg(env, NODE_TOGPP, &msg, 0);

        } else if (msg.cmd == RMS_EXIT) {
            /* Exit if GPP says we should */
            break;
        } else if (msg.cmd == VISION_SETUPBUFFERS) {
            /* Setup buffer addresses */
			imgIn.buf = (unsigned char *)(msg.arg1);
			imgOut.buf = (unsigned char *)(msg.arg2);
		}else if(msg.cmd == VISION_SETUPTEMPBUFFERS){
 			imgTemp.buf = (unsigned char *)(msg.arg1);
		}else if(msg.cmd == VISION_SETUPIMAGESIZE){
			imgIn.w = *((unsigned int *)(imgIn.buf));
			imgIn.h = *((unsigned int *)(imgIn.buf+4));
			imgOut.w = *((unsigned int *)(imgIn.buf+8));
			imgOut.h = *((unsigned int *)(imgIn.buf+12));
			downsizefactor = (imgIn.w/imgOut.w);
			imgInSize = (2*imgIn.w*imgIn.h);
			imgOutSize = (2*imgOut.w*imgOut.h);
		}
    }

    return (RMS_EOK);
}
