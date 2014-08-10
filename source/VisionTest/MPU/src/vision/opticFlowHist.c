#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define RESOLUTION	10
#define LONG_MAX	2147483647

char TrackZone_2(unsigned int *act, unsigned int *prev, unsigned int start, unsigned int end, signed int *depl)
{
	//Déplacement max:+/-10 pixels
	int i,j;
	signed int E=0,sigma=0;
	long min = LONG_MAX;
	int depl_temp;
	long erreur[2*RESOLUTION+1];
	for(i=-RESOLUTION;i<=RESOLUTION;i++)
	{
		erreur[i+RESOLUTION] = 0;
		for(j=start;j<=end;j++)
		{
			erreur[i+RESOLUTION] += abs(act[j+i] - prev[j]);
		}
	}

	//Valeurs statistiques des erreurs
	for(i=0;i<2*RESOLUTION+1;i++)
	{
		E += erreur[i];
	}
	E/=2*RESOLUTION+1;

	for(i=0;i<21;i++)
	{
		sigma += (erreur[i]-E)*(erreur[i]-E);
	}
	sigma /= 2*RESOLUTION+1;
	sigma = sqrt(sigma);

	for(i=0;i<2*RESOLUTION+1;i++)
	{
		if(erreur[i]<min)
		{
			min = erreur[i];
			depl_temp = i-10;
		}
	}
	if(min>(E-sigma))
	{
		return -1;
	}
	*depl = depl_temp;
	return 0;
}

int CalcOF3_2(unsigned int *act, unsigned int *prev, unsigned int length)
{
	int T=0,T_temp;
	//Calcul de la moyenne de la norme des gradients
	int i,j;
	float Gm=0;
	unsigned int start,end,zone=0;
	signed int nbZones = 0;
	float borne;
	
	for(i=1;i<length;i++)
	{
		Gm += abs(prev[i]-prev[i-1]);
	}
	Gm /= (length-1);

	//Recherche des zones
	//Pour la sauvegarde des valeurs, chaque zone à une taille supérieur ou égale à 10
	int *buff = (int*)(malloc((length/RESOLUTION)*sizeof(int)));
	borne = 3*Gm/4;
	for(i=2*RESOLUTION;i<length-2*RESOLUTION-1;i++)
	{
		if(abs(prev[i]-prev[i-1])>borne && zone==0)
		{
			start = i;
			zone = 1;
		}
		else if(zone)
		{
			if((abs(prev[i]-prev[i-1])<borne && i-start>=10) || i-start>20 )
			{
				end = i;
				zone = 0;
				if(TrackZone_2(act,prev,start,end,&T_temp)!=-1)
				{
					buff[nbZones++] = T_temp;
				}
			}
		}
	}
	memcpy(prev,act,length*sizeof(unsigned int));
	if(nbZones == 0){ free(buff); return -255;}

	//Filtrage médian pour avoir le résultat
	for(i=0;i<nbZones;i++)
	{
		for(j=i;j<nbZones-1;j++)
		{
			if(buff[i]>buff[j+1])
			{
				buff[i] ^= buff[j+1];
				buff[j+1] ^= buff[i];
				buff[i] ^= buff[j+1];
			}
		}
	}

	T =  buff[nbZones/2];
	free(buff);
	return T;
}


