#include <stdio.h>
#include <stdlib.h>
#include "type_NB.h"
#include "rrc_NB.h"
#include "sche_pro_NB.h"
#define LOG(...) printf(__VA_ARGS__)

const uint32_t TBS_SIB1[16]={208,208,208,328,328,328,440,440,440,680,680,680,0,0,0};//0:Reserved


extern uint32_t Cell_id;

uint32_t Get_pattern_base(Pattern_base * Pattern_base_S,SIB2_NB * SIB2_NB_S)
{
	int F_lowest_CE0=SIB2_NB_S->subcarrier_Offset[0];
	int F_lowest_CE1=SIB2_NB_S->subcarrier_Offset[1];
	int F_lowest_CE2=SIB2_NB_S->subcarrier_Offset[2];
	int F_highest_CE0=SIB2_NB_S->subcarrier_Offset[0]+SIB2_NB_S->num_Subcarrier[0];
	int F_highest_CE1=SIB2_NB_S->subcarrier_Offset[1]+SIB2_NB_S->num_Subcarrier[1];
	int F_highest_CE2=SIB2_NB_S->subcarrier_Offset[2]+SIB2_NB_S->num_Subcarrier[2];
	// LOG("%d,%d,%d,%d,%d,%d\n",F_lowest_CE0,F_lowest_CE1,F_lowest_CE2,F_highest_CE0,F_highest_CE1,F_highest_CE2);
	Pattern_base_S=(Pattern_base *)calloc(12,sizeof(Pattern_base));
	Pattern_base_S[0]->startFreq=F_lowest_CE1;
	Pattern_base_S[0]->endFreq=F_lowest_CE0;
	Pattern_base_S[0]->FreqUnit=F_lowest_CE0-F_lowest_CE1;
	Pattern_base_S[1]->startFreq=F_lowest_CE2;
	Pattern_base_S[1]->endFreq=F_lowest_CE0;
	Pattern_base_S[1]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[2]->startFreq=F_lowest_CE2;
	Pattern_base_S[2]->endFreq=F_lowest_CE0;
	Pattern_base_S[2]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[3]->startFreq=F_lowest_CE2;
	Pattern_base_S[3]->endFreq=F_lowest_CE0;
	Pattern_base_S[3]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[4]->startFreq=F_lowest_CE2;
	Pattern_base_S[4]->endFreq=F_lowest_CE0;
	Pattern_base_S[4]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[5]->startFreq=F_lowest_CE2;
	Pattern_base_S[5]->endFreq=F_lowest_CE0;
	Pattern_base_S[5]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[6]->startFreq=F_lowest_CE2;
	Pattern_base_S[6]->endFreq=F_lowest_CE0;
	Pattern_base_S[6]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[7]->startFreq=F_lowest_CE2;
	Pattern_base_S[7]->endFreq=F_lowest_CE0;
	Pattern_base_S[7]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[8]->startFreq=F_lowest_CE2;
	Pattern_base_S[8]->endFreq=F_lowest_CE0;
	Pattern_base_S[8]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[9]->startFreq=F_lowest_CE2;
	Pattern_base_S[9]->endFreq=F_lowest_CE0;
	Pattern_base_S[9]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[10]->startFreq=F_lowest_CE2;
	Pattern_base_S[10]->endFreq=F_lowest_CE0;
	Pattern_base_S[10]->FreqUnit=F_lowest_CE0-F_lowest_CE2;
	Pattern_base_S[11]->startFreq=F_lowest_CE2;
	Pattern_base_S[11]->endFreq=F_lowest_CE0;
	Pattern_base_S[11]->FreqUnit=F_lowest_CE0-F_lowest_CE2;



	system("pause");
	free(Pattern_base_S);
}

uint32_t resourceAllocation(SIB2_NB * SIB2_NB_S)
{
	Pattern_base Pattern_base_S={0};
	Get_pattern_base(&Pattern_base_S,SIB2_NB_S);
}



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
