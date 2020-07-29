#include "pul_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int pul_detect(long int *data)
{
	int i,n;
	int d_value;
	n = 0;

	#ifdef JUMP_FL
	for(i=(BEGIN_WINDOW);i<END_WINDOW-1;i++)
	{
		//DETECT DATA LOSS 
		if(data[i]==-1)
			return -1;
	}	
	#else
	for(i=(BEGIN_WINDOW-1);i<END_WINDOW;i++)
	{
		//DETECT DATA LOSS 
		if(data[i]==-1)
			return -1;
	}
	#endif

	
	for(i=BEGIN_WINDOW;i<END_WINDOW;i++)
	{		
		#ifdef JUMP_FL
		//if the first number is -1
		if((i==BEGIN_WINDOW)&&(data[BEGIN_WINDOW-1]==-1))
			continue;
		#endif
		
		d_value = data[i]-data[i-1];

		if(d_value>0)
		{
			if(d_value > UP_THRESHOLD_VALUE)
			{
				//stage_up return 31,pulse_up return pul_length*2
				while(d_value > UPE_THRESHOLD_VALUE)
				{
					n++;
					d_value = data[i+n]-data[i+n-1];

					#ifdef JUMP_FL
					//if the last number is -1
					if(data[i+n]==-1)
						return 31;
					#endif
					
					if(n+i==END_WINDOW)
						return 31;
				}
				return 2*n;
			}
		}
		else
		{
			if(d_value < DOWN_THRESHOLD_VALUE)
			{
				//stage_down return 30,pulse_down return pul_length*2+1
				while(d_value < DOWNE_THRESHOLD_VALUE)
				{
					n++;
					d_value = data[i+n]-data[i+n-1];

					#ifdef JUMP_FL
					//if the last number is -1
					if(data[i+n]==-1)
						return 31;
					#endif
					
					if(n+i==END_WINDOW)
						return 30;
				}
				return 2*n+1;
			}
		}
	}
	return 0;
}


int pul_judg(int pul_num)
{
	int u_or_d;
	//int pul_l;

	if(pul_num == 31)
		return 2;
	else if (pul_num == 30)
		return 3;
	else if(pul_num == -1)
		return -1;
	else if(pul_num == 0)
		return 4;
	
	//exclude 31,30,-1,detect pul_length
	if(pul_num) 
	{
		if(pul_num%2)
		{
			u_or_d = 0;
			//pul_l = pul_num/2;
		}
		else
		{
			u_or_d = 1;
			//pul_l = pul_num/2;
		}
	}
	return u_or_d;
}


int pul_out_m(int pulse,char *msg, FILE *fpo)
{
	
	if(pulse==1)
   		fprintf(fpo, "%s%s\n", LTOOL_STR_PULS_UP,msg);
	else if(pulse==0)
		fprintf(fpo, "%s%s\n", LTOOL_STR_PULS_DOWN,msg);
	else if(pulse==2)
		fprintf(fpo,"%s%s\n", LTOOL_STR_STAGE_UP,msg);
	else if(pulse==3)
		fprintf(fpo,"%s%s\n", LTOOL_STR_STAGE_DOWN,msg);
	else if(pulse==4)
		fprintf(fpo, "%s%s\n", LTOOL_STR_STILL,msg);
	else 
		fprintf(fpo, "%s%s\n", LTOOL_STR_LOSS,msg);
	return 1;
}

int pul_out_d(int pulse,int d_pulse,char *msg, FILE *fpo)
{
	if(pulse == d_pulse)
	{
		if(d_pulse==1)
			fprintf(fpo, "%s%s\n", LTOOL_STR_PULS_UP,msg);
		else if(d_pulse==0)
			fprintf(fpo, "%s%s\n", LTOOL_STR_PULS_DOWN,msg);
		else if(d_pulse==2)
			fprintf(fpo, "%s%s\n", LTOOL_STR_STAGE_UP,msg);
		else if(d_pulse==3)
			fprintf(fpo, "%s%s\n", LTOOL_STR_STAGE_DOWN,msg);
		else if(d_pulse==4)
			fprintf(fpo, "%s%s\n", LTOOL_STR_STILL,msg);
		else
			fprintf(fpo, "%s%s\n", LTOOL_STR_LOSS,msg);
	}
	else
	{
		if(d_pulse==1)
			fprintf(fpo, "%s%s  %s\n", LTOOL_STR_PULS_UP,msg,LTOOL_STR_ERROR);
		else if(d_pulse==0)
			fprintf(fpo, "%s%s  %s\n", LTOOL_STR_PULS_DOWN,msg,LTOOL_STR_ERROR);
		else if(d_pulse==2)
			fprintf(fpo, "%s%s  %s\n", LTOOL_STR_STAGE_UP,msg,LTOOL_STR_ERROR);
		else if(d_pulse==3)
			fprintf(fpo, "%s%s  %s\n", LTOOL_STR_STAGE_DOWN,msg,LTOOL_STR_ERROR);
		else if(d_pulse==4)
			fprintf(fpo, "%s%s  %s\n", LTOOL_STR_STILL,msg,LTOOL_STR_ERROR);
		else
			fprintf(fpo, "%s%s\n", LTOOL_STR_LOSS,msg);
	}
	return 1;
}



