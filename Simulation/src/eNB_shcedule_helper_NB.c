#include <stdio.h>
#include <stdlib.h>
#include "type_NB.h"
#include "rrc_NB.h"


const uint32_t TBS_SIB1[16]={208,208,208,328,328,328,440,440,440,680,680,680,0,0,0};//0:Reserved


extern uint32_t Cell_id;

uint32_t repetiitonSIB1(uint32_t schedulingInfoSIB1)
{
	uint32_t repetition[3]={4,8,16};
	int i;
	for (i = 0; i < 11; ++i)
	{
		if(schedulingInfoSIB1==i)	return repetition[i%3];
	}
}

uint32_t RFstartSIB1(uint32_t repetitionNum_SIB1)
{
	if(repetitionNum_SIB1==4)
	{
		if(Cell_id%4==0)	return 0;
		else if(Cell_id%4==1)	return 12;
		else if(Cell_id%4==2)	return 32;
		else if(Cell_id%4==3)	return 48;
	}
	else if(repetitionNum_SIB1==8)
	{
		if(Cell_id%2==0)	return 0;
		else if(Cell_id%2==1)	return 16;
	}
	else if(repetitionNum_SIB1==16)
	{
		if(Cell_id%2==0)	return 0;
		else if(Cell_id%2==1)	return 1;
	}
	else
	{
		LOG("Abnormal repetitionNum_SIB1!\n");
		exit(1);
	}
}
uint32_t getTBS_SIB1(uint32_t TBS_Index)
{
	return TBS_SIB1[TBS_Index];
}

uint32_t get_si_windowStart(SIB1_NB * SIB1_NB_S,frame_t frame)
{
	//si-window start at subframe #0 in RF=(H-SFN*1024+SFN) mod T = floor(x/10)+si_RadioFrameOffset
	//T:si-Periodicty,w is si-windowLength,n is the order of entry in the list of SI msg.
	//Assume RF # replace H-SFN,SFN because RF not recycle from 0-1023, RF #=(H-SFN*1024+SFN) if RF#<1024
	int x=0;//x=(n-1)*w, n==1 for SIB2-NB, floor(x/10)=0
	if((frame%(SIB1_NB_S->si_Periodicity/10))==(x+SIB1_NB_S->si_RadioFrameOffset))	return frame;
	else return -1;
}

uint32_t get_si_scheSubframe(uint32_t si_TB)
{
	//TBS(56 bits,120 bits) --> 2 sub-frames, other TBS --> 8 sub-frames, see TS 36.213 [Table 16.4.1.5.1-1].
	if((si_TB==56)||(si_TB==120))	return 2;
	else	return 8;
}
