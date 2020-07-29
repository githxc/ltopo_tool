#include "pul_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int pul_detect(long int *data)
{
	int i,n;
	int d_value;
	n = 0;

	for(i=1;i<7;i++)
	{
		if(data[i]==-1)
			return -1;
	}

	
	for(i=1;i<8;i++)
	{
		if((i==1)&&(data[0]==-1))
			continue;
		d_value = data[i]-data[i-1];
		
		if(d_value>0)
		{
			if(d_value > UP_THRESHOLD_VALUE)
			{
				while(d_value > UPE_THRESHOLD_VALUE)
				{
					n++;
					d_value = data[i+n]-data[i+n-1];
					if((n+i==8)||(data[i+n]==-1))
						return 31;
				}
				return 2*n;
			}
		}
		else
		{
			if(d_value < DOWN_THRESHOLD_VALUE)
			{
				while(d_value < DOWNE_THRESHOLD_VALUE)
				{
					n++;
					d_value = data[i+n]-data[i+n-1];
					if((n+i==8)||(data[i+n]==-1))
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
	int pul_l;

	
	if(pul_num == 31)
		return 2;
	else if (pul_num == 30)
		return 3;
	else if(pul_num == -1)
		return -1;

	if(pul_num) //如果传回来的值不是30或31或-1
	{
		if(pul_num%2)
		{
			u_or_d = 0;
			pul_l = pul_num/2;
		}
		else
		{
			u_or_d = 1;
			pul_l = pul_num/2;
		}
	}
	else if(pul_num == 0)
	{
		pul_l = 0;
		u_or_d = 4;
	}

	if(pul_l <= S_PUL_LENGTH)
	{
		return u_or_d;
	}
	else
	{
		if(u_or_d)
			return 5;
		else
			return 6;
	}
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
	else if(pulse==5)
		fprintf(fpo,"%s%s\n",LTOOL_STR_LPULS_UP,msg);
	else if(pulse==6)
		fprintf(fpo,"%s%s\n",LTOOL_STR_LPULS_DOWN,msg);
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
			else if(d_pulse==5)
				fprintf(fpo,"%s%s\n",LTOOL_STR_LPULS_UP,msg);
			else if(d_pulse==6)
				fprintf(fpo,"%s%s\n",LTOOL_STR_LPULS_DOWN,msg);
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
			else if(d_pulse==5)
				fprintf(fpo,"%s%s  %s\n",LTOOL_STR_LPULS_UP,msg,LTOOL_STR_ERROR);
			else if(d_pulse==6)
				fprintf(fpo,"%s%s  %s\n",LTOOL_STR_LPULS_DOWN,msg,LTOOL_STR_ERROR);
			else
				fprintf(fpo, "%s%s\n", LTOOL_STR_LOSS,msg);
		}
	return 1;
}



