/*All Parameters related to scheduling MIB/SIB1,23,NPDCCH config are element in mac_sche_ctrl*/

#include "sche_pro_NB.h"
#include "dci_NB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include <list>
#include <queue>

#define LOG(...) printf(__VA_ARGS__)
#define sib1_startingRF 0 // it means SIB1 start sched at first RF.  1 means start at second RF.

uint32_t sib1_Period=256;//256 RF
bool shcedSIB1=false,schedSIB2=false,lock=false;
extern uint32_t NPDCCH_period;
uint32_t si_RepetitionPattern[4]={1,4,3};

char channel_N[L_channels][10] = {
	"NA",
	//DL
	"NPSS",
	"NSSS",
	"NPBCH",
	"SIB1_NB",
	"SIB2/3_NB",
	"NPDCCH",
	"NPDSCH",
	//UL
	"NPRACH",
	"NPUSCH"
};

//schedule NPBCH and reserve NPSS/NSSS on virtual DL channel
void NB_schedule_MIB(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channle_bitmap, bool initialSche)
{
	uint32_t scheSubframe=0,scheFrame=0;
	int i;
	scheFrame=frame;
	if(initialSche)	scheSubframe=subframes;
	else scheSubframe=subframes+1;
	if(scheSubframe==10)
	{
		scheSubframe=0;
		++scheFrame;
	}
	for (i = 0; i < NPDCCH_period; ++i)
	{
		if(scheSubframe==5)	DL_Channle_bitmap[i]=NPSS; //Reserve for NPSS, not send MAC_PDU/DCI
		else if(((scheFrame & 0x01)==0)&&(scheSubframe==9))
		{
			// LOG("(frame & 0x01):%d\n",(frame & 0x01));
			DL_Channle_bitmap[i]=NSSS;//Reserve for NSSS, not send MAC_PDU/DCI
		}
		else if(scheSubframe==0)	DL_Channle_bitmap[i]=NPBCH;//mac_rrc_data_req(get MIB content from RRC)

	// LOG("frame:%d,subframe:%d,Channel:%s\n",frame,subframes,channel_N[DL_Channle_bitmap[frame*10+subframes]]);
		// LOG("frame:%d,subframe:%d,Channel:%s\n",scheFrame,scheSubframe,channel_N[DL_Channle_bitmap[i]]);
		// system("pause");

		++scheSubframe;
		if(scheSubframe==10)
		{
			scheSubframe=0;
			++scheFrame;
		}
		if(scheFrame==1024)	scheFrame=0;
	}
}

uint32_t t_si_Period,t_si_windowStart,si_windowEnd;
void NB_schedule_SI(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channle_bitmap,MIB_NB * MIB_NB_S, SIB1_NB * SIB1_NB_S,bool initialSche)//schedule SIB1/SIB2/3
{
	/*SIB1-NB in subframe #4 of every other frame in 16 continuous frames. Period = 256RF*/
	uint32_t repetitionNum_SIB1=repetiitonSIB1(MIB_NB_S->schedulingInfoSIB1);
	uint32_t startRF_SIB1=RFstartSIB1(repetitionNum_SIB1);
	uint32_t TBS_Index=MIB_NB_S->schedulingInfoSIB1;
	uint32_t TBS_SIB1=getTBS_SIB1(TBS_Index);
	uint32_t repetitionOffset=sib1_Period/repetitionNum_SIB1;//if offset=64-->0,64,128,192,256....

	uint32_t scheSubframe=0,scheFrame=0;
	int i;
	scheFrame=frame;
	if(initialSche)	scheSubframe=subframes;
	else scheSubframe=subframes+1;
	if(scheSubframe==10)
	{
		scheSubframe=0;
		++scheFrame;
	}
	for (i = 0; i < NPDCCH_period; ++i)
	{
		//Schedule SIB1 and disable shcedule SIB1 from every 16 frames to next repetiiton.
		if(scheFrame%repetitionOffset==0)	shcedSIB1=true;
		else if(scheFrame%repetitionOffset==16)	shcedSIB1=false;
		//Bug fixed(frame & 0x01==0)-->((frame & 0x01)==0)
		//starting schedule SIB1 from first RF or second RF base on sib1_startingRF
		if (shcedSIB1&&((scheFrame & 0x01)==sib1_startingRF)&&(scheSubframe==4))
		{
			DL_Channle_bitmap[i]=SIB1;
		}
		// LOG("repetitionNum_SIB1:%d,startRF_SIB1:%d,TBS_SIB1:%d,repetitionOffset:%d\n",repetitionNum_SIB1,startRF_SIB1,TBS_SIB1,repetitionOffset);
		// system("pause");

		/*SIB2-NB,SIB3-NB use same scheudlingInfo in subframe # within si-window. Period = 64RF*/
		uint32_t si_windowStart=get_si_windowStart(SIB1_NB_S,scheFrame);//get start frame or -1
		/*Bug fixed, After free memory, the value will change even no assignment
		uint32_t t_si_windowStart,si_windowEnd,SIscheSubframe;
		uint32_t t_si_Period;*/
		uint32_t SIscheSubframe;
		// LOG("si_windowStart:%d,si_windowEnd:%d\n",si_windowStart,si_windowEnd);
		if((si_windowStart!=-1)&&(!lock))
		{
			// LOG("Start Sche SIB2-NB\n");
			lock=true;
			t_si_windowStart=si_windowStart;
			si_windowEnd=si_windowStart+(SIB1_NB_S->si_WindowLength/10)-1;
			t_si_Period=si_windowStart+(SIB1_NB_S->si_Periodicity/10)-1;
			// LOG("si_windowStart:%d,si_windowEnd:%d,next_si_Period:%d\n",t_si_windowStart,si_windowEnd,t_si_Period);
			// system("pause");
		}
		//Schedule SIB2/3 within SI-window base on si-Repetiiton
		if((scheFrame>=t_si_windowStart)&&(scheFrame<=si_windowEnd))
		{
			if(((scheFrame-SIB1_NB_S->si_RadioFrameOffset) & SIB1_NB_S->si_RepetitionPattern)==0)
			{
				schedSIB2=true;
				if(scheSubframe==0)	SIscheSubframe=get_si_scheSubframe(SIB1_NB_S->si_TB);// for SIB2/3-NB
			}
			else	schedSIB2=false;
		}

		if((DL_Channle_bitmap[i]==NA)&&(schedSIB2)&&(SIscheSubframe>0))
		{
			// LOG("si_windowStart:%d,si_windowEnd:%d,SIscheSubframe:%d\n",t_si_windowStart,si_windowEnd,SIscheSubframe);
			// system("pause");
			DL_Channle_bitmap[i]=SIB2_3;//call mac_rrc_data_req();
			--SIscheSubframe;
			if((scheFrame==si_windowEnd)&&(SIscheSubframe>0))
			{
				LOG("SIB2-NB not finish transmission within si-window\n");
				exit(1);
			}
		}
		if((scheFrame==t_si_Period)&&(scheSubframe==9))	lock=false;//Wake up scheSIB2 at the end of this si-period.
		// LOG("frame:%d,subframe:%d,Channel:%s\n",scheFrame,scheSubframe,channel_N[DL_Channle_bitmap[i]]);
		// system("pause");
		++scheSubframe;
		if(scheSubframe==10)
		{
			scheSubframe=0;
			++scheFrame;
		}
		if(scheFrame==1024)	scheFrame=0;
	}
}


void NB_schedule_ulsch(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channle_bitmap,
	uint32_t **UL_Channle_bitmap,SIB2_NB * SIB2_NB_S)
{
	uint32_t scheSubframe=0,scheFrame=0;
	int i;
	scheFrame=frame;
	scheSubframe=subframes+1;
	if(scheSubframe==10)
	{
		scheSubframe=0;
		++scheFrame;
	}
	// DCIFormatN0 *DCIFormatN0;
	// void	*ULSCH_dci = NULL;
	//Check if there's UE's UL transmissions(incldued Scheduled Info) in UE_list
	Ulsch_ind(frame,subframes);
	for (i = 0; i < NPDCCH_period; ++i)
	{

		resourceAllocation(SIB2_NB_S);






		++scheSubframe;
		if(scheSubframe==10)
		{
			scheSubframe=0;
			++scheFrame;
		}
		if(scheFrame==1024)	scheFrame=0;
	}

}

void NB_schedule_dlsch(frame_t frame,sub_frame_t subframes,uint32_t *DL_Channle_bitmap)
{
	void	*DLSCH_dci = NULL;
	HI_DCI0_request_t HI_DCI0;
	//Initial DCI
	HI_DCI0.num_DCI=0//Number of DCI PDUs included in this message
	HI_DCI0.PDU_Type=0;//0-->DCI N0 PDU
    HI_DCI0.DCI_Format;
	//Postpose
}

void NB_shceudle_RA(frame_t frame,sub_frame_t subframes,uint32_t *DL_Channle_bitmap,uint32_t **UL_Channle_bitmap)
{
	//Postpose
}
