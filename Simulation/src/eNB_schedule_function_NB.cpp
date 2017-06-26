/*All Parameters related to scheduling MIB/SIB1,23,NPDCCH config are element in mac_sche_ctrl*/
#include "sche_pro_NB.h"
#include "dci_NB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
// #include "interface_NB.h"

using namespace std;

#define LOG(...) printf(__VA_ARGS__)


// uint32_t sib1_Period=256;//256 RF
// bool shcedSIB1=false,schedSIB2=false,lock=false;
extern uint32_t CSS_NPDCCH_period[3];
extern uint32_t USS_NPDCCH_period[3];
// extern uint32_t NPDCCH_period;

uint32_t si_RepetitionPattern[4]={1,4,3};
uint32_t max_mcs[2]={10,12};
uint32_t mapped_mcs[3][8]={{1,5,9,10,3,7,11,12},
							{0,3,7,10,3,7,11,12},
							{0,2,6,10,0,4,8,12}};
#define L_DCI_PDU 6
char DCI_PDU_type[L_DCI_PDU][20] = {
	"DCIFormatN0",
	"DCIFormatN1",
	"DCIFormatN1_RA",
	"DCIFormatN1_RAR",
	"DCIFormat2_Ind",
	"DCIFormat2_Pag"
};

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
// void reserve_schedule_MIB(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channel_bitmap, bool initialSche)
// {
	// uint32_t scheSubframe=0,scheFrame=0;
	// int i;
	// scheFrame=frame;
	// if(initialSche)	scheSubframe=subframes;
	// else scheSubframe=subframes+1;
	// if(scheSubframe==10)
	// {
	// 	scheSubframe=0;
	// 	++scheFrame;
	// }
	// for (int i = 0; i < NPDCCH_period; ++i)
	// {
	// 	if(scheSubframe==5)	DL_Channel_bitmap[i]=NPSS; //Reserve for NPSS, not send MAC_PDU/DCI
	// 	else if(((scheFrame & 0x01)==0)&&(scheSubframe==9))
	// 	{
	// 		// LOG("(frame & 0x01):%d\n",(frame & 0x01));
	// 		DL_Channel_bitmap[i]=NSSS;//Reserve for NSSS, not send MAC_PDU/DCI
	// 	}
	// 	else if(scheSubframe==0)	DL_Channel_bitmap[i]=NPBCH;//mac_rrc_data_req(get MIB content from RRC)

	// LOG("frame:%d,subframe:%d,Channel:%s\n",frame,subframes,channel_N[DL_Channel_bitmap[frame*10+subframes]]);
		// LOG("frame:%d,subframe:%d,Channel:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
		// system("pause");

// 		++scheSubframe;
// 		if(scheSubframe==10)
// 		{
// 			scheSubframe=0;
// 			++scheFrame;
// 		}
// 		if(scheFrame==1024)	scheFrame=0;
// 	}
// }

// uint32_t t_si_Period,t_si_windowStart,si_windowEnd;
// void reserve_schedule_SI(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channel_bitmap,MIB_NB * MIB_NB_S, SIB1_NB * SIB1_NB_S,bool initialSche)//schedule SIB1/SIB2/3
// {
	/*SIB1-NB in subframe #4 of every other frame in 16 continuous frames. Period = 256RF*/
	// uint32_t repetitionNum_SIB1=repetiitonSIB1(MIB_NB_S->schedulingInfoSIB1);
	// uint32_t startRF_SIB1=RFstartSIB1(repetitionNum_SIB1);
	// uint32_t TBS_Index=MIB_NB_S->schedulingInfoSIB1;
	// uint32_t TBS_SIB1=getTBS_SIB1(TBS_Index);
	// uint32_t repetitionOffset=sib1_Period/repetitionNum_SIB1;//if offset=64-->0,64,128,192,256....

	// uint32_t scheSubframe=0,scheFrame=0;
	// int i;
	// scheFrame=frame;
	// if(initialSche)	scheSubframe=subframes;
	// else scheSubframe=subframes+1;
	// if(scheSubframe==10)
	// {
	// 	scheSubframe=0;
	// 	++scheFrame;
	// }
	// for (int i = 0; i < NPDCCH_period; ++i)
	// {
		//Schedule SIB1 and disable shcedule SIB1 from every 16 frames to next repetiiton.
		// if(scheFrame%repetitionOffset==0)	shcedSIB1=true;
		// else if(scheFrame%repetitionOffset==16)	shcedSIB1=false;
		//Bug fixed(frame & 0x01==0)-->((frame & 0x01)==0)
		//starting schedule SIB1 from first RF or second RF base on sib1_startingRF
		// if (shcedSIB1&&((scheFrame & 0x01)==sib1_startingRF)&&(scheSubframe==4))
		// {
		// 	DL_Channel_bitmap[i]=SIB1;
		// }
		// LOG("repetitionNum_SIB1:%d,startRF_SIB1:%d,TBS_SIB1:%d,repetitionOffset:%d\n",repetitionNum_SIB1,startRF_SIB1,TBS_SIB1,repetitionOffset);
		// system("pause");

		/*SIB2-NB,SIB3-NB use same scheudlingInfo in subframe # within si-window. Period = 64RF*/
		// uint32_t si_windowStart=get_si_windowStart(SIB1_NB_S,scheFrame);//get start frame or -1
		/*Bug fixed, After free memory, the value will change even no assignment
		uint32_t t_si_windowStart,si_windowEnd,SIscheSubframe;
		uint32_t t_si_Period;*/
		// uint32_t SIscheSubframe;
		// LOG("si_windowStart:%d,si_windowEnd:%d\n",si_windowStart,si_windowEnd);
		// if((si_windowStart!=-1)&&(!lock))
		// {
		// 	// LOG("Start Sche SIB2-NB\n");
		// 	lock=true;
		// 	t_si_windowStart=si_windowStart;
		// 	si_windowEnd=si_windowStart+(SIB1_NB_S->si_WindowLength/10)-1;
		// 	t_si_Period=si_windowStart+(SIB1_NB_S->si_Periodicity/10)-1;
			// LOG("si_windowStart:%d,si_windowEnd:%d,next_si_Period:%d\n",t_si_windowStart,si_windowEnd,t_si_Period);
			// system("pause");
		// }
		//Schedule SIB2/3 within SI-window base on si-Repetiiton
		// if((scheFrame>=t_si_windowStart)&&(scheFrame<=si_windowEnd))
		// {
		// 	if(((scheFrame-SIB1_NB_S->si_RadioFrameOffset) & SIB1_NB_S->si_RepetitionPattern)==0)
		// 	{
		// 		schedSIB2=true;
		// 		if(scheSubframe==0)	SIscheSubframe=get_si_scheSubframe(SIB1_NB_S->si_TB);// for SIB2/3-NB
		// 	}
		// 	else	schedSIB2=false;
		// }

		// if((DL_Channel_bitmap[i]==NA)&&(schedSIB2)&&(SIscheSubframe>0))
		// {
			// LOG("si_windowStart:%d,si_windowEnd:%d,SIscheSubframe:%d\n",t_si_windowStart,si_windowEnd,SIscheSubframe);
			// system("pause");
			// DL_Channel_bitmap[i]=SIB2_3;//call mac_rrc_data_req();
			// --SIscheSubframe;
			// if((scheFrame==si_windowEnd)&&(SIscheSubframe>0))
			// {
			// 	LOG("SIB2-NB not finish transmission within si-window\n");
			// 	exit(1);
			// }
		// }
		// if((scheFrame==t_si_Period)&&(scheSubframe==9))	lock=false;//Wake up scheSIB2 at the end of this si-period.
		// LOG("frame:%d,subframe:%d,Channel:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
		// system("pause");
		// ++scheSubframe;
		// if(scheSubframe==10)
		// {
		// 	scheSubframe=0;
		// 	++scheFrame;
		// }
		// if(scheFrame==1024)	scheFrame=0;
	// }
// }



bool scheFlag=false;
// Sche_RES_t Sche_Response;//Should defined in PHY.
extern Sche_RES_t Sche_Response;
// extern vector <uint32_t> b0,b1,b2;
extern vector<vector<uint32_t> > locationS;
extern uint32_t DCI_Rep[3];
extern vector<int> Sfreq;
extern vector<uint32_t> UL_Channel;
extern uint32_t totalNumUE;
extern uint8_t CSS_flag;
int EndPoint=0;
// extern ofstream resourceUtilization,AverageDelay;
int Sum_Occupied_resource__U=0;
int Sum_Delay=0;
extern uint8_t runCase;
extern uint8_t TotalNumUE[10];
extern uint8_t TotalNumUE_H[10];//{12,24,36,48,60,72,84,96,108,120};
extern int End_Time;
extern uint8_t highOfferedLoad;
// clock_t tStart = clock();
// /* Do your stuff here */
// printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
// return 0;

// void NB_schedule_ulsch(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channel_bitmap,
// 	uint32_t **UL_Channel_bitmap,SIB2_NB * SIB2_NB_S,UL_IND_t & UL_Indicaiton)
void NB_schedule_ulsch(uint32_t scheH_SFN,frame_t scheFrame,sub_frame_t scheSubframe,uint32_t CE_Level,MIB_NB & MIB_NB_S,SIB1_NB & SIB1_NB_S,SIB2_NB & SIB2_NB_S,RRCCoonectionSetup_NB & Msg4_S,UL_IND_t & UL_Indicaiton,uint8_t & CSS_flag)
{
	// DCI_UL_PDU_t * ULSCH_dci=NULL;
	// uint32_t DCI_Rep[3]={SIB2_NB_S.rep[0],SIB2_NB_S.rep[1],SIB2_NB_S.rep[2]};
	//UL scheduler maintain a DCI_List
	// queue<HI_DCI0_request_t> & DCI_List=Sche_Response.DCI_L;
	// DCI_List.PDU_Type
	// DCI_List.num_DCI
	// DCI_List.DCI_Format.DCI_UL_PDU.NCCE_index
	// DCI_List.DCI_Format.DCI_UL_PDU.Aggregation_L
	// DCI_List.DCI_Format.DCI_UL_PDU.Start_symbol
	// DCI_List.DCI_Format.DCI_UL_PDUstartTime;
	// DCI_List.DCI_Format.DCI_UL_PDUendTime;
	// DCI_List.DCI_Format.DCI_UL_PDU.DCIN0
	// ! \brief UE list used by eNB to order UEs for scheduling
	if(CE_Level==0 || CE_Level==1 || CE_Level==2)	scheFlag=true;
	else	scheFlag=false;

	if(scheFlag)//CE 0/1/2
	{
		LOG("Total Num served UE:%d\n",totalNumUE);
		LOG("[NB_schedule_ulsch:CE%d, CSS_flag:%d ]\n",CE_Level,CSS_flag);
		// system("pause");
		list<UE_TEMPLATE_NB> & UE_Info_List=UL_Indicaiton.UL_UE_Info_List[CE_Level];//Get the sche_Info of CE 0/1/2 UE_list.
		list<HI_DCI0_request_t> & DCI_List=Sche_Response.DCI_L;
		typename list<UE_TEMPLATE_NB>::iterator it1;// distinguish Template and type
		// DCIs start/end time in search space
		// DCI_Queue.DCI_Format.DCI_UL_PDU.Aggregation_L
		// DCI_Queue.DCI_Format.DCI_UL_PDUstartTime;
		// DCI_Queue.DCI_Format.DCI_UL_PDUendTime;
		// DCI_Queue.DCI_Format.DCI_UL_PDU.DCIN0
  		//queue<HI_DCI0_request_t> & DCI_Queue
        // extern uint32_t CSS_NPDCCH_period[3];
        // extern uint32_t USS_NPDCCH_period[3];
        uint32_t NPDCCH_period=0,npdcch_Offset=0,T_SearchSpace=0;
        if(CSS_flag==1)
        {
            NPDCCH_period=CSS_NPDCCH_period[CE_Level];
            npdcch_Offset=SIB2_NB_S.npdcch_Offset_RA[CE_Level];
            T_SearchSpace=SIB2_NB_S.npdcch_NumRepetitions_RA[CE_Level];
        }
        else
        {
           NPDCCH_period=USS_NPDCCH_period[CE_Level];
           npdcch_Offset=Msg4_S.UE_specificConfig.npdcch_Offset_USS[CE_Level];
           T_SearchSpace=Msg4_S.UE_specificConfig.npdcch_NumRepetitions[CE_Level];
        }
		// uint32_t StartSearchSpace=scheH_SFN * 10240+ scheFrame * 10 + scheSubframe + npdcch_Offset * NPDCCH_period;
        //Update start time of search space in pp
        uint32_t offsetSearchSpace=npdcch_Offset * NPDCCH_period;
        LOG("offsetSearchSpace:%d,H_SFN:%d,frame:%d,subframe:%d\n",offsetSearchSpace,scheH_SFN,scheFrame,scheSubframe);
        for (int i = 0; i < offsetSearchSpace; ++i)
        {
            ++scheSubframe;
            if(scheFrame==10)
            {
                scheSubframe=0;
                ++scheFrame;
            }
            if(scheFrame==1024)
            {
                scheFrame=0;
                ++scheH_SFN;
            }
        }
        LOG("StartTime of search space:%d,H_SFN:%d,frame:%d,subframe:%d\n",offsetSearchSpace,scheH_SFN,scheFrame,scheSubframe);
		uint32_t cntSearchspace=0,cnt_default=0;// cnt used searchspace and # of control signal
	  	// uint32_t index_S=0;//record index of current occupued search space....

    	UE_Info_List.sort(compareMyType3);//sort UEs base on round

		for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		{
			LOG("CSS_flag:%d,UE_id:%d,CRC_indication:%d,round:%d,UL_Buffer_Size:%d,sche_Msg5_Time:%d,schedMsg3:%d,configured:%d,\n",CSS_flag,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).UL_Buffer_Size,(*it1).sche_Msg5_Time,(*it1).schedMsg3,(*it1).configured);
			// system("pause");
            (*it1).schedStatus=false;
			//LTE: don't schedule if Msg4 is not received yet/ NB-IoT: Reschedule Msg3 by DCI N0
		    if (((*it1).configured==false)&&((*it1).sche_Msg5_Time==-1)&&((*it1).round>1)) {
				// LOG("frame %d subfarme %d, UE %d: not configured, skipping UE scheduling \n",
				// scheFrame,scheSubframe,(*it1).UE_id);
		    	(*it1).schedMsg3=true;
				LOG("Reschedule Msg3: CE_Level:%d, UE_id:%d,schedMsg3:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).schedMsg3);
				// system("pause");
				//Update Msg3 next_Arrival_Time after schedule Msg3
				// continue;//Pospone until the other stuff done...
		    }
		    //Wait until receive ack for Msg4, the time=sche_Msg5_Time
		    if(((*it1).configured==false)&&((*it1).sche_Msg5_Time!=-1)&&(CSS_flag==0))
		    {
				LOG("CE_Level:%d,UE_id:%d,schedMsg3:%d,CRC_indication:%d,round:%d,multi_tone:%d,first_Arrival_Time:%d,sche_Msg5_Time:%d,UL_Buffer_Size:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).schedMsg3,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*it1).UL_Buffer_Size);
				LOG("Wait until receive ack for Msg4..\n");
				// system("pause");
		    	continue;
			}

		    //Check if UE_is_to_be_scheduled, UL_Buffer_Size only update when receive DV and BSR
		    //Some UE info like UL_Buffer_Size,.. should be reset after each shcedule round.
            //If schedule Msg3 retransmission-->keep going
            //If BSR=0, delete UE
		    if (((*it1).UL_Buffer_Size <= 0)&&((*it1).schedMsg3==false))//BSR=0
            {
                LOG("UL_Buffer_Size <= 0 schedMsg3==false\n");
                // LOG("CE_Level:%d,UE_id:%d,schedMsg3:%d,CRC_indication:%d,round:%d,multi_tone:%d,first_Arrival_Time:%d,sche_Msg5_Time:%d,UL_Buffer_Size:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).schedMsg3,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*it1).UL_Buffer_Size);
                LOG("[Delete] CE_Level:%d,UE_id:%d,UL_Buffer_Size:%d,BSR:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).BSR);
                // resourceUtilization << numUE << "," << utilization/simTimes << endl;
                // AverageDelay << numUE << "," << utilization/simTimes << endl;
                Sum_Delay=Sum_Delay+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
                End_Time=(*it1).next_Arrival_Time;
                LOG("End_Time:%d,Sum_Delay:%d\n",End_Time,Sum_Delay);
                // system("pause");
                it1 = UE_Info_List.erase (it1);
                --it1;
                // system("pause");
		    	continue;
		    }
            //CSS:schedule Msg3 retransmission, USS:shcedule UE-specific.
            if(((*it1).schedMsg3==false)&&(CSS_flag==1))
            {
                LOG("UE_id:%d,configured:%d for UE-specific shcedule",(*it1).UE_id,(*it1).configured);
                // system("pause");
                continue;
            }
            else if(((*it1).schedMsg3==true)&&(CSS_flag==0))
            {
                LOG("UE_id:%d,configured:%d for Common shcedule(Msg3 retransmission)",(*it1).UE_id,(*it1).configured);
                // system("pause");
                continue;
            }

			//Step 1: DCI resource determination
			HI_DCI0_request_t DCI_Info={0};
			bool Find_S=true;//find start time of DCI only once
            uint32_t DCI_S=0;
	    	while(cntSearchspace<=T_SearchSpace)
    		{
                // LOG("scheH_SFN:%d,scheFrame:%d,scheSubframe:%d\n",scheH_SFN,scheFrame,scheSubframe);
	    		DCI_S=check_if_DL_subframe(scheH_SFN,scheFrame,scheSubframe,MIB_NB_S,SIB1_NB_S);
                LOG("CurrentTime:%d,cntSearchspace:%d\n",DCI_S,cntSearchspace);
	    		if(DCI_S!=0)//Get current time of DL subframe
	    		{
					if(DCI_List.empty())
					{
						if(Find_S)
						{
							for (int j=0;j<locationS[CE_Level].size(); j++)//locationS: 3X8 array
							{
								if(locationS[CE_Level][j]==cntSearchspace)
								{
						  			DCI_Info.DCI_Format.DCI_UL_PDU.startTime=DCI_S;
						  			Find_S=false;
								}
							}
						}
						DCI_Info.DCI_Format.DCI_UL_PDU.cntR++;
						if(DCI_Info.DCI_Format.DCI_UL_PDU.cntR==DCI_Rep[CE_Level])
						{
							DCI_Info.DCI_Format.DCI_UL_PDU.endTime=DCI_S;
							// DCI_Info.num_DCI++;
							DCI_List.push_back  (DCI_Info);
							(*it1).schedStatus=true;
							// ++cntSearchspace;//This should be update with scheFrame/subframes in sync.
							break;
						}
					}
					else
					{
						typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.end();
						--DCI_it1;
						if((*DCI_it1).DCI_Format.DCI_UL_PDU.endTime < DCI_S)
						{
							if(Find_S)
							{
								for (int j=0;j<locationS[CE_Level].size(); j++)//locationS: 3X8 array
								{
									if(locationS[CE_Level][j]==cntSearchspace)//(0,2,4,6,8,10,12,14), (0,4,8,12,16,20,24,28)
									{
							  			DCI_Info.DCI_Format.DCI_UL_PDU.startTime=DCI_S;
							  			Find_S=false;
									}
								}
							}
							if(!Find_S)  DCI_Info.DCI_Format.DCI_UL_PDU.cntR++;//Bug fixed: Add if(!Find_S)
							if(DCI_Info.DCI_Format.DCI_UL_PDU.cntR==DCI_Rep[CE_Level])
							{
								DCI_Info.DCI_Format.DCI_UL_PDU.endTime=DCI_S;
								// DCI_Info.num_DCI++;
								DCI_List.push_back  (DCI_Info);
								(*it1).schedStatus=true;
								// ++cntSearchspace;//This should be update with scheFrame/subframes in sync.
								break;
							}
						}
					}
					++cntSearchspace;
		    		// LOG("[NB_schedule_dlsch]: frame:%d,subframes:%d,DL_subframe\n",scheFrame,scheSubframe);
		    		// system("pause");
	    		}
	    		else	++cnt_default;
	    		++scheSubframe;
	    		if(scheSubframe==10)
	    		{
	    			scheSubframe=0;
	    			++scheFrame;
	    		}
	    		if(scheFrame==1024)
	    		{
	    			scheFrame=0;
	    			scheH_SFN++;
	      		}
			    // cout<<"cnt_default:"<<cnt_default<<"cntSearchspace: "<<cntSearchspace<<endl;
			    // system("pause");
			}
			//Check if DCI have available reosurce in this pp.
			if((*it1).schedStatus==false)
            {
                LOG("No DCIs available reosurce in this pp...\n");
                continue;
            }

			LOG("DCI_List(1,2....m):\n");
			for (list<HI_DCI0_request_t>::iterator DCI_it1=DCI_List.begin(); DCI_it1 != DCI_List.end(); ++DCI_it1)
			{
				LOG("startTime:%d,endTime%d\n",(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime);
			}
			// system("pause");

			//Step 2: Scheduling dealy & subcarrier indication determination
			// resourceAllocation(SIB2_NB_S,UE_Info_List);//scheduling delay(Idelay)/subcarrier ind(Isc)
			// SIB2_NB_S.period[i]
			// SIB2_NB_S.rep[i]
			// SIB2_NB_S.start_time[i]
			// SIB2_NB_S.num_Subcarrier[i]
			// SIB2_NB_S.subcarrier_Offset[i]

			typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.end();
			--DCI_it1;
			//UL_ChannelTime: record start, end time of this UL transmission
			uint32_t UL_ChannelTime=0,Isc=0,ScheDelay=0;
			uint32_t shcedulingdelay[4]={8,16,32,64};
			for (int i = 0; i < sizeof(shcedulingdelay)/sizeof(uint32_t); ++i)
			{
				for (int j = 0; j < Sfreq.size(); ++j)//{0,4,8,12,16,20,24,28,32,36,40,44}
				{
					UL_ChannelTime=(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime+shcedulingdelay[i];
					//UL_Channel array record the last time for 12 tone, update when UL transmission ends in X.
					if(UL_ChannelTime>UL_Channel[j])
					{
						if(check_if_NPRACH(SIB2_NB_S,UL_ChannelTime,Sfreq[j])==1)
							continue;
						UL_Channel[j]=UL_ChannelTime;//record startTime of UL transmission->use for UL_CONFIG_Request
						ScheDelay=shcedulingdelay[i];
						// (*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.scind=j;
						Isc=j;
						break;
					}
				}
				if(ScheDelay!=0)	break;
			}
			// LOG("UE_id:%d,Idelay:%d,Isc:%d,UL transmission startTime:%d\n",(*it1).UE_id,ScheDelay,Isc,UL_Channel[Isc]);
			// system("pause");
			//delete DCI in DCI list if there's no available UL resource-->sche this UE in next pp.
			if(ScheDelay==0)
			{
				DCI_List.pop_back ();
				(*it1).schedStatus=false;
				LOG("Can't find available UL resource skip this UE scheduling in this pp...\n");
				continue;//skip this loop for this UE.
			}

			//Step 3: Mcs & Nru determination
            // Consider reschedule for Msg3-->(*it1)..schedMsg3=true;
			//First Round to calculate RU required by each UEs
			uint32_t mcs=max_mcs[(*it1).multi_tone];//max_mcs[2]={10,12};
			// uint32_t mapped_mcs[3][8]={{1,5,9,10,3,7,11,12},
			// 							{0,3,7,10,3,7,11,12},
			// 							{0,2,6,10,0,4,8,12}};
			uint32_t mappedMcsIndex=(*it1).PHR+4 * (*it1).multi_tone;
			// uint32_t mapped_mcs[(*it1).CE_Level][mappedMcsIndex];
			uint32_t ru_index=0;
            int TBS=0;
			bool max_Iru=false;
			uint32_t RU_table[8]={1,2,3,4,5,6,8,10};
			TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
			// LOG("mcs:%d,mapped_mcs:%d,TBS:%d\n",mcs,mapped_mcs[(*it1).CE_Level][mappedMcsIndex],TBS);
            if((*it1).schedMsg3)
            {
                mcs=2;//Imcs=010;,pi/4 QPSK,Nru=1,TBS=88 bits
                TBS=88>>3;
                ru_index=0;
            }
            else
            {
    			while((mapped_mcs[(*it1).CE_Level][mappedMcsIndex]<mcs)||((TBS>(*it1).UL_Buffer_Size)&&(mcs>=0)))
    			{
    				// LOG("XX");
    				// system("pause");
    				--mcs;
    				TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
    				// LOG("mcs:%d,TBS:%d\n",mcs,TBS);
    			}
    			while((TBS<(*it1).UL_Buffer_Size)&&(ru_index<=7))
    			{
    				// LOG("YY");
    				// system("pause");
    				ru_index++;
    				TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
    				if(max_Iru)	break;
    			}
            }
			(*it1).allocate_Buffer_Size=TBS;

			//Step 4: Pre UL resource mapping
			uint32_t Nrep[3]={SIB2_NB_S.rep[0],SIB2_NB_S.rep[1],SIB2_NB_S.rep[2]};
            uint32_t Nulslots=num_ULslots(Isc);
            //1 UL slots=0.5ms
			uint32_t N=RU_table[ru_index] * Nrep[(*it1).CE_Level] * Nulslots * 0.5;
            //How many reosurce occupied in this UE(single-tone)
            Sum_Occupied_resource__U=Sum_Occupied_resource__U+N;
			while(N>0)
			{
				// LOG("N:%d\n",N);
				// system("pause");
				//Sfreq[i]={0,4,8,12,16,20,24,28,32,36,40,44}
				if(check_if_NPRACH(SIB2_NB_S,UL_ChannelTime,Sfreq[Isc])==1)
				{
					++UL_ChannelTime;
					continue;
				}
				else//This time no NPRACH resource.
				{
					++UL_ChannelTime;
					N--;
				}
			}
			N=RU_table[ru_index] * Nrep[(*it1).CE_Level] * num_ULslots(Isc);
			//Sched UL transmission will end in UL_ChannelTime-->record to (*it1).next_Arrival_Time.(BSR)
			(*it1).next_Arrival_Time=UL_ChannelTime;
			LOG("CE_Level:%d,UE_id:%d,FirstMsg3ArrivalTime:%d,schedMsg3:%d,ACKforMsg4Time:%d,DCI_StartTime/EndTime:%d/%d,ScheDelay:%d,N:%d,Sched UL transmission startTime/EndTime:%d/%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).first_Arrival_Time,(*it1).schedMsg3,(*it1).sche_Msg5_Time,(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime,ScheDelay,N,UL_Channel[Isc],(*it1).next_Arrival_Time);
			// LOG("\n");
			LOG("CE_Level:%d,UE_id:%d,CRC_indication:%d,round:%d,multi_tone:%d,PHR:%d,MSC:%d,DV:%d,BSR:%d,UL_Buffer_Size:%d,TBS:%d,Nru:%d,Nrep:%d,Nulslots:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,mcs,(*it1).DV,(*it1).BSR,(*it1).UL_Buffer_Size,TBS,RU_table[ru_index],Nrep[(*it1).CE_Level],Nulslots);
			// LOG("\n");
			// system("pause");




			// UL_Channel[j] record last time of the Isc
			UL_Channel[Isc]=UL_ChannelTime;

			//Step 5: Fill above DCI filed(Isc,Idelay,msc,Iru) and flag/NDI/RV/DCI_Rep/Irep
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.type=1;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.Scheddly=ScheDelay;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.scind=Isc;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ResAssign=ru_index;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.mcs=mcs;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.RepNum=Nrep[(*it1).CE_Level];
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.rv=0;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.DCIRep=get_DCI_Filed(T_SearchSpace,DCI_Rep[(*it1).CE_Level]);

			if((*it1).round>1)	(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi=(*it1).oldNDI_UL;
			else	(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi=1-(*it1).oldNDI_UL;
			(*it1).oldNDI_UL=(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi;

			LOG("DCI format N0:\n\tflag:%d\n\tIdelay:%d\n\tIsc:%d\n\tIru:%d\n\tmcs:%d\n\tIrep:%d\n\tRV:%d\n\tNDI:%d\n\tDCIRep:%d\n",(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.type,ScheDelay,Isc,ru_index,mcs,Nrep[(*it1).CE_Level],(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.rv,(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi,(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.DCIRep);
			// system("pause");
			LOG("\n");
		}//loop for UE List

        //Add two case: 1 is updated BSR, one is only one DV/BSR
		LOG("Sched UEs done in UE_List: UL_Buffer_Size update when receive DV/BSR\n");
		for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		{
			if(((*it1).schedStatus==true)&&((*it1).schedMsg3==false))//ScheduleDone this pp, reset UL Buffer size, it only update when receive DV and BSR
			{
				// (*it1).remaining_Buffer_Sizes=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
				// if((*it1).remaining_Buffer_Sizes<=0), Bug: can't judge nagatove value..
				if(((*it1).allocate_Buffer_Size >= (*it1).UL_Buffer_Size))
				{
					// (*it1).UL_Buffer_Size=0;
					LOG("[Delete] CE_Level:%d,UE_id:%d,UL_Buffer_Size:%d,TBS:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).allocate_Buffer_Size);
                    // resourceUtilization << numUE << "," << utilization/simTimes << endl;
                    // AverageDelay << numUE << "," << utilization/simTimes << endl;
                    Sum_Delay=Sum_Delay+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
                    End_Time=(*it1).next_Arrival_Time;
                    LOG("End_Time:%d,Sum_Delay:%d\n",End_Time,Sum_Delay);
                    // system("pause");
                    it1 = UE_Info_List.erase (it1);
					--it1;
				}
				else
				{
                    // (*it1).remaining_Buffer_Sizes=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
                    LOG("[Reset] UE_id:%d,UL_Buffer_Size:%d, remaining_Buffer_Sizes:%d,BSRNextArrival:%d Update UL_Buffer_Size when receive DV or BSR\n",(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size,(*it1).next_Arrival_Time);
					(*it1).UL_Buffer_Size=0;
                    // (*it1).UL_Buffer_Size=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
				}
			}
		}
		LOG("UE_Info_List size:%d\n",UE_Info_List.size());
        for (int i = 0; i < 3; ++i)
        {
            list<UE_TEMPLATE_NB> & check_UE_Info_List=UL_Indicaiton.UL_UE_Info_List[i];
            // typename list<UE_TEMPLATE_NB>::iterator it1;// distinguish Template and type
            if(check_UE_Info_List.empty ())
            {
               if(EndPoint!=3)  ++EndPoint;
            }
        }
        if(highOfferedLoad!=1)
        {
            LOG("EndPoint:%d,TotalNumUE[runCase]:%d,totalNumUE:%d\n",EndPoint,TotalNumUE[runCase],totalNumUE);
            if((EndPoint!=3)||((TotalNumUE[runCase]-6)!=totalNumUE))  EndPoint=0;
        }
        else
        {
            LOG("EndPoint:%d,TotalNumUE[runCase]:%d,totalNumUE:%d\n",EndPoint,TotalNumUE_H[runCase],totalNumUE);
            if((EndPoint!=3)||(TotalNumUE_H[runCase]!=totalNumUE))  EndPoint=0;
  	    }
    }
}
void NB_schedule_dlsch(frame_t frame,sub_frame_t subframes,uint32_t *DL_Channel_bitmap)
{
	void	*DLSCH_dci = NULL;
	//Postpose
}

void NB_shceudle_RA(frame_t frame,sub_frame_t subframes,uint32_t *DL_Channel_bitmap,uint32_t **UL_Channel_bitmap)
{
	//Postpose
}
