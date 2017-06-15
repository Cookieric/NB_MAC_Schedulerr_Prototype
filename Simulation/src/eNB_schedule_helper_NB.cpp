#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "type_NB.h"
#include "rrc_NB.h"
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include "sche_pro_NB.h"

using namespace std;

#define LOG(...) printf(__VA_ARGS__)

const uint32_t TBS_SIB1[16]={208,208,208,328,328,328,440,440,440,680,680,680,0,0,0};//0:Reserved
uint32_t PHR_table[3][2]={{2,3},{1,2},{0,1}};
uint32_t DV_table[15]={0,10,14,19,26,36,49,67,91,125,171,234,321,768,1500};//>1500, BS assume 1500

uint32_t BSR_table[63]={0,10,12,14,17,19,22,26,31,36,42,49,57,67,78,91,
107,125,146,171,200,234,274,321,376,440,515,603,706,826,967,
1132,1326,1552,1817,2127,2490,2915,3413,3995,4677,5476,6411,
7505,8787,10287,12043,14099,16507,19325,22624,26487,31009,
36304,42502,49759,58255,68201,79846,93479,109439,128125,150000};//>150000, BS assume 150000

extern uint32_t Cell_id;
extern uint32_t H_SFN;
extern uint32_t n_pp;//number of  NPDCCH_period
extern uint32_t NPDCCH_period;
extern char channel_N[L_channels][10];

typedef struct _Pattern_base {
	int startFreq;
	int endFreq;
	int FreqUnit;
	int assignedPrio;
}Pattern_base;


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

bool compareMyType3 (const UE_TEMPLATE_NB &a, const UE_TEMPLATE_NB &b)
{
	if(a.round!=b.round)	return a.round > b.round;
	else	return a.CE_Level < b.CE_Level;
}

uint8_t DCIs_resource_determinaiton(uint32_t scheFrame,uint32_t scheSubframe,SIB2_NB * SIB2_NB_S,list<UE_TEMPLATE_NB> & UE_Info_List,Sche_RES_t & Sche_Response,uint32_t *DL_Channel_bitmap)
{

  	uint32_t Sche_H_SFN=H_SFN;

	uint32_t StartSearchSpace=SIB2_NB_S->npdcch_Offset_RA[0] * NPDCCH_period;//sync with i=0~pp
	uint32_t EndSearchSpace;
	uint32_t T_SearchSpace=StartSearchSpace+SIB2_NB_S->npdcch_NumRepetitions_RA[0];
	uint32_t cntSearchspace=0,cnt_default=0;
	for (int i = 0; i < NPDCCH_period; ++i)
	{
		if(cntSearchspace==T_SearchSpace)
		{
			EndSearchSpace=StartSearchSpace+i;
			break;
		}
		if(DL_Channel_bitmap[i]==NA)	++cntSearchspace;
		else	++cnt_default;
		// LOG("DL_Channel_bitmap:%s\n",channel_N[DL_Channel_bitmap[i]]);
		// LOG("Frame:%d,subframes:%d,DL_Channel_bitmap:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
		// ++scheSubframe;
		// if(scheSubframe==10)
		// {
		// 	scheSubframe=0;
		// 	++scheFrame;
		// }
		// if(scheFrame==1024)
		// {
		// 	scheFrame=0;
		// 	Sche_H_SFN++;
  // 		}
	}
	cout<<"cnt_default:"<<cnt_default<<"EndSearchSpace: "<<EndSearchSpace<<endl;
	system("pause");
	typename list<UE_TEMPLATE_NB>::iterator it1;

	UE_Info_List.sort(compareMyType3);//sort UEs base on round/CE_level
	for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
	{
		LOG("UE_id:%d,CRC_indication:%d,round:%d,CE_Level:%d,multi_tone:%d,PHR:%d,DV:%d,UL_Buffer_Size:%d,first_Arrival_Time:%d\n",(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).CE_Level,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time);
		system("pause");
	}
	uint32_t R[3]={SIB2_NB_S->rep[0],SIB2_NB_S->rep[1],SIB2_NB_S->rep[2]};
	uint32_t n0=SIB2_NB_S->npdcch_NumRepetitions_RA[0]/R[0];
	uint32_t n1=SIB2_NB_S->npdcch_NumRepetitions_RA[1]/R[1];
	uint32_t n2=SIB2_NB_S->npdcch_NumRepetitions_RA[2]/R[2];
	vector <uint32_t> b0(n0,0),b1(n1,0),b2(n2,0);
	vector<uint32_t>::iterator V_it;
	uint32_t bi=0;
  	for (V_it=b0.begin(); V_it<b0.end(); V_it++)//(0,4,8,12,16,20,24,28)
  	{
  		*V_it=bi*R[0];
  		cout<<' '<<*V_it;
  		bi++;
  	}
  	cout<<endl;
  	bi=0;
  	for (V_it=b1.begin(); V_it<b1.end(); V_it++)//(0,8,16,24)
  	{
  		*V_it=bi*R[1];
  		cout<<' '<<*V_it;
  		bi++;
  	}
  	cout<<endl;
  	bi=0;
  	for (V_it=b2.begin(); V_it<b2.end(); V_it++)//(0,16)
  	{
  		*V_it=bi*R[2];
  		cout<<' '<<*V_it;
  		bi++;
  	}
  	cout<<endl;
  	system("pause");
	// DCI_Queue.DCI_Format.DCI_UL_PDU.Aggregation_L
	// DCI_Queue.DCI_Format.DCI_UL_PDUstartTime;
	// DCI_Queue.DCI_Format.DCI_UL_PDUendTime;
	// DCI_Queue.DCI_Format.DCI_UL_PDU.DCIN0
  	//queue<HI_DCI0_request_t> & DCI_Queue
	list<HI_DCI0_request_t> & DCI_List=Sche_Response.DCI_L;
  	HI_DCI0_request_t DCI_Info;
  	bool DCI_set;
  	uint32_t index_S=0;//record index of current occupued search space....
	for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
	{
		DCI_set=false;
	  	for (uint32_t i = StartSearchSpace; i < EndSearchSpace; ++i)//0~65..
	  	{
	  		if((*it1).CE_Level==0)
	  		{
				if(DL_Channel_bitmap[i]==NA)
				{
				  	for (V_it=b0.begin(); V_it<b0.end(); V_it++)//(0,4,8,12,16,20,24,28)
					{
						if(index_S==(*V_it))//0=0;4=4;8=8....28=28
						{
							DCI_List.push_back  (DCI_Info);
                            typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.end();
                            --DCI_it1;
							//Actual timing: Sche_H_SFN * 10240+scheFrame * 10+scheSubframe
							(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime=Sche_H_SFN * 10240+scheFrame * 10+scheSubframe;
							for (int j = i; j < NPDCCH_period; ++j)
							{
								if(DL_Channel_bitmap[j]==NA)
								{
									DL_Channel_bitmap[j]==NPDCCH;
									(*DCI_it1).DCI_Format.DCI_UL_PDU.cntR++;
									index_S++;
								}
								if((*DCI_it1).DCI_Format.DCI_UL_PDU.cntR==R[0])
								{
									//Actual timing: Sche_H_SFN * 10240+scheFrame * 10+scheSubframe
									(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime=j;// j used by following Idelay/Isc caculaiton
									DCI_set=true;
									break;
								}
							}
							break;
						}
					}
					if(DCI_set)	continue;
					index_S++;
				}
	  		}
	  		else if((*it1).CE_Level==1)
	  		{

	  		}
	  		else//((*it1).CE_Level==2)
	  		{

	  		}
			// LOG("DL_Channel_bitmap:%s\n",channel_N[DL_Channel_bitmap[i]]);
			LOG("Frame:%d,subframes:%d,DL_Channel_bitmap:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
			++scheSubframe;
			if(scheSubframe==10)
			{
				scheSubframe=0;
				++scheFrame;
			}
			if(scheFrame==1024)
			{
				scheFrame=0;
				Sche_H_SFN++;
	  		}
	  		if(DCI_set) break;
	  	}
	 }
}

uint8_t get_CE_level()
{
	double PC[3]={0.5,0.2,0};//CE leve(0,1,2)-->(50%,30%,20%), ratio of devices in period_UL report.
	double pc=uniform_rng();
	if(pc>PC[0])	return 0;
	else if((pc<PC[0])&&(pc>PC[1]))	return 1;
	else if((pc>=PC[2])&&(pc<PC[1]))	return 2;
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

uint8_t get_DV_index()//UE function: payload size follow pareto distri.
{
	double payloadSize=pareto_dist(20,2.5);//scale=20, Shape=2.5
	if(payloadSize>200)
	{
		payloadSize=200;
		// printf("pareto_dist[%d]:%lf\n", i,payloadSize);
	}
	else
		payloadSize=floor(payloadSize+0.5);
	// printf("payloadSize:%.2lf\n",payloadSize);
	for (int i = 0; i < sizeof(DV_table)/sizeof(uint32_t); ++i)
	{
		if((payloadSize<=DV_table[i]) && (payloadSize>=0))	return	i;
	}
	return 14;
}

uint8_t get_BSR_index(uint32_t remaining_Buffer_Sizes)
{
	for (int i = 0; i < sizeof(BSR_table)/sizeof(uint32_t); ++i)
	{
		if((remaining_Buffer_Sizes<=BSR_table[i]) && (remaining_Buffer_Sizes>=0))	return	i;
		else
		{
			LOG("Abnormal remaining_Buffer_Sizes in get_BSR_index()\n");
			system("pause");
			exit(1);
		}
	}
	return 63;
}

uint8_t get_CRC_indication()
{
	double pCRC=uniform_rng();
	double Ps=0.9;
	if(pCRC<=Ps)	return 0;//decode UL transmission succesfully
	else	return 1;//decode UL transmission fail
}

uint32_t Ulsch_ind(frame_t frame,sub_frame_t subframes,UL_IND_t & UL_Indicaiton)
{
	list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List;
	typename list<UE_TEMPLATE_NB>::iterator it1;
	if(((H_SFN * 10240+frame * 10+subframes)%1000)==0)//New UE's Msg3 arrive with fixed Inter aiival time
	{
		LOG("New UEs' Msg3 arrive at frame:%d,subframes:%d\n",frame,subframes);
		static int UE_id=10;
		int new_num_UE=7;
		UE_TEMPLATE_NB UL_UE_Info;
		// int *ptr = &var;//avoid copy constructor action
		// int &ptr = var;//avoid copy constructor action
		// list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List;
		// typename list<UE_TEMPLATE_NB>::iterator it1;
		// UE_Info_List = UL_Indicaiton.UL_UE_Info_List;// Bug is copy a constructor...use int &ptr = var; to avoid copy.
		for (int i = 0; i < new_num_UE; ++i)
		{
			UE_Info_List.push_back (UL_UE_Info);
		}
		for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		{
			(*it1).UE_id=UE_id;
			++UE_id;
			(*it1).oldNDI_UL=-1;
			(*it1).CRC_indication=get_CRC_indication();
			(*it1).round=1;
			(*it1).ul_active=true;
			(*it1).configured=false;
			(*it1).sche_Msg5_Time=-1;
			(*it1).CE_Level=get_CE_level();
			(*it1).multi_tone=get_multi_tone_support();
			(*it1).PHR=PHR_table[(*it1).CE_Level][(*it1).multi_tone];
			(*it1).DV=get_DV_index();
			(*it1).UL_Buffer_Size=DV_table[(*it1).DV];
			(*it1).first_Arrival_Time=H_SFN*10240+frame*10+subframes;
			(*it1).next_Arrival_Time=-1;
			(*it1).allocate_Buffer_Size=1;
			(*it1).mcs=-1;
			(*it1).Qm=-1;
			(*it1).remaining_Buffer_Sizes=-1;
			(*it1).startFreqPos=-1;
			(*it1).num_tone=-1;
			(*it1).num_UL_Slot=-1;
			(*it1).num_RU=-1;
			(*it1).remainging_subframe=-1;
			LOG("UE_id:%d,round:%d,CE_Level:%d,multi_tone:%d,PHR:%d,DV:%d,UL_Buffer_Size:%d,first_Arrival_Time:%d\n",(*it1).UE_id,(*it1).round,(*it1).CE_Level,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time);
			system("pause");
		}
	}
	else//check UL_UE_Info. if BSR arrival
	{
		// UE_TEMPLATE_NB UL_UE_Info;
		// list<UE_TEMPLATE_NB> % UE_Info_List = UL_Indicaiton.UL_UE_Info_List;
		// typename list<UE_TEMPLATE_NB>::iterator it1;
		for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		{
			if((H_SFN * 10240+frame * 10+subframes)==(*it1).next_Arrival_Time)
			{
				(*it1).CRC_indication=get_CRC_indication();
				(*it1).round=1;
				(*it1).BSR=get_BSR_index((*it1).remaining_Buffer_Sizes);
				(*it1).UL_Buffer_Size=BSR_table[(*it1).BSR];
			}
		}
	}
	if(H_SFN * 10240+frame * 10+subframes==(*it1).sche_Msg5_Time)
	{
		(*it1).configured=true;//Receive ACK for Msg4
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

uint32_t Get_pattern_base(SIB2_NB * SIB2_NB_S,vector<int> & resourceAllocPattern)
{
	int F_lowest_CE0=SIB2_NB_S->subcarrier_Offset[0];
	int F_lowest_CE1=SIB2_NB_S->subcarrier_Offset[1];
	int F_lowest_CE2=SIB2_NB_S->subcarrier_Offset[2];
	int F_highest_CE0=SIB2_NB_S->subcarrier_Offset[0]+SIB2_NB_S->num_Subcarrier[0];
	int F_highest_CE1=SIB2_NB_S->subcarrier_Offset[1]+SIB2_NB_S->num_Subcarrier[1];
	int F_highest_CE2=SIB2_NB_S->subcarrier_Offset[2]+SIB2_NB_S->num_Subcarrier[2];
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
	Pattern_base_List.sort (compareMyType);
	// LOG("Sort Pattern_base_List:\n");
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
	Pattern_base_List.sort (compareMyType1);
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



uint32_t resourceAllocation(SIB2_NB * SIB2_NB_S,uint32_t **UL_Channel_bitmap,list<UE_TEMPLATE_NB> & UE_Info_List)
{
	//Get pattern-base base on changed NPRACH configuration and multi-tone option
	if(SIB2_NB_S->flag_NPRACH_Change)
	{
		Pattern_base Pattern_base_S={0};
		vector<int> Sfreq;
		Get_pattern_base(SIB2_NB_S,Sfreq);
		LOG("Pattern-base, Sfreq[%d]: ",Sfreq.size());
		for (vector<int>::iterator it = Sfreq.begin() ; it != Sfreq.end(); ++it)
		{
			LOG("%d ",*it);
		}
		LOG("\n");
		SIB2_NB_S->flag_NPRACH_Change=false;
	}
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
