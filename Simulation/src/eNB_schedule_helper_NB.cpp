#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "type_NB.h"
#include "rrc_NB.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include "sche_pro_NB.h"

using namespace std;

#define LOG(...) printf(__VA_ARGS__)

const uint32_t TBS_SIB1[16]={208,208,208,328,328,328,440,440,440,680,680,680,0,0,0};//0:Reserved
int PHR_table[3][2]={{2,3},{1,2},{0,1}};
uint32_t DV_table[16]={0,10,14,19,26,36,49,67,91,125,171,234,321,768,1500,1500};//>1500, BS assume 1500, Map to highest
uint32_t DV1_table[16]={0,5,12,17,23,31,43,58,79,108,148,203,278,545,1134,1500};//Map to medium
uint32_t DV2_table[16]={0,1,11,15,20,27,37,50,68,92,126,172,235,322,769,1500};//Map to lowest

uint32_t UL_TBS_Table[13][8]={{16,32,56,88,120,152,208,256},
{24,56,88,144,176,208,256,344},
{32,72,144,176,208,256,328,424},
{40,104,176,208,256,328,440,568},
{56,120,208,256,328,408,552,680},
{72,144,224,328,424,504,680,872},
{88,176,256,392,504,600,808,1000},
{104,224,328,472,584,712,1000,0},
{120,256,392,536,680,808,0,0},
{136,296,456,616,776,936,0,0},
{144,328,504,680,872,1000,0,0},
{176,376,584,776,1000,0,0,0},
{208,440,680,1000,0,0,0,0}};


uint32_t BSR_table[64]={0,10,12,14,17,19,22,26,31,36,42,49,57,67,78,91,
107,125,146,171,200,234,274,321,376,440,515,603,706,826,967,
1132,1326,1552,1817,2127,2490,2915,3413,3995,4677,5476,6411,
7505,8787,10287,12043,14099,16507,19325,22624,26487,31009,
36304,42502,49759,58255,68201,79846,93479,109439,128125,150000,150000};//>150000, BS assume 150000, Map to highest

uint32_t BSR1_table[64]={0,5,11,13,16,18,21,24,29,34,39,46,53,62,73,85,
99,116,136,159,186,217,254,298,349,408,478,559,655,766,897,
1050,1229,1439,1685,1972,2309,2703,3164,3704,4336,5077,5944,
6958,8146,9537,11165,13071,15303,17916,20975,24556,28748,
33657,39403,46131,54007,63228,74024,86663,101459,118782,139063,150000};//Map to medium

uint32_t BSR2_table[64]={0,1,11,13,15,18,20,23,27,32,37,43,50,58,68,79,
92,108,126,147,172,201,235,275,322,377,441,516,604,707,827,
968,1133,1327,1553,1818,2128,2491,2916,3414,3996,4678,5477,
6412,7506,8788,10288,12044,14100,16508,19326,22625,26488,
31010,36305,42503,49760,58256,68202,79847,93480,109440,128126,150000};//Map to lowest
extern uint32_t Cell_id;
extern uint32_t H_SFN;
extern uint32_t n_pp;//number of  NPDCCH_period
// extern uint32_t NPDCCH_period;
extern uint32_t NPDCCH_period[3];
extern char channel_N[L_channels][10];

extern uint8_t File_LOG_Flag;
extern uint8_t LOG_Flag;


// /*Move to sche_pro_NB C header files*/
// //SIB1_Helper_Fun
// extern "C" uint32_t repetiitonSIB1(uint32_t);
// extern "C" uint32_t RFstartSIB1(uint32_t);
// extern "C" uint32_t getTBS_SIB1(uint32_t);
// //SIB2_Helper_Fun
// extern "C" uint32_t get_si_windowStart(SIB1_NB *,frame_t);
// extern "C" uint32_t get_si_scheSubframe(uint32_t);
// extern "C" uint32_t resourceAllocation(SIB2_NB *);

double uniform_rng()
{
  double random;
  random = (double)(rand()%RAND_MAX)/((double)RAND_MAX);
  //LOG_D(OTG,"Uniform taus random number= %lf\n", random);
  return random;
}
//For CCE allocation
bool compareMyType5 (const HI_DCI0_request_t &a, const HI_DCI0_request_t &b)
{
	return a.DCI_Format.DCI_UL_PDU.startTime < b.DCI_Format.DCI_UL_PDU.startTime;
}

bool compareMyType4 (const HI_DCI0_request_t &a, const HI_DCI0_request_t &b)
{
	return a.DCI_Format.DCI_UL_PDU.endTime > b.DCI_Format.DCI_UL_PDU.endTime;
}

bool compareMyType3 (const UE_TEMPLATE_NB &a, const UE_TEMPLATE_NB &b)
{
	// return a.round > b.round;
	if(a.first_Arrival_Time!=b.first_Arrival_Time)	return a.first_Arrival_Time < b.first_Arrival_Time;
	else if(a.round!=b.round) return a.round > b.round;
	// else if (a.UL_Buffer_Size != b.UL_Buffer_Size) return a.UL_Buffer_Size < b.UL_Buffer_Size;


	// if(a.round!=b.round)	return a.round > b.round;
	// else	return a.UL_Buffer_Size < b.UL_Buffer_Size;
	// else	return a.UL_Buffer_Size > b.UL_Buffer_Size;
	// else	return a.CE_Level < b.CE_Level;
	//PHR/MCS...
}
uint32_t get_aggregation(uint32_t CE_Level,uint32_t T_SearchSpace,uint32_t R)
{
	//It could choose 1 or 2 base on channel quality, CE 0 has best channel quality
	//If channel quality is low use a higher aggregation level
	if((T_SearchSpace==1)&&(R==1)&&(CE_Level==0))	return 1;
	else if((T_SearchSpace==2)&&(R==1)&&(CE_Level==0))	return 1;
	else	return 2;
}

extern vector<int> Sfreq;
uint32_t get_nprah_resource(int End_Time,SIB2_NB & SIB2_NB_S)
{
	// FreqPosition belong to {0,4,8,12,16,20,24,28,32,36,40,44}
	int nprah_resource=0;
	for (int i = 0; i < End_Time; ++i)
	{
		for (int j = 0; j < Sfreq.size(); ++j)//{0,4,8,12,16,20,24,28,32,36,40,44}
		{
			for (int k = 0; k < 3; ++k)//Three CE level
			{
				uint32_t Trange = i%SIB2_NB_S.period[k];
				uint32_t T_E=ceil(SIB2_NB_S.start_time[k]+(1.4 * 4 * SIB2_NB_S.rep[k]));//5.6-->6, 11.2-->12, 22.4-->23
				if((SIB2_NB_S.start_time[k]<=Trange)&&(T_E>Trange))//8~14,8~20,8~31
				{
					uint32_t F_S = SIB2_NB_S.subcarrier_Offset[k];//0, 24, 36
					uint32_t F_E = SIB2_NB_S.subcarrier_Offset[k]+SIB2_NB_S.num_Subcarrier[k];//24, 36, 48
					if((Sfreq[j]>=F_S)&&(Sfreq[j]<F_E))	nprah_resource=nprah_resource+4;
				}
			}
		}
	}
	// LOG("nprah_resource:%d\n",nprah_resource);
	return nprah_resource/4;
}


uint32_t get_DCI_Filed(uint32_t Rmax,uint32_t DCI_Rep)
{
	uint32_t DCI_Filed=0;
	if(Rmax>=8)
	{
		if((Rmax/8)==DCI_Rep)	DCI_Filed=0;
		else if((Rmax/4)==DCI_Rep)	DCI_Filed=1;
		else if((Rmax/2)==DCI_Rep)	DCI_Filed=2;
		else if(Rmax==DCI_Rep)		DCI_Filed=3;
	}
	else if(Rmax==4)
	{
		if(DCI_Rep==1)	DCI_Filed=0;
		else if(DCI_Rep==2)	DCI_Filed=1;
		else if(DCI_Rep==4)	DCI_Filed=2;
	}
	else if(Rmax==2)
	{
		if(DCI_Rep==1)	DCI_Filed=0;
		else if(DCI_Rep==2)	DCI_Filed=1;
	}
	else if((Rmax==1)&&(DCI_Rep==1))	DCI_Filed=0;
	else
	{
		LOG("Abnormal Rmax/R\n");
		exit(1);
	}
	return DCI_Filed;
}

uint32_t num_ULslots(uint32_t Isc)
{
	if((Isc>=0)&&(Isc<=11))	return 16;
	else if((Isc>=12)&&(Isc<=15)) return 8;
	else if((Isc>=16)&&(Isc<=17)) return 4;
	else if(Isc==18) return 2;
	else
	{
		LOG("Abnormal subcarrier indication(Isc)\n");
		exit(1);
	}
}

uint32_t get_I_TBS(uint32_t mcs,uint32_t multi_tone)
{
	uint32_t I_TBS;
	if(multi_tone!=0) I_TBS=mcs;
	else
	{
		if(mcs==1)	I_TBS=2;
		else if(mcs==2)	I_TBS=1;
		else
		{
			I_TBS=mcs;
		}
	}
	return I_TBS;
}

// uint32_t RU_table[8]={1,2,3,4,5,6,8,10};
int get_TBS_UL(uint32_t mcs,uint32_t multi_tone,uint32_t & Iru,bool & maxIru)
{
	int TBS;
	uint32_t I_TBS=get_I_TBS(mcs,multi_tone);
	// LOG("I_TBS:%d,Iru:%d\n",I_TBS,Iru);
	if(Iru>7)
	{
		--Iru;
		maxIru=true;
	}
	TBS=UL_TBS_Table[I_TBS][Iru];
	if(TBS==0)
	{
		--Iru;
		maxIru=true;
	}
	TBS=UL_TBS_Table[I_TBS][Iru];
	// LOG("%d\n",TBS);
	return TBS>>3;
}

uint32_t check_if_NPRACH(SIB2_NB & SIB2_NB_S,uint32_t UL_ChannelTime,uint32_t FreqPosition)
{
	// SIB2_NB_S.period[i]//1280
	// SIB2_NB_S.rep[i]
	// SIB2_NB_S.start_time[i]
	// SIB2_NB_S.num_Subcarrier[i]
	// SIB2_NB_S.subcarrier_Offset[i]
	// UL_ChannelTime,9,19,129,10251....
	// FreqPosition belong to {0,4,8,12,16,20,24,28,32,36,40,44}
	// uint32_t scheH_SFN,scheFrame,scheSubframe;
	// scheH_SFN=UL_ChannelTime/10240;
	// scheFrame=(UL_ChannelTime-scheH_SFN * 10240)/10;//1
	// scheSubframe=UL_ChannelTime-scheH_SFN * 10240-scheFrame * 10;
	for (int i = 0; i < 3; ++i)//Three CE level
	{
		uint32_t Trange = UL_ChannelTime%SIB2_NB_S.period[i];
		uint32_t T_E=ceil(SIB2_NB_S.start_time[i]+(1.4 * 4 * SIB2_NB_S.rep[i]));//14
		if((SIB2_NB_S.start_time[i]<=Trange)&&(T_E>Trange))//8~14
		{
			uint32_t F_S = SIB2_NB_S.subcarrier_Offset[i];//0, 24, 36
			uint32_t F_E = SIB2_NB_S.subcarrier_Offset[i]+SIB2_NB_S.num_Subcarrier[i];//24, 36, 48
			if((FreqPosition>=F_S)&&(FreqPosition<F_E))	return 1;
		}
	}
	return 0;
}

#define sib1_startingRF 0 // it means SIB1 start sched at first RF.  1 means start at second RF.
uint32_t sib1_Period=256;//256 RF
bool shcedSIB1=false;
// bool schedSIB2=false,lock=false;
// uint32_t SIscheSubframe;
// uint32_t t_si_Period,t_si_windowStart,si_windowEnd;

uint32_t check_if_DL_subframe(uint32_t Sche_H_SFN,uint32_t scheFrame,uint32_t scheSubframe,MIB_NB & MIB_NB_S,SIB1_NB & SIB1_NB_S)
{
    // if(scheSubframe==5) DL_Channel_bitmap[i]=NPSS; //Reserve for NPSS, not send MAC_PDU/DCI
    if(scheSubframe==5) return 0; //Reserve for NPSS, not send MAC_PDU/DCI
    else if(((scheFrame & 0x01)==0)&&(scheSubframe==9))
    {
        // LOG("(frame & 0x01):%d\n",(frame & 0x01));
        // DL_Channel_bitmap[i]=NSSS;//Reserve for NSSS, not send MAC_PDU/DCI
        return 0;
    }
    else if(scheSubframe==0)    return 0;//mac_rrc_data_req(get MIB content from RRC)
    // else if(scheSubframe==0)    DL_Channel_bitmap[i]=NPBCH;//mac_rrc_data_req(get MIB content from RRC)


    /*SIB1-NB in subframe #4 of every other frame in 16 continuous frames. Period = 256RF*/
    uint32_t repetitionNum_SIB1=repetiitonSIB1(MIB_NB_S.schedulingInfoSIB1);//4
    uint32_t startRF_SIB1=RFstartSIB1(repetitionNum_SIB1);//0
    uint32_t TBS_Index=MIB_NB_S.schedulingInfoSIB1;
    uint32_t TBS_SIB1=getTBS_SIB1(TBS_Index);
    uint32_t repetitionOffset=sib1_Period/repetitionNum_SIB1;//64
	uint32_t T=Sche_H_SFN * 10240+scheFrame * 10+scheSubframe;
	uint32_t SIB1_T=T%repetitionOffset;//144 mod 64=16
    if((0<=SIB1_T)&&(SIB1_T<=16))   shcedSIB1=true;//0~15
    else	shcedSIB1=false;
    //Bug fixed(frame & 0x01==0)-->((frame & 0x01)==0)
    if (shcedSIB1&&((scheFrame & 0x01)==sib1_startingRF)&&(scheSubframe==4))
    {
        // DL_Channel_bitmap[i]=SIB1;
        return 0;
    }

    // LOG("repetitionNum_SIB1:%d,startRF_SIB1:%d,TBS_SIB1:%d,repetitionOffset:%d\n",repetitionNum_SIB1,startRF_SIB1,TBS_SIB1,repetitionOffset);
    // system("pause");

    /*SIB2-NB,SIB3-NB use same scheudlingInfo in subframe # within si-window. Period = 64RF*/
    // uint32_t si_windowStart=get_si_windowStart(SIB1_NB_S,scheFrame);//get start frame or -1
    /*Bug fixed, After free memory, the value will change even no assignment
    uint32_t t_si_windowStart,si_windowEnd,SIscheSubframe;
    uint32_t t_si_Period;*/
    //1,3,5...15,...65,67,...scheFrame
	uint32_t SIB2_T=(T%SIB1_NB_S.si_Periodicity);//131%640=131,132%640=132,...143%640=143...651%640=11....799%640=159
    if((0<=SIB2_T)&&(SIB2_T<SIB1_NB_S.si_WindowLength)&&((scheFrame & 0x01)== SIB1_NB_S.si_RadioFrameOffset))
    {
        // DL_Channel_bitmap[i]=SIB23;
        return 0;
    }
    // LOG("si_windowStart:%d,si_windowEnd:%d\n",si_windowStart,si_windowEnd);
    // if((si_windowStart!=-1)&&(!lock))
    // {
    //     // LOG("Start Sche SIB2-NB\n");
    //     lock=true;
    //     t_si_windowStart=si_windowStart;
    //     si_windowEnd=si_windowStart+(SIB1_NB_S.si_WindowLength/10)-1;
    //     t_si_Period=si_windowStart+(SIB1_NB_S.si_Periodicity/10)-1;
    //     // LOG("si_windowStart:%d,si_windowEnd:%d,next_si_Period:%d\n",t_si_windowStart,si_windowEnd,t_si_Period);
    //     // system("pause");
    // }
    //Schedule SIB2/3 within SI-window base on si-Repetiiton
    // if((scheFrame>=t_si_windowStart)&&(scheFrame<=si_windowEnd))
    // {
    //     if(((scheFrame-SIB1_NB_S.si_RadioFrameOffset) & SIB1_NB_S.si_RepetitionPattern)==0)
    //     {
    //         schedSIB2=true;
    //         if(scheSubframe==0) SIscheSubframe=get_si_scheSubframe(SIB1_NB_S.si_TB);// for SIB2/3-NB
    //     }
    //     else    schedSIB2=false;
    // }

    // if((DL_Channel_bitmap[i]==NA)&&(schedSIB2)&&(SIscheSubframe>0))
    // if((schedSIB2)&&(SIscheSubframe>0))
    // {
    //     // LOG("si_windowStart:%d,si_windowEnd:%d,SIscheSubframe:%d\n",t_si_windowStart,si_windowEnd,SIscheSubframe);
    //     // system("pause");
    //     // DL_Channel_bitmap[i]=SIB2_3;//call mac_rrc_data_req();
    //     --SIscheSubframe;
    //     if((scheFrame==si_windowEnd)&&(SIscheSubframe>0))
    //     {
    //         LOG("SIB2-NB not finish transmission within si-window\n");
    //         exit(1);
    //     }
    //     return 0;
    // }
    // if((scheFrame==t_si_Period)&&(scheSubframe==9)) lock=false;//Wake up scheSIB2 at the end of this si-period.
    // LOG("[check_if_DL_subframe]: frame:%d,subframe:%d,DL_subframe\n",scheFrame,scheSubframe);
    return T;
}

uint32_t DCI_Rep[3];
void PossibleSearchspace(SIB2_NB & SIB2_NB_S,vector<vector<uint32_t> > & locationS)
{
	for (int i = 0; i < 3; ++i)
	{
		DCI_Rep[i]=SIB2_NB_S.rep[i];
		uint32_t n=SIB2_NB_S.candidate[i];
		locationS[i].resize(n);//Adjust possible search space location base on DCI candidate
	}

	vector<uint32_t>::iterator V_it;
	uint32_t bi=0;
	for(int i=0;i<locationS.size(); i++)//locationS: 3X8 array
	{
		bi=0;
		cout<<"CE"<<i<<" possible search space location: ";
		for (int j=0;j<locationS[i].size(); j++)
		{
			locationS[i][j]=bi*DCI_Rep[i];
			cout << locationS[i][j] << " ";
			// cout<<"bi:"<<bi<<','<<"DCI_R:"<<DCI_Rep[i]<<endl;
			bi++;
		}
		cout << endl;
	}
  	// for (V_it=b0.begin(); V_it<b0.end(); V_it++)//(0,1,2,3,4,5,6,7)
  	// {
  	// 	*V_it=bi*R[0];
  	// 	cout<<' '<<*V_it;
  	// 	bi++;
  	// }
  	// cout<<endl;
  	// bi=0;
  	// for (V_it=b1.begin(); V_it<b1.end(); V_it++)//(0,2,4,6,8,10,12,14)
  	// {
  	// 	*V_it=bi*R[1];
  	// 	cout<<' '<<*V_it;
  	// 	bi++;
  	// }
  	// cout<<endl;
  	// bi=0;
  	// for (V_it=b2.begin(); V_it<b2.end(); V_it++)//(0,4,8,12,16,20,24,28)
  	// {
  	// 	*V_it=bi*R[2];
  	// 	cout<<' '<<*V_it;
  	// 	bi++;
  	// }
  	cout<<endl;
  	system("pause");
}
// uint8_t DCIs_resource_determinaiton(uint32_t scheFrame,uint32_t scheSubframe,SIB2_NB * SIB2_NB_S,list<UE_TEMPLATE_NB> & UE_Info_List,Sche_RES_t & Sche_Response,uint32_t *DL_Channel_bitmap)
uint8_t DCIs_resource_determinaiton(uint32_t scheH_SFN,uint32_t scheFrame,uint32_t scheSubframe,uint32_t pp,MIB_NB & MIB_NB_S,SIB1_NB & SIB1_NB_S,SIB2_NB & SIB2_NB_S,list<UE_TEMPLATE_NB> & UE_Info_List,list<HI_DCI0_request_t> & DCI_List)
{
	// DCI_Queue.DCI_Format.DCI_UL_PDU.Aggregation_L
	// DCI_Queue.DCI_Format.DCI_UL_PDUstartTime;
	// DCI_Queue.DCI_Format.DCI_UL_PDUendTime;
	// DCI_Queue.DCI_Format.DCI_UL_PDU.DCIN0
  	//queue<HI_DCI0_request_t> & DCI_Queue

	// uint32_t StartSearchSpace=SIB2_NB_S.npdcch_Offset_RA[0] * pp;//npdcch_Offset_RA[0]=0,sync with i=0~pp
	// uint32_t EndSearchSpace;
 //    uint32_t T_SearchSpace[3];
	// T_SearchSpace[0]=SIB2_NB_S.npdcch_NumRepetitions_RA[0];
 //    T_SearchSpace[1]=SIB2_NB_S.npdcch_NumRepetitions_RA[1];
 //    T_SearchSpace[2]=SIB2_NB_S.npdcch_NumRepetitions_RA[2];
	// uint32_t cntSearchspace=0,cnt_default=0;

	// // list<HI_DCI0_request_t> & DCI_List=Sche_Response.DCI_L;//Move to NB_schedule_ulsch()
 //  	bool DCI_set;
 //  	uint32_t index_S=0;//record index of current occupued search space....

 //    if(pp==NPDCCH_period[0])
 //    {
	// 	typename list<UE_TEMPLATE_NB>::iterator it1;
 //    	UE_Info_List.sort(compareMyType3);//sort UEs base on round
	// 	for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
	// 	{
	// 		LOG("UE_id:%d,CRC_indication:%d,round:%d,CE_Level:%d,multi_tone:%d,PHR:%d,DV:%d,UL_Buffer_Size:%d,first_Arrival_Time:%d\n",(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).CE_Level,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time);
	// 		system("pause");
	// 	}

	//   	HI_DCI0_request_t DCI_Info;
	//   	uint32_t Sche_H_SFN_CE0=scheH_SFN;
 //    	uint32_t scheFrameCE0=scheFrame;
 //    	uint32_t scheSubframeCE0=scheSubframe;
 //    	bool scheDCI;//check ith search space.

 //    	for (int i = 0; i < pp; ++i)
 //    	{
 //    		// if(DL_Channel_bitmap[i]==NA)	++cntSearchspace;
 //            // if(check_if_DL_subframe(pp,Sche_H_SFN_CE0,scheFrameCE0,scheSubframeCE0,MIB_NB_S,SIB1_NB_S)==1) ++cntSearchspace;
 //    		// else	++cnt_default;
 //    		uint32_t DCI_S=check_if_DL_subframe(pp,Sche_H_SFN_CE0,scheFrameCE0,scheSubframeCE0,MIB_NB_S,SIB1_NB_S);
 //    		if(DCI_S!=0)//Get current time of DL subframe
 //    		{
 //    			if(DCI_List.empty())	DCI_List.push_back  (DCI_Info);
 //    			else
 //    			{
	// 				typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.end();
	// 				--DCI_it1;
 //    				if((*DCI_it1).DCI_Format.DCI_UL_PDU.endTime<=DCI_S)	DCI_List.push_back  (DCI_Info);
 //    				else	continue;
 //    			}
	// 			vector<uint32_t>::iterator V_it;
	// 		  	for (V_it=b0.begin(); V_it<b0.end(); V_it++)//(0,1,2,3,4,5,6,7)
	// 		  	{
	// 		  		if((*V_it)==index_S)
	// 		  		{
	// 		  			scheDCI=true;
	// 		  		}
	// 		  	}
 //    		}
 //    		else	continue;
 //    		// LOG("Frame:%d,subframes:%d,DL_Channel_bitmap:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);

 //    		++scheSubframeCE0;
 //    		if(scheSubframeCE0==10)
 //    		{
 //    			scheSubframeCE0=0;
 //    			++scheFrameCE0;
 //    		}
 //    		if(scheFrameCE0==10240)
 //    		{
 //    			scheFrameCE0=0;
 //    			Sche_H_SFN_CE0++;
 //      		}
 //    	}
 //    cout<<"cnt_default:"<<cnt_default<<"cntSearchspace: "<<cntSearchspace<<endl;
 //    system("pause");
 //    }



	// for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
	// {
	// 	DCI_set=false;
	//   	for (uint32_t i = StartSearchSpace; i < EndSearchSpace; ++i)//0~65..
	//   	{
	//   		if((*it1).CE_Level==0)
	//   		{
	// 			if(DL_Channel_bitmap[i]==NA)
	// 			{
	// 			  	for (V_it=b0.begin(); V_it<b0.end(); V_it++)//(0,4,8,12,16,20,24,28)
	// 				{
	// 					if(index_S==(*V_it))//0=0;4=4;8=8....28=28
	// 					{
	// 						DCI_List.push_back  (DCI_Info);
 //                            typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.end();
 //                            --DCI_it1;
	// 						//Actual timing: Sche_H_SFN * 10240+scheFrame * 10+scheSubframe
	// 						(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime=Sche_H_SFN * 10240+scheFrame * 10+scheSubframe;
	// 						for (int j = i; j < NPDCCH_period; ++j)
	// 						{
	// 							if(DL_Channel_bitmap[j]==NA)
	// 							{
	// 								DL_Channel_bitmap[j]==NPDCCH;
	// 								(*DCI_it1).DCI_Format.DCI_UL_PDU.cntR++;
	// 								index_S++;
	// 							}
	// 							if((*DCI_it1).DCI_Format.DCI_UL_PDU.cntR==R[0])
	// 							{
	// 								//Actual timing: Sche_H_SFN * 10240+scheFrame * 10+scheSubframe
	// 								(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime=j;// j used by following Idelay/Isc caculaiton
	// 								DCI_set=true;
	// 								break;
	// 							}
	// 						}
	// 						break;
	// 					}
	// 				}
	// 				if(DCI_set)	continue;
	// 				index_S++;
	// 			}
	//   		}
	// 		// LOG("DL_Channel_bitmap:%s\n",channel_N[DL_Channel_bitmap[i]]);
	// 		LOG("Frame:%d,subframes:%d,DL_Channel_bitmap:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
	// 		++scheSubframe;
	// 		if(scheSubframe==10)
	// 		{
	// 			scheSubframe=0;
	// 			++scheFrame;
	// 		}
	// 		if(scheFrame==10240)
	// 		{
	// 			scheFrame=0;
	// 			Sche_H_SFN++;
	//   		}
	//   		if(DCI_set) break;
	//   	}
	//  }
}

uint32_t get_CE_level()
{
	// double PC[3]={0.6,0.2,0};//CE leve(0,1,2)-->(40%,40%,20%), ratio of devices in period_UL report.
	// double PC[3]={0.66,0.33,0};//CE leve(0,1,2)-->(33%,33%,33%), ratio of devices in period_UL report.
	// double pc=uniform_rng();
	// if(pc>=PC[0])	return 0;
	// else if((pc<PC[0])&&(pc>=PC[1]))	return 1;
	// else if((pc>=PC[2])&&(pc<PC[1]))	return 2;
	// else
	// {
	// 	LOG("Abnormal probability in get_CE_level()\n");
	// 	system("pause");
	// 	exit(1);
	// }
	uint32_t pc=rand()%3;
	if(pc==0)	return pc;
	else if(pc==1)	return pc;
	else if(pc==2)	return pc;
	else
	{
		LOG("Abnormal probability in get_CE_level()\n");
		system("pause");
		exit(1);
	}
}

uint8_t get_multi_tone_support()
{
	//# of single-tone/total # of devices for each UL periodic report.
	double SingleToneRatio[7]={0,0.1,0.33,0.5,0.66,0.9,1};
	int Alpha=2;
	double ps=uniform_rng();
	if(ps>=SingleToneRatio[Alpha])	return 1;
	else if((0<=ps)&&(ps<SingleToneRatio[Alpha]))	return 0;
	else
	{
		LOG("Abnormal probability in get_multi_tone_support()\n");
		system("pause");
		exit(1);
	}
}


double pareto_dist(double scale, double shape)
{
  double pareto_rn;

  if ((scale<=0)||(shape<=0)) {
    printf("Pareto :: scale=%.2f or shape%.2f <0 , adjust new values: sale=3, shape=4 \n", scale,shape);
    scale=3;
    shape=4;
  }

  pareto_rn=scale * pow(1/(1-uniform_rng()), 1/shape);
  // printf("Pareto Random Nb = %lf (scale=%.2f, shape=%.2f)\n", pareto_rn,scale,shape);
  return pareto_rn;
}
//Simulation Case 3
extern uint32_t simDataSize;
extern uint32_t DataSize[10];//{20,40,60,80,100,120,140,160,180,200...};
extern uint32_t indexDataSize;

extern ofstream fout_LOG;

uint32_t get_PHR_index()
{
	uint32_t pc=rand()%4;
	uint32_t PHR=0;
	switch(pc)
	{
		case 0:
			PHR=pc;
			break;
		case 1:
			PHR=pc;
			break;
		case 2:
			PHR=pc;
			break;
		case 3:
			PHR=pc;
			break;
	}
	return	PHR;
}

int get_DV_index(UE_TEMPLATE_NB & UE_Info)//UE function: payload size follow pareto distri.
{
	// double payloadSize=pareto_dist(20,2.5);//scale=20, Shape=2.5, gen_packet(traffic_model)
	uint32_t payloadSize=DataSize[indexDataSize];


	/*No MAR traffic model*/
	// if(payloadSize>200)
	// {
	// 	payloadSize=200;
	// 	// printf("pareto_dist[%d]:%lf\n", i,payloadSize);
	// }
	// else	payloadSize=floor(payloadSize+0.5);

	simDataSize=payloadSize;

	/*No MAR traffic model*/
	// if(LOG_Flag)	printf("payloadSize:%d\n",payloadSize);
	// if(File_LOG_Flag)	fout_LOG<<"payloadSize:"<<payloadSize<<endl;
	// system("pause");

	UE_Info.payloadSize=payloadSize;
	if(UE_Info.payloadSize>100)	UE_Info.I_payloadSize=1;
	else	UE_Info.I_payloadSize=0;

	for (int i = 0; i < sizeof(BSR_table)/sizeof(uint32_t)-1; ++i)
	{
		if((payloadSize<=BSR_table[i]) && (payloadSize>=0))
		{
			return	i;
		}
	}
	if(payloadSize<=0)	return 0;
	return 63;

	// for (int i = 0; i < sizeof(DV_table)/sizeof(uint32_t)-1; ++i)
	// {
	// 	if((payloadSize<=DV_table[i]) && (payloadSize>=0))	return	i;
	// }
	// return 15; // Maximum size in DV table
}

int get_BSR_index(int payloadSize)
{
	for (int i = 0; i < sizeof(BSR_table)/sizeof(uint32_t)-1; ++i)
	{
		if((payloadSize<=BSR_table[i]) && (payloadSize>=0))
		{
			return	i;
		}
	}
	if(payloadSize<=0)	return 0;
	return 63;
}

uint32_t get_CRC_indication()
{
	double pCRC=uniform_rng();
	double Ps=0.9;
	if(pCRC<=Ps)	return 0;//decode UL transmission succesfully
	else	return 1;//decode UL transmission fail
}

uint32_t Sche_res(frame_t frame,sub_frame_t subframes,Sche_RES_t & Sche_Response)
{
	uint32_t DCI_Send=H_SFN * 10240+frame * 10+subframes;
	list<HI_DCI0_request_t> & DCI_List=Sche_Response.DCI_L;
	// typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.begin();
	while(!DCI_List.empty())
	{
		// LOG("H_SFN:%d,frame:%d,subframes:%d",H_SFN,frame,subframes);
		// LOG(",Send DCI(DCI startTime/EndTime:%d/",DCI_List.front().DCI_Format.DCI_UL_PDU.startTime);
		// LOG("%d\n",DCI_List.front().DCI_Format.DCI_UL_PDU.endTime);
		// LOG("H_SFN:%d,frame:%d,subframes:%d,Send DCI(DCI startTime/EndTime:%d/%d) to PHY from DCI_List\n",H_SFN,frame,subframes,DCI_List.front().DCI_Format.DCI_UL_PDU.startTime,DCI_List.front().DCI_Format.DCI_UL_PDU.startTime.endTime);
		DCI_List.pop_front ();
	}
	// if((DCI_Send==(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime)&&(DCI_Send!=0))
	// {
		// LOG("H_SFN:%d,frame:%d,subframes:%d,Send DCI(DCI startTime/EndTime:%d/%d) to PHY from DCI_List\n",H_SFN,frame,subframes,(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime);
		// DCI_List.pop_front ();
		// system("pause");
	// }
}
//Evaluation Performance
uint32_t totalNumUE=0;
extern uint8_t TotalNumUE[10];//{12,18,24,30,36,42,48,54,60,66}
extern uint32_t TotalNumUE_H[10];//{12,24,36,48,60,72,84,96,108,120};
extern uint32_t CEi_NumUE[3];
bool simCtrl=true;
extern uint8_t runCase;//0,1,2,3,4,5,6,7,8,9
extern uint8_t highOfferedLoad;
int new_num_UE=0;
int UE_id=0;

extern uint8_t mappingBufferSize;
uint32_t Sum_Occupied_resource__U=0;

uint32_t Ulsch_ind(frame_t frame,sub_frame_t subframes,UL_IND_t & UL_Indicaiton)
{
    // list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List[0];
    // list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List[CE_Level]
    typename list<UE_TEMPLATE_NB>::iterator it1;
    // ctime=H_SFN * 10240+frame * 10+subframes; // calculate the current time
    if(((H_SFN * 10240+frame * 10+subframes)%1000)==0)//New UE's Msg3 arrive with fixed Inter aiival time
    {
    	if(highOfferedLoad==0)	new_num_UE=6;
    	else if(highOfferedLoad==1)	new_num_UE=10;

    	totalNumUE=totalNumUE+new_num_UE;
    	if(highOfferedLoad==0)
    	{
	    	if(totalNumUE>TotalNumUE[runCase])//{6,12,18,24,30,36,42,48,54,60};
	    	{
	    		// totalNumUE=TotalNumUE[runCase]-new_num_UE;
	    		totalNumUE=TotalNumUE[runCase];
	    		simCtrl=false;
	    	}
	    }
	    else if(highOfferedLoad==1)
	    {
	    	if(totalNumUE>TotalNumUE_H[runCase])//{12,24,36,48,60,72,84,96,108,120};
	    	{
	    		// totalNumUE=TotalNumUE_H[runCase]-new_num_UE;
	    		totalNumUE=TotalNumUE_H[runCase];
	    		simCtrl=false;
	    	}
	    }
    	if(LOG_Flag)
    	{
    		LOG("New UEs' Msg3 arrive at frame:%d,subframes:%d\n",frame,subframes);
 		   	LOG("new_num_UE:%d\n",new_num_UE);
 		   	LOG("totalNumUE:%d\n",totalNumUE);
    		// system("pause");
    	}
    	if(File_LOG_Flag)
    	{
			fout_LOG<<"New UEs' Msg3 arrive at "<<"H_SFN:"<<H_SFN<<"frame:"<<frame<<"subframes:"<<subframes<<endl;
			fout_LOG<<"totalNumUE:"<<totalNumUE<<endl;
		    // system("pause");
    	}
    	// int *ptr = &var;//avoid copy constructor action
    	// int &ptr = var;//avoid copy constructor action
    	// list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List;
    	// typename list<UE_TEMPLATE_NB>::iterator it1;
    	// UE_Info_List = UL_Indicaiton.UL_UE_Info_List;// Bug is copy a constructor...use int &ptr = var; to avoid copy.
    	if(simCtrl)
    	{
	    	for (int i = 0; i < new_num_UE; ++i)
	    	{
	    		UE_TEMPLATE_NB UL_UE_Info={0};
	    		UL_UE_Info.UE_id=UE_id;
	    		// UL_UE_Info.CRC_indication=get_CRC_indication();
	    		UL_UE_Info.round=1;
	    		UL_UE_Info.oldNDI_UL=0;
	    		UL_UE_Info.CE_Level=get_CE_level();
	    		// UL_UE_Info.CE_Level=0;
	    		// UL_UE_Info.ul_active=true;
	    		//Assume RA procedure succeed
	    		UL_UE_Info.CRC_indication=0;
	    		UL_UE_Info.configured=false;
	    		// UL_UE_Info.schedMsg3=false;
	    		UL_UE_Info.schedStatus=false;
	    		UL_UE_Info.sche_Msg5_Time=-1;
	    		UL_UE_Info.multi_tone=0;
	    		// UL_UE_Info.PHR=PHR_table[UL_UE_Info.CE_Level][UL_UE_Info.multi_tone];
	    		// UL_UE_Info.DV=get_DV_index();
	    		// UL_UE_Info.UL_Buffer_Size=DV_table[UL_UE_Info.DV];
	    		UL_UE_Info.PHR=-1;
	    		UL_UE_Info.DV=-1;
	    		UL_UE_Info.BSR=-1;
	    		UL_UE_Info.UL_Buffer_Size=-1;
	    		UL_UE_Info.first_Arrival_Time=H_SFN * 10240+frame * 10+subframes;
	    		UL_UE_Info.next_Arrival_Time=-1;
	    		// UL_UE_Info.next_Arrival_Time=UL_UE_Info.first_Arrival_Time;
	    		UL_UE_Info.allocate_Buffer_Size=1;
	    		UL_UE_Info.mcs=-1;
	    		UL_UE_Info.Qm=-1;
	    		// UL_UE_Info.remaining_Buffer_Sizes=-1;
	    		// UL_UE_Info.Resource=0;
	    		UL_UE_Info.startFreqPos=-1;
	    		UL_UE_Info.num_tone=-1;
	    		UL_UE_Info.num_UL_Slot=-1;
	    		UL_UE_Info.num_RU=-1;
	    		UL_UE_Info.remainging_subframe=-1;
	    		list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List[UL_UE_Info.CE_Level];
	    		CEi_NumUE[UL_UE_Info.CE_Level]++;
	    		UE_Info_List.push_back (UL_UE_Info);//Add to UE list
	    		++UE_id;
	    	}
			for (int i = 0; i < 3; ++i)
			{
				list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List[i];
				// LOG("UL_UE_Info_List[%d].size:%d\n",i,UE_Info_List.size());
		    	for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		    	{
		            if(((*it1).CRC_indication==0)&&((*it1).configured==false))//Get the time of schedule Msg5
		            {
		            	(*it1).configured=true;
		            	(*it1).sche_Msg5_Time=0;
		            	//DCI N0-->Msg4-->ACK...within a pp
		                // (*it1).sche_Msg5_Time=(*it1).first_Arrival_Time+8 * 4+8 * 4 *(*it1).CE_Level+64;
		        		// (*it1).PHR=PHR_table[(*it1).CE_Level][(*it1).multi_tone];
		        		(*it1).PHR=get_PHR_index();
		        		UE_TEMPLATE_NB *j = &*it1;
		        		/*Disable DV, First BSR done in get_DV_index()*/
						(*it1).BSR=get_DV_index(*j);
						// if(mappingBufferSize==0)	(*it1).UL_Buffer_Size=DV_table[(*it1).DV];
						// else if(mappingBufferSize==1)	(*it1).UL_Buffer_Size=DV1_table[(*it1).DV];
						// else if(mappingBufferSize==2)	(*it1).UL_Buffer_Size=DV2_table[(*it1).DV];
		    			// (*it1).BSR=get_BSR_index((*it1).payloadSize);
						if(mappingBufferSize==0)	(*it1).UL_Buffer_Size=BSR_table[(*it1).BSR];
						else if(mappingBufferSize==1)	(*it1).UL_Buffer_Size=BSR1_table[(*it1).BSR];
						else if(mappingBufferSize==2)	(*it1).UL_Buffer_Size=BSR2_table[(*it1).BSR];
						// if((*it1).UL_Buffer_Size>200)	(*it1).UL_Buffer_Size=200;
		            }
		            else if(((*it1).CRC_indication==1)&&((*it1).configured==false))//Msg3 retransmission
		            {
		                //Msg3 retransmission arrriva at next_Arrival_Time base on NB_scheudle_ulsch
		                (*it1).round++;
		            }
                	if(LOG_Flag)
    				{
		    			LOG("CE_Level:%d,UE_id:%d,CRC_indication:%d,round:%d,multi_tone:%d,PHR:%d,DV:%d,BSR:%d,UL_Buffer_Size:%d,first_Arrival_Time:%d,sche_Msg5_Time:%d,payloadSize:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).BSR,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*it1).payloadSize);
		    			// system("pause");
		    		}
		    		if(File_LOG_Flag)
		    		{
		    			fout_LOG<<"CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",CRC_indication:"<<(*it1).CRC_indication<<",round:"<<(*it1).round<<",multi_tone:"<<",PHR:"<<(*it1).PHR<<",DV:"<<(*it1).DV<<",BSR:"<<(*it1).BSR<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<",first_Arrival_Time:"<<(*it1).first_Arrival_Time<<",sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<",payloadSize:"<<(*it1).payloadSize<<endl;
		    		}
		        }
			}
		}
		// system("pause");
    }
	for (int i = 0; i < 3; ++i)//Scan by each subframe...
	{
		list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List[i];
		// LOG("UL_UE_Info_List[%d].size:%d\n",i,UE_Info_List.size());
    	for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
    	{
	      //   if((H_SFN * 10240+frame * 10+subframes)==(*it1).sche_Msg5_Time)
	      //   {
	      //       (*it1).configured=true;//Receive ACK for Msg4, Ready to schedule DCI N0 for Msg5 at next pp
       //      	if(File_LOG_Flag)
    			// {
	      //       	fout_LOG<<"[ACK for Msg4 Arrive]"<<"CE_Level:"<<(*it1).CE_Level<<" UE_id:"<<(*it1).UE_id<<" FirstMsg3ArrivalTime:"<<(*it1).first_Arrival_Time<<" ACKforMsg4Time:"<<(*it1).sche_Msg5_Time<<"configured:"<<(*it1).configured<<endl;
	      //       }
	      //   }
	    	if((H_SFN * 10240+frame * 10+subframes)==((*it1).next_Arrival_Time))//Next BSR arrival time
	    	{
	    		if((*it1).next_Arrival_Time==-1)
	    		{
	    			LOG("UL_UE_Info.next_Arrival_Time==-1, Abnormal\n");
	    			system("pause");
	    			exit(1);
	    		}
	    		// UE_TEMPLATE_NB UL_UE_Info;
	    		// list<UE_TEMPLATE_NB> % UE_Info_List = UL_Indicaiton.UL_UE_Info_List;
	            if((*it1).configured==false)
	            {
	            	LOG("In ulsch_ind(), configured==false, Abnormal\n");
	            	system("pause");
	            	exit(1);
	                (*it1).CRC_indication=get_CRC_indication();
	                // (*it1).round=1; // round++
	                // (*it1).round++;
	                // (*it1).BSR=get_BSR_index((*it1).remaining_Buffer_Sizes);
	                // (*it1).UL_Buffer_Size=BSR_table[(*it1).BSR];
	                if((*it1).CRC_indication==1)//// Msg3 retransmission
	                {
    	            	if(File_LOG_Flag)
    					{
	                		fout_LOG<<"[UE Arrival]"<<" CE_Level:"<<(*it1).CE_Level<<" UE_id:"<<(*it1).UE_id<<" FirstMsg3ArrivalTime:"<<(*it1).first_Arrival_Time<<" ACKforMsg4Time:"<<(*it1).sche_Msg5_Time<<endl;
	                	}
	                    (*it1).round++; // trigger time recalculation
	                }
	                else
	                {
	                	(*it1).round=1;
	                	// (*it1).schedMsg3=false;//disable Msg3 flag
		                (*it1).sche_Msg5_Time=(*it1).next_Arrival_Time+8 * 4+8 * 4 *(*it1).CE_Level+64;
		        		// (*it1).PHR=PHR_table[(*it1).CE_Level][(*it1).multi_tone];
		        		(*it1).PHR=get_PHR_index();
						UE_TEMPLATE_NB *j = &*it1;
						(*it1).DV=get_DV_index(*j);
						if(mappingBufferSize==0)	(*it1).UL_Buffer_Size=DV_table[(*it1).DV];
						else if(mappingBufferSize==1)	(*it1).UL_Buffer_Size=DV1_table[(*it1).DV];
						else if(mappingBufferSize==2)	(*it1).UL_Buffer_Size=DV2_table[(*it1).DV];
    	            	if(File_LOG_Flag)
    					{
							fout_LOG<<"[UE Arrival]"<<"CE_Level:"<<(*it1).CE_Level<<" UE_id:"<<(*it1).UE_id<<" FirstMsg3ArrivalTime:"<<(*it1).first_Arrival_Time<<" ACKforMsg4Time:"<<(*it1).sche_Msg5_Time<<endl;
						}
	                }
	            }
	            else//Msg5,UL Info
	            {
	    			(*it1).CRC_indication=get_CRC_indication();
	    			// (*it1).round=1;//round++;
	    			// (*it1).round++;
	    			// if((*it1).UL_Buffer_Size>200)	(*it1).UL_Buffer_Size=200;
	                if((*it1).CRC_indication==1)//Ready to rescheudle Msg5, ULInfo
	                {
	                	(*it1).BSR=get_BSR_index((*it1).payloadSize);
						if(mappingBufferSize==0)	(*it1).UL_Buffer_Size=BSR_table[(*it1).BSR];
						else if(mappingBufferSize==1)	(*it1).UL_Buffer_Size=BSR1_table[(*it1).BSR];
						else if(mappingBufferSize==2)	(*it1).UL_Buffer_Size=BSR2_table[(*it1).BSR];
	                    (*it1).round++;
	                }
	                else
	                {
	                	// Sum_Occupied_resource__U=Sum_Occupied_resource__U+(*it1).Resource;
	                	// (*it1).Resource=0;
	                	(*it1).round=1;
		    			(*it1).DV=-1;//When receive BSR, clear DV
		    			// (*it1).BSR=get_BSR_index((*it1).remaining_Buffer_Sizes);
		    			//Update UE UL buffer size base on UL grant(DCI format N0)
		    			//consider BSR subheader(2)+ subheader for MAC PDU(1-3)
		    			(*it1).payloadSize=(*it1).payloadSize-(*it1).allocate_Buffer_Size+2+2;
		    			// (*it1).allocate_Buffer_Size=0;
		    			(*it1).BSR=get_BSR_index((*it1).payloadSize);
						if(mappingBufferSize==0)	(*it1).UL_Buffer_Size=BSR_table[(*it1).BSR];
						else if(mappingBufferSize==1)	(*it1).UL_Buffer_Size=BSR1_table[(*it1).BSR];
						else if(mappingBufferSize==2)	(*it1).UL_Buffer_Size=BSR2_table[(*it1).BSR];
	                }
	            }
            	if(LOG_Flag)
    			{
	    			LOG("CE_Level:%d,UE_id:%d,CRC_indication:%d,round:%d,multi_tone:%d,PHR:%d,DV:%d,BSR:%d,UL_Buffer_Size:%d,first_Arrival_Time:%d,sche_Msg5_Time:%d,payloadSize:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).BSR,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*it1).payloadSize);
					// system("pause");
	    		}
            	if(File_LOG_Flag)
    			{
					fout_LOG<<"[UE with BSR Arrival]"<<"CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",CRC_indication:"<<(*it1).CRC_indication<<",round:"<<(*it1).round<<",ArrivalTime:"<<(*it1).next_Arrival_Time<<"UL grant(Byte):"<<(*it1).allocate_Buffer_Size<<",payloadSize:"<<(*it1).payloadSize<<",BSR:"<<(*it1).BSR<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<" FirstMsg3ArrivalTime:"<<(*it1).first_Arrival_Time<<" ACKforMsg4Time:"<<(*it1).sche_Msg5_Time<<endl;
					// system("pause");
	    		}
	        }
	    }
    }
}

bool compareMyType (const Pattern_base &a, const Pattern_base &b)
{
	return a.FreqUnit < b.FreqUnit ;
	// if(a2->round!=b2->round)	return b2->round - a2->round;
	// else	return b2->DV - a2->DV;
}

bool compareMyType1 (const Pattern_base &a, const Pattern_base &b)
{
	return a.startFreq < b.startFreq ;
	// if(a2->round!=b2->round)	return b2->round - a2->round;
	// else	return b2->DV - a2->DV;
}

uint32_t Get_pattern_base(SIB2_NB & SIB2_NB_S,vector<int> & resourceAllocPattern)
{
	int F_lowest_CE0=SIB2_NB_S.subcarrier_Offset[0];
	int F_lowest_CE1=SIB2_NB_S.subcarrier_Offset[1];
	int F_lowest_CE2=SIB2_NB_S.subcarrier_Offset[2];
	int F_highest_CE0=SIB2_NB_S.subcarrier_Offset[0]+SIB2_NB_S.num_Subcarrier[0];
	int F_highest_CE1=SIB2_NB_S.subcarrier_Offset[1]+SIB2_NB_S.num_Subcarrier[1];
	int F_highest_CE2=SIB2_NB_S.subcarrier_Offset[2]+SIB2_NB_S.num_Subcarrier[2];
	// LOG("%d,%d,%d,%d,%d,%d\n",F_lowest_CE0,F_lowest_CE1,F_lowest_CE2,F_highest_CE0,F_highest_CE1,F_highest_CE2);
	Pattern_base Pattern_base_S;
	list<Pattern_base> Pattern_base_List;
	list<Pattern_base>::iterator P_it1,P_it2;
	for (int i = 0; i < 12; ++i)	Pattern_base_List.push_back (Pattern_base_S);
	P_it1=Pattern_base_List.begin();
	(*P_it1).startFreq=F_lowest_CE1;
	(*P_it1).endFreq=F_lowest_CE0;
	(*P_it1).FreqUnit=F_lowest_CE0-F_lowest_CE1;
	// LOG("%d,%d,%d\n",(*P_it1).startFreq,(*P_it1).endFreq,(*P_it1).FreqUnit);
	P_it1++;
	(*P_it1).startFreq=F_lowest_CE2;
	(*P_it1).endFreq=F_lowest_CE0;
	(*P_it1).FreqUnit=F_lowest_CE0-F_lowest_CE2;
	P_it1++;
	(*P_it1).startFreq=F_lowest_CE0;
	(*P_it1).endFreq=F_lowest_CE1;
	(*P_it1).FreqUnit=F_lowest_CE1-F_lowest_CE0;
	P_it1++;
	(*P_it1).startFreq=F_lowest_CE2;
	(*P_it1).endFreq=F_lowest_CE1;
	(*P_it1).FreqUnit=F_lowest_CE1-F_lowest_CE2;
	P_it1++;
	(*P_it1).startFreq=F_lowest_CE0;
	(*P_it1).endFreq=F_lowest_CE2;
	(*P_it1).FreqUnit=F_lowest_CE2-F_lowest_CE0;
	P_it1++;
	(*P_it1).startFreq=F_lowest_CE1;
	(*P_it1).endFreq=F_lowest_CE2;
	(*P_it1).FreqUnit=F_lowest_CE2-F_lowest_CE1;
	P_it1++;
	(*P_it1).startFreq=F_highest_CE1;
	(*P_it1).endFreq=F_highest_CE0;
	(*P_it1).FreqUnit=F_highest_CE0-F_highest_CE1;
	P_it1++;
	(*P_it1).startFreq=F_highest_CE2;
	(*P_it1).endFreq=F_highest_CE0;
	(*P_it1).FreqUnit=F_highest_CE0-F_highest_CE2;
	P_it1++;
	(*P_it1).startFreq=F_highest_CE0;
	(*P_it1).endFreq=F_highest_CE1;
	(*P_it1).FreqUnit=F_highest_CE1-F_highest_CE0;
	P_it1++;
	(*P_it1).startFreq=F_highest_CE2;
	(*P_it1).endFreq=F_highest_CE1;
	(*P_it1).FreqUnit=F_highest_CE1-F_highest_CE2;
	P_it1++;
	(*P_it1).startFreq=F_highest_CE0;
	(*P_it1).endFreq=F_highest_CE2;
	(*P_it1).FreqUnit=F_highest_CE2-F_highest_CE0;
	P_it1++;
	(*P_it1).startFreq=F_highest_CE1;
	(*P_it1).endFreq=F_highest_CE2;
	(*P_it1).FreqUnit=F_highest_CE2-F_highest_CE1;

	for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end(); ++P_it1)
	{
		if((*P_it1).FreqUnit<0)
		{
			P_it1 = Pattern_base_List.erase (P_it1);
			--P_it1;
		}
	}
	LOG("Sort Pattern_base_List base Freq Unit:\n");
	Pattern_base_List.sort (compareMyType);
	// for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end(); ++P_it1)
	// {
	// 	LOG("startFreq:%d,endFreq:%d,FreqUnit:%d\n",(*P_it1).startFreq,(*P_it1).endFreq,(*P_it1).FreqUnit);
	// }
	//The following loop has some bug...
	P_it2=Pattern_base_List.begin();
	for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end(); ++P_it1)
	{
		if(P_it1!=Pattern_base_List.begin())
		{
			if((*P_it2).FreqUnit==(*P_it1).FreqUnit&&(*P_it2).startFreq==(*P_it1).startFreq&&(*P_it2).endFreq==(*P_it1).endFreq)
			{
				P_it1 = Pattern_base_List.erase (P_it1);
				--P_it1;
				if(P_it2!=Pattern_base_List.begin())	--P_it2;
			}
			++P_it2;
		}
	}
	for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end(); ++P_it1)
	{
		LOG("startFreq:%d,endFreq:%d,FreqUnit:%d\n",(*P_it1).startFreq,(*P_it1).endFreq,(*P_it1).FreqUnit);
	}
	// LOG("Sort Pattern_base_List base Start Freq:\n");
	Pattern_base_List.sort (compareMyType1);
	// for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end(); ++P_it1)
	// {
	// 	LOG("startFreq:%d,endFreq:%d,FreqUnit:%d\n",(*P_it1).startFreq,(*P_it1).endFreq,(*P_it1).FreqUnit);
	// }
	int TotalFreq=0;
	P_it2=Pattern_base_List.begin();
	for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end();++P_it1)
	{
		if(TotalFreq==48)
		{
			P_it1 = Pattern_base_List.erase (P_it1);
			--P_it1;
		}
		else
		{
			if(P_it1==Pattern_base_List.begin())	TotalFreq=TotalFreq+(*P_it1).FreqUnit;
			else
			{
				if((*P_it2).endFreq==(*P_it1).startFreq)
				{
					TotalFreq=TotalFreq+(*P_it1).FreqUnit;
					++P_it2;
				}
				else
				{
					P_it1 = Pattern_base_List.erase (P_it1);
					--P_it1;
				}
			}
		}
	}
	// for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end(); ++P_it1)
	// {
	// 	LOG("startFreq:%d,endFreq:%d,FreqUnit:%d\n",(*P_it1).startFreq,(*P_it1).endFreq,(*P_it1).FreqUnit);
	// }
	// system("pause");
	int cnt=0,assignedIndex=0;

	//multi_tone_option: A row form left to right must match Frequency split from lower to higher position.
	int multi_tone_option[5][3]={{1,3,6},{3,1,6},{1,1,3},{3,3,1},{1,1,1}};// could be other like {1,3}

	for (P_it1=Pattern_base_List.begin(); P_it1!=Pattern_base_List.end();++P_it1)
	{
		for (int i = assignedIndex; i < 3; ++i)
		{
			if((multi_tone_option[4][i] * 4) > (*P_it1).FreqUnit)
			{
				continue;
			}
			else
			{
				resourceAllocPattern.push_back ((*P_it1).startFreq);
				cnt=(*P_it1).startFreq+multi_tone_option[4][i] * 4;//multi_tone_option[0][i]-->{1,3,6}
				while(cnt<(*P_it1).endFreq)//{0,4,8,12,16,20,24,28,32,36,40,44}
				{
					resourceAllocPattern.push_back(cnt);
					cnt=cnt+multi_tone_option[4][i] * 4;
				}
				assignedIndex=i+1;
				break;
			}
		}
	}
	// LOG("resourceAllocPattern: ");
	// for (vector<int>::iterator it = resourceAllocPattern.begin() ; it != resourceAllocPattern.end(); ++it)
	// {
	// 	LOG("%d ",*it);
	// }
	// LOG("\n");
	// system("pause");
}



// uint32_t resourceAllocation(SIB2_NB * SIB2_NB_S,uint32_t **UL_Channel_bitmap,list<UE_TEMPLATE_NB> & UE_Info_List)
uint32_t resourceAllocation(SIB2_NB & SIB2_NB_S,list<UE_TEMPLATE_NB> & UE_Info_List)
{

	// system("pause");
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

uint32_t get_si_windowStart(SIB1_NB & SIB1_NB_S,frame_t frame)
{
	//si-window start at subframe #0 in RF=(H-SFN*10240+SFN) mod T = floor(x/10)+si_RadioFrameOffset
	//T:si-Periodicty,w is si-windowLength,n is the order of entry in the list of SI msg.
	//Assume RF # replace H-SFN,SFN because RF not recycle from 0-1023, RF #=(H-SFN*10240+SFN) if RF#<10240
	int x=0;//x=(n-1)*w, n==1 for SIB2-NB, floor(x/10)=0
	if((frame%(SIB1_NB_S.si_Periodicity/10))==(x+SIB1_NB_S.si_RadioFrameOffset))	return frame;
	else return -1;
}

uint32_t get_si_scheSubframe(uint32_t si_TB)
{
	//TBS(56 bits,120 bits) --> 2 sub-frames, other TBS --> 8 sub-frames, see TS 36.213 [Table 16.4.1.5.1-1].
	if((si_TB==56)||(si_TB==120))	return 2;
	else	return 8;
}
