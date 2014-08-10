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
#include "resize.h"

#define TRACE &trace

#define DMMMEMSEG          	    	0
#define VISION_SETUPBUFFERS     	0xABCD
#define VISION_SETUPTEMPBUFFERS     0xBCDE
#define VISION_SETUPIMAGESIZE		0xABCE
#define VISION_WRITEREADY       	0xADDD

/*
 *  ======== DMMCOPY_TI_execute ========
 */
RMS_STATUS VISION_TI_execute(NODE_EnvPtr env)
{
    RMS_DSPMSG      msg;
    unsigned int   size, tempSize, sizeProut;
    unsigned char*  tempAddrIn;
    unsigned char*  tempAddrOut;
	unsigned int x,y;
	unsigned int downsizefactor;

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
			
			/*
			* Note: We can't use memcpy since pointers for buffers of
			* size > 64K will wrap around. 
			*/

            BCACHE_invalidate((Ptr)(imgIn.buf),size);

//Copy Input to Output through tempBuffer: 2 ways to read image pixels
			tempAddrOut = imgTemp.buf;
            tempAddrIn = imgIn.buf;
			tempSize = (2*imgIn.w*imgIn.h)-1;	//?
            while (tempSize > 0 )
            {
				*tempAddrOut = *tempAddrIn;
                tempAddrOut++;
                tempAddrIn++;		
                tempSize--;
            }

			tempAddrOut = imgOut.buf;
            tempAddrIn = imgTemp.buf;

			for(y=0;y<imgOut.h;y++)
			{
				for(x=0;x<imgOut.w;x++)
				{
					//2 bytes per pixel: UY(px 1,3,5,...) or VY(px 2,4,6,...)
					*tempAddrOut = *tempAddrIn;
		            tempAddrOut++;
		            tempAddrIn++;
					*tempAddrOut = *tempAddrIn;
		            tempAddrOut++;
		            tempAddrIn++;
				}
			}

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
		}
    }

    return (RMS_EOK);
}
