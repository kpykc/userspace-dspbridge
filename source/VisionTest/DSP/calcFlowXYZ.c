//unsigned int histTempX[320];
extern unsigned int *histTempX;
//unsigned int histTempY[240];
extern unsigned int *histTempY;

int prevTx = 0;
int prevTy = 0;
int prevTz = 0;

unsigned int errorHists(unsigned int *hist1, unsigned int *hist2, unsigned int start, unsigned int end)
{
	int j;
	unsigned int erreur = 0;
	for(j=start;j<end;j++)
	{
		erreur += (hist1[j]-hist2[j])*(hist1[j]-hist2[j]);
	}
	erreur *= 512;
	return erreur/(end-start);
}

unsigned int calcFlowXYZ(int *Tx_min, int *Ty_min, int *Tz_min, unsigned int *histX, unsigned int *prevHistX, unsigned int *histY, unsigned int *prevHistY, unsigned int *skip)
{
    int Tx=0,Ty=0,Tz=0;
    int Tz_inf,Tz_sup;
    int TyPrev=-255;
    short start,end;
    unsigned int erreurX,erreurY;
    unsigned int erreur,min;
    short first=1;
    int idx,idxGros;
    short i;
    short X,Y;
	#define FACT	100
	
	Tz_inf = prevTz-10;
	if(Tz_inf<-30)
	{
		Tz_inf = -30;
	}
	Tz_sup = prevTz+10;
	if(Tz_sup>30)
	{
		Tz_sup = 30;
	}
	
    /*for(Tz=-10;Tz<=10;Tz++)
    {
    	for(Ty=prevTy-5*FACT;Ty<=prevTy+5*FACT;Ty+=FACT)
    	{
    		for(Tx=prevTx-5*FACT;Tx<=prevTx+5*FACT;Tx+=FACT)
    		{
				start = -1;
				memset(histTempX,0,320*sizeof(unsigned int));
				for(i=-160;i<160;i++)
				{
					idxGros = i*(FACT-Tz)-Tx;
					idx = idxGros/FACT + 160;
					if(idx>=0 && idx<320)
					{
						if(start==-1)
							start = idx;
						end = idx;
						histTempX[160+i] = prevHistX[idx];
					}
				}
				erreurX = errorHists(histTempX,histX,start,end+1);

    			if(TyPrev != Ty)
    			{
					start = -1;
					memset(histTempY,0,240*sizeof(unsigned int));
					for(i=-120;i<120;i++)
					{
						idxGros = i*(FACT-Tz)-Ty;
						idx = idxGros/FACT + 120;
						if(idx>=0 && idx<240)
						{
							if(start==-1)
								start = idx;
							end = idx;
							histTempY[120+i] = prevHistY[idx];
						}
					}
					erreurY = errorHists(histTempY,histY,start,end+1);
					TyPrev = Ty;
    			}

				erreur = erreurX + erreurY;

				if(first)
				{
					min = erreur;
					*Tx_min = Tx;
					*Ty_min = Ty;
					*Tz_min = Tz;
					first = 0;
				}
				else if(erreur<min)
				{
					min = erreur;
					*Tx_min = Tx;
					*Ty_min = Ty;
					*Tz_min = Tz;
				}
    		}
    	}
    }
    
    prevTx = *Tx_min;
    prevTy = *Ty_min;
    prevTz = *Tz_min;
    
	memcpy(prevHistX,histX,320*sizeof(unsigned int));
	memcpy(prevHistY,histY,240*sizeof(unsigned int));*/
	
    //for(Tz=-10;Tz<=10;Tz++)
    for(Tz=-5;Tz<=5;Tz++)
    {
    	for(Ty=prevTy-300;Ty<=prevTy+300;Ty+=100)
    	{
    		for(Tx=prevTx-300;Tx<=prevTx+300;Tx+=100)
    		{
				start = -1;
				memset(histTempX,0,320*sizeof(unsigned int));
				for(i=-160;i<160;i++)
				{
					idxGros = i*(100+Tz)-Tx;
					idx = idxGros/100 + 160;
					if(idx>=0 && idx<320)
					{
						if(start==-1)
							start = idx;
						end = idx;
						histTempX[160+i] = prevHistX[idx];
					}
				}
				erreurX = errorHists(histTempX,histX,start,end+1);

    			if(TyPrev != Ty)
    			{
					start = -1;
					memset(histTempY,0,240*sizeof(unsigned int));
					for(i=-120;i<120;i++)
					{
						idxGros = i*(100+Tz)-Ty;
						idx = idxGros/100 + 120;
						if(idx>=0 && idx<240)
						{
							if(start==-1)
								start = idx;
							end = idx;
							histTempY[120+i] = prevHistY[idx];
						}
					}
					erreurY = errorHists(histTempY,histY,start,end+1);
					TyPrev = Ty;
    			}

				erreur = erreurX + erreurY;

				if(first)
				{
					min = erreur;
					*Tx_min = Tx;
					*Ty_min = Ty;
					*Tz_min = Tz;
					first = 0;
				}
				else if(erreur<min)
				{
					min = erreur;
					*Tx_min = Tx;
					*Ty_min = Ty;
					*Tz_min = Tz;
				}
    		}
    	}
    }
    
    if(*Tz_min==0 && (*skip)++<9)
    {
    	*Tx_min = prevTx;
    	*Ty_min = prevTy;
    	*Tz_min = prevTz;
    }
    else
    {
		prevTx = *Tx_min;
		prevTy = *Ty_min;
		prevTz = *Tz_min;
		*skip=0;
		memcpy(prevHistX,histX,320*sizeof(unsigned int));
		memcpy(prevHistY,histY,240*sizeof(unsigned int));
	}
    
	//memcpy(prevHistX,histX,320*sizeof(unsigned int));
	//memcpy(prevHistY,histY,240*sizeof(unsigned int));
	
	return min;
}

