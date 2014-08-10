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
#include "opticflow_sobel.h"

#define TRACE &trace

#define VISION_SETUPBUFFERS     	0xABCD
#define VISION_SETUPTEMPBUFFER      0xBCDE
#define VISION_SETUPIMAGESIZE		0xABCE
#define VISION_SETUPMALLOCBUFFER    0xBCDF
#define VISION_WRITEREADY       	0xADDD

int segid;
unsigned int *histTemp;
unsigned int nbSkipped = 0;

extern unsigned int calcFlowXYZ(int *Tx_min, int *Ty_min, int *Tz_min, unsigned int *histX, unsigned int *prevHistX, unsigned int *histY, unsigned int *prevHistY, unsigned int *skip);

//Histogram Z test
#define FACT	110

unsigned int *histTempX,*histTempY;

/*
 *  ======== DMMCOPY_TI_execute ========
 */
RMS_STATUS VISION_TI_execute(NODE_EnvPtr env)
{
    RMS_DSPMSG      msg;
    
    char *mallocBase;
    
    unsigned int size,imgInSize,imgOutSize;

	struct img_struct imgIn;
	struct img_struct imgOut;
	struct img_struct imgTemp;
	
	unsigned int *profileX, *prevProfileX;
	unsigned int *profileY, *prevProfileY;
	unsigned int threshold = 1000;
	int test,testGros;
	
	unsigned char *hist,*histCum;
	
	int Tx,Ty,Tz;
	
	unsigned int erreur;
	
    /* Initialize msg structure. */
    msg.cmd = ~(RMS_EXIT);

    for (;;) {

        /* Get message. */
        NODE_getMsg(env, &msg, NODE_FOREVER);

        if (msg.cmd == VISION_WRITEREADY) 
		{
            /* How many MAUs to copy? */
            size  = (unsigned int)(msg.arg1);			
            BCACHE_invalidate((Ptr)(imgIn.imageData),size);
            
            //EgaliserHist(&imgIn, hist, histCum);
            test = ApplySobelFilter2(&imgIn, profileX, profileY, &threshold);
            
            /*To generate histogram with known Tx Ty to check the on-board implementation*/
            /*memset(profileX,0,320*sizeof(unsigned int));
            memset(prevProfileX,0,320*sizeof(unsigned int));
            memset(profileY,0,240*sizeof(unsigned int));
            memset(prevProfileY,0,240*sizeof(unsigned int));
            for(test=0;test<320;test++)
            {
            	if(test<160)
            	{
            		profileX[test] = test;
	            	prevProfileX[test] = test;
	            }
	            else
	            {
	            	profileX[test] = 320 - test;
	            	prevProfileX[test] = 320 - test;
	            }
            }
            
            for(test=0;test<240;test++)
            {
            	if(test<120)
            	{
            		profileY[test] = test;
	            	prevProfileY[test] = test;
	            }
	            else
	            {
	            	profileY[test] = 240 - test;
	            	prevProfileY[test] = 240 - test;
	            }
            }/**/
            
            //Tz histogram generation
            /*for(test=-160;test<160;test++)
            {
            	testGros = test*(FACT+1)+2*FACT;
            	testGros /= FACT;
            	testGros += 160;
            	if(testGros>=0 && testGros<320)
            	{
            		profileX[test+160] = prevProfileX[testGros];
            	}
            	else
            	{
            		profileX[test+160] = 0;
            	}
            }
            for(test=-120;test<120;test++)
            {
            	testGros = test*(FACT+1)-FACT;
            	testGros /= FACT;
            	testGros += 120;
            	if(testGros>=0 && testGros<240)
            	{
            		profileY[test+120] = prevProfileY[testGros];
            	}
            	else
            	{
            		profileY[test+120] = 0;
            	}
            }/**/
            

			//Tx = CalcOF3_2(profileX,prevProfileX,320);
			//Ty = CalcOF3_2(profileY,prevProfileY,240);
			erreur = calcFlowXYZ(&Tx,&Ty,&Tz,profileX,prevProfileX,profileY,prevProfileY,&nbSkipped);

			msg.arg1 = size;
			msg.arg2 = (RMS_WORD)erreur;
			*((int*)(imgOut.imageData)) = Tx;
			*((int*)(imgOut.imageData+sizeof(int))) = Ty;
			*((int*)(imgOut.imageData+2*sizeof(int))) = Tz;

            BCACHE_writebackInvalidate((Ptr)(imgOut.imageData), size);

            /* Tell GPP that we're done copying */
            NODE_putMsg(env, NODE_TOGPP, &msg, 0);

        } else if (msg.cmd == RMS_EXIT) {
            /* Exit if GPP says we should */
            break;
        } else if (msg.cmd == VISION_SETUPBUFFERS) {
            /* Setup buffer addresses */
			imgIn.imageData = (unsigned char *)(msg.arg1);
			imgOut.imageData = (unsigned char *)(msg.arg2);
		}else if(msg.cmd == VISION_SETUPTEMPBUFFER){
 			imgTemp.imageData = (unsigned char *)(msg.arg1);
		}else if(msg.cmd == VISION_SETUPIMAGESIZE){
			imgIn.width = *((unsigned int *)(imgIn.imageData));
			imgIn.height = *((unsigned int *)(imgIn.imageData+4));
			imgOut.width = *((unsigned int *)(imgIn.imageData+8));
			imgOut.height = *((unsigned int *)(imgIn.imageData+12));
			imgTemp.width = imgIn.width;
			imgTemp.height = imgIn.height;
			imgInSize = (2*imgIn.width*imgIn.height);
			imgOutSize = (2*imgOut.width*imgOut.height);
			imgIn.widthStep = 2*imgIn.width;	//YUYV input image
			imgTemp.widthStep = imgTemp.width;
		}else if(msg.cmd == VISION_SETUPMALLOCBUFFER){
			mallocBase = (char*)msg.arg1;
			
			profileX = (unsigned int*)(imgOut.imageData);
			prevProfileX = (unsigned int*)(imgOut.imageData+320*sizeof(unsigned int));
			profileY = (unsigned int*)(imgOut.imageData+2*320*sizeof(unsigned int));
			prevProfileY = (unsigned int*)(imgOut.imageData+(240+2*320)*sizeof(unsigned int));
			
			hist = (unsigned char*)(mallocBase += 256*sizeof(unsigned char));
			histCum = (unsigned char*)(mallocBase += 256*sizeof(unsigned char));
			histTemp = (unsigned int*)(mallocBase += 256*sizeof(unsigned int));
			
			histTempX = (unsigned int*)(mallocBase += 320*sizeof(unsigned int));
			histTempY = (unsigned int*)(mallocBase += 240*sizeof(unsigned int));
			
			/*profileX = (unsigned int*)(mallocBase);
			prevProfileX = (unsigned int*)(mallocBase+320*sizeof(unsigned int));
			profileY = (unsigned int*)(mallocBase+2*320*sizeof(unsigned int));
			prevProfileY = (unsigned int*)(mallocBase+(240+2*320)*sizeof(unsigned int));*/
			
			memset(profileX,0,320*sizeof(int));
			memset(prevProfileX,0,320*sizeof(int));
   			memset(profileY,0,240*sizeof(int));
   			memset(prevProfileY,0,240*sizeof(int));
		}
    }

    return (RMS_EOK);
}

