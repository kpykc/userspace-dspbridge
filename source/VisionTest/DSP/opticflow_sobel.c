#include "image.h"

void CvtYUYV2Gray(unsigned char *grayframe, unsigned char *frame, int imW, int imH){
    unsigned short x, y;
    unsigned char *Y, *gray;
    //get only Y component for grayscale from (Y1)(U1,2)(Y2)(V1,2)
    for (y = 0; y < imH; y++) {
        Y = frame + (imW * 2 * y);
        gray = grayframe + (imW * y);
        for (x=0; x < imW; x += 2) {
            gray[x] = *Y;
            Y += 2;
            gray[x + 1] = *Y;
            Y += 2;
        }
    }
}

void ApplySobelFilter(struct img_struct* imgIn, unsigned int *profileX, unsigned int *profileY, unsigned int *threshold)
{
    int nbPointsDetected = 0;
    int nbPixelsTested = 0;
    int percentDetected;

    unsigned short x,y;
    signed short Gx,Gy;
    long Gtot;

    unsigned char *in = imgIn->imageData + imgIn->width;
    unsigned char *inPrevLine = imgIn->imageData;
    unsigned char *inNextLine = imgIn->imageData + 2*imgIn->width;

    unsigned int *profileXIni = profileX+1;

    memset(profileX,0,320*sizeof(int));
    memset(profileY,0,240*sizeof(int));

    profileX++;
    profileY++;

    for(y=1;y<imgIn->height-1;y++)
    {
        profileX = profileXIni;
        for(x=1;x<imgIn->width-1;x++)
        {
            Gx = -*(inPrevLine);
            Gy = -*(inPrevLine++);
            Gy -= 2*(*(inPrevLine++));
            Gx += *inPrevLine;
            Gy -= *(inPrevLine--);

            Gx -= 2*(*(in++));
            in++;
            Gx += 2*(*(in--));

            Gx -= *inNextLine;
            Gy += *(inNextLine++);
            Gy += 2*(*(inNextLine++));
            Gx += *inNextLine;
            Gy += *(inNextLine--);

            Gtot = Gx*Gx + Gy*Gy;

            if(Gtot>*threshold)
            {
                nbPointsDetected++;
                
            	*profileY += 1;
            	*profileX += 1;
            }
            nbPixelsTested++;
            profileX++;
        }
        in += 2;
        inPrevLine += 2;
        inNextLine += 2;
        profileY++;
    }

    percentDetected = (100*nbPointsDetected)/nbPixelsTested;
    if(percentDetected>20)
    {
    	if(*threshold<100)
    		*threshold+=10;
        else 
		    *threshold+=100;
    }
    else if(percentDetected<10)
    {
    	if(*threshold>100)
	        *threshold-=100;
	    else if(*threshold>10)
	    	*threshold -= 10;
    }
}

signed int CalcOF(unsigned int *act, unsigned int *prev, unsigned int length)
{
    short i,j;
    unsigned int diff;
    unsigned int min;
    signed int temp;
    unsigned char first = 1;
    signed short T = 255;
    unsigned int centerPoint = length/2;
    unsigned int *ptrAct,*ptrPrev;

    for(i=centerPoint-30;i<=centerPoint+30;i++)
    {
        diff = 0;
        ptrAct = act+i-60;
        ptrPrev = prev+centerPoint-60;
        for(j=-60;j<=60;j++)
        {
            temp = *(ptrAct++) - *(ptrPrev++);
            diff += temp*temp;
        }

        if(first)
        {
            min = diff;
            T = centerPoint-i;
            first = 0;
        }
        else if(diff<min)
        {
            min = diff;
            T = centerPoint-i;
        }
    }
    
	memcpy(prev, act, length*sizeof(int));
	
    return -T;
}

