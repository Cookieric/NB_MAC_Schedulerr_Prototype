#include "sche_pro_NB.h"
#include "dci_NB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include <fstream>
// #include "interface_NB.h"

using namespace std;

#define LOG(...) printf(__VA_ARGS__)


// uint32_t sib1_Period=256;//256 RF
// bool shcedSIB1=false,schedSIB2=false,lock=false;
extern uint32_t CSS_NPDCCH_period[3];
extern uint32_t USS_NPDCCH_period[3];
// extern uint32_t NPDCCH_period;
extern ofstream fout_LOG;

extern uint8_t File_LOG_Flag;
extern uint8_t LOG_Flag;

uint32_t si_RepetitionPattern[4]={1,4,3};
uint32_t max_mcs[2]={10,12};
// uint32_t mapped_mcs[3][8]={{1,5,9,10,3,7,11,12},
// 							{0,3,7,10,3,7,11,12},
// 							{0,2,6,10,0,4,8,12}};
uint32_t mapped_mcs[4]={1,4,7,10};
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


//Record DCI_Resource Time for NCCE 0 and 1
uint32_t T_DCI_Resource[2]={0};
bool scheFlag=false;
// Sche_RES_t Sche_Response;//Should defined in PHY.
extern Sche_RES_t Sche_Response;
// extern vector <uint32_t> b0,b1,b2;
extern vector<vector<uint32_t> > locationS;
extern uint32_t DCI_Rep[3];
extern vector<int> Sfreq;
extern vector<uint32_t> UL_Channel;
extern uint32_t totalNumUE;
// extern uint8_t CSS_flag;
int EndPoint=0;
// extern ofstream resourceUtilization,AverageDelay;
extern uint32_t Sum_Occupied_resource__U;
// int Sum_Delay=0;
uint32_t Sum_Delay[3]={0,0,0};

uint32_t P_Sum_Delay[2][3]={0};
uint32_t P_CE_NumUE[2][3]={0};

extern uint8_t runCase;
extern uint8_t TotalNumUE[10];
extern uint32_t TotalNumUE_H[10];//{12,24,36,48,60,72,84,96,108,120};
extern int End_Time;
extern uint8_t highOfferedLoad;
// clock_t tStart = clock();
// /* Do your stuff here */
// printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
// return 0;

uint32_t N=0;
// void NB_schedule_ulsch(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channel_bitmap,
// 	uint32_t **UL_Channel_bitmap,SIB2_NB * SIB2_NB_S,UL_IND_t & UL_Indicaiton)
void NB_schedule_ulsch(uint32_t scheH_SFN,frame_t scheFrame,sub_frame_t scheSubframe,uint32_t CE_Level,MIB_NB & MIB_NB_S,SIB1_NB & SIB1_NB_S,SIB2_NB & SIB2_NB_S,RRCCoonectionSetup_NB & Msg4_S,UL_IND_t & UL_Indicaiton)
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
	// if((CE_Level<0) || (CE_Level)>2 ) { // check if the value is out of range
 //    // add warning and print the value of CE_level, set ce_level to zero and proceed
 //        LOG("Warning: Exceed CE level definition, set to CE level to zero and proceed..\n");
 //        scheFlag = true;
 //        CE_Level = 0;
 //    } else scheFlag=true;
	scheFlag=true;

	if(scheFlag)//CE 0/1/2 if (CE_level
	{
	   	if(File_LOG_Flag)
		{
			fout_LOG<<"Total Num served UE:"<<totalNumUE<<endl;
		}
    	if(LOG_Flag)
		{
			LOG("Total Num served UE:%d\n",totalNumUE);
			// LOG("[NB_schedule_ulsch:CE%d, CSS_flag:%d ]\n",CE_Level,CSS_flag);
			// system("pause");
		}
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
        // if(CSS_flag==1) // if css_flag & uss_flag, else if css_flag, else // uss_flag
        // {
        //     NPDCCH_period=CSS_NPDCCH_period[CE_Level];
        //     npdcch_Offset=SIB2_NB_S.npdcch_Offset_RA[CE_Level];
        //     T_SearchSpace=SIB2_NB_S.npdcch_NumRepetitions_RA[CE_Level];
        // }
        // else
        // {
           NPDCCH_period=USS_NPDCCH_period[CE_Level];
           npdcch_Offset=Msg4_S.UE_specificConfig.npdcch_Offset_USS[CE_Level];
           T_SearchSpace=Msg4_S.UE_specificConfig.npdcch_NumRepetitions[CE_Level];
        // }
		// uint32_t StartSearchSpace=scheH_SFN * 10240+ scheFrame * 10 + scheSubframe + npdcch_Offset * NPDCCH_period;
        //Update start time of search space in pp
        uint32_t offsetSearchSpace=npdcch_Offset * NPDCCH_period;
        // LOG("offsetSearchSpace:%d,H_SFN:%d,frame:%d,subframe:%d\n",offsetSearchSpace,scheH_SFN,scheFrame,scheSubframe);
        uint32_t schedTime=scheH_SFN * 10240+scheFrame * 10+scheSubframe;
        scheSubframe=(schedTime+offsetSearchSpace)%10;
        scheFrame=((schedTime+offsetSearchSpace)/10)%1024;
        scheH_SFN=(schedTime+scheSubframe+offsetSearchSpace)/10240;
		schedTime=scheH_SFN * 10240+scheFrame * 10+scheSubframe;
        // for (int i = 0; i < offsetSearchSpace; ++i)
        // {
        //     ++scheSubframe;
        //     if(scheFrame==10)
        //     {
        //         scheSubframe=0;
        //         ++scheFrame;
        //     }
        //     if(scheFrame==1024)
        //     {
        //         scheFrame=0;
        //         ++scheH_SFN;
        //     }
        // }
        // LOG("StartTime of search space:%d,H_SFN:%d,frame:%d,subframe:%d\n",offsetSearchSpace,scheH_SFN,scheFrame,scheSubframe);
        // system("pause");
		uint32_t cntSearchspace=0;//cnt used searchspace and record index of current occupued search space....
        uint32_t cnt_default=0;// cnt # of non-DL subframe
        uint32_t num_DCI=0;//record # of DCIs for each HI_DCI0.requst message
    	UE_Info_List.sort(compareMyType3);//sort UEs base on round
    	// fout_LOG<<"[Sort UEs base on some requirements(Round/dataSize...)]"<<endl;
		for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		{
            (*it1).schedStatus=false;
            //CSS:schedule Msg3 retransmission, USS:shcedule UE-specific.
            // if(((*it1).schedMsg3==false)&&(CSS_flag==1))
            // {
            //     // LOG("UE_id:%d,configured:%d for UE-specific shcedule",(*it1).UE_id,(*it1).configured);
            //     // system("pause");
            //     continue;
            // }
            // else if(((*it1).schedMsg3==true)&&(CSS_flag==0))
            // {
            //     // LOG("UE_id:%d,configured:%d for Common shcedule(Msg3 retransmission)",(*it1).UE_id,(*it1).configured);
            //     // system("pause");
            //     continue;
            // }

			// fout_LOG<<"[Enter UE_list]"<<"UE_Id:"<<(*it1).UE_id<<endl;
			// LOG("CE_Level:%d,CSS_flag:%d,UE_id:%d,CRC_indication:%d,round:%d,UL_Buffer_Size:%d,sche_Msg5_Time:%d,schedMsg3:%d,configured:%d\n",(*it1).CE_Level,CSS_flag,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).UL_Buffer_Size,(*it1).sche_Msg5_Time,(*it1).schedMsg3,(*it1).configured);
			// system("pause");
			//LTE: don't schedule if Msg4 is not received yet/ NB-IoT: Reschedule Msg3 by DCI N0
		    // if (((*it1).configured==false)&&((*it1).sche_Msg5_Time==-1)&&((*it1).round>1)) {
				// LOG("frame %d subfarme %d, UE %d: not configured, skipping UE scheduling \n",
				// scheFrame,scheSubframe,(*it1).UE_id);
		    	// (*it1).schedMsg3=true;
				// LOG("Reschedule Msg3: CE_Level:%d, UE_id:%d,schedMsg3:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).schedMsg3);
				// system("pause");
				//Update Msg3 next_Arrival_Time after schedule Msg3
		    // }

		    //Wait until receive ack for Msg4, the time=sche_Msg5_Time
		    // if(((*it1).configured==false)&&((*it1).sche_Msg5_Time!=-1)&&(CSS_flag==0))->has bug in sche_Msg5_Time becasue cnfigured status not changed when currentTime=sche_Msg5_Time in ulsch_ind...
		    if(((*it1).sche_Msg5_Time>schedTime)||((*it1).configured==false))
		    {
		    	LOG("%d,%d\n",(*it1).sche_Msg5_Time,(*it1).configured);
		    	LOG("Assume RA succeed, so code should not go here..\n");
		    	system("pause");
		    	exit(1);

    	    	if(LOG_Flag)
				{
					LOG("CE_Level:%d,UE_id:%d,CRC_indication:%d,round:%d,multi_tone:%d,first_Arrival_Time:%d,sche_Msg5_Time:%d,UL_Buffer_Size:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*it1).UL_Buffer_Size);
					LOG("Wait until receive ack for Msg4..\n");
					// system("pause");
				}
			   	if(File_LOG_Flag)
				{
					fout_LOG<<"Wait until receive ack for Msg4.."<<"CE_Level:"<<(*it1).CE_Level<<" UE_id:"<<(*it1).UE_id<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<"configured:"<<(*it1).configured<<" first_Arrival_Time:"<<(*it1).first_Arrival_Time<<" sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<" UL_Buffer_Size"<<(*it1).UL_Buffer_Size<<endl;
				}
		    	continue;
			}
			//(*it1).next_Arrival_Time==-1, initilization
			if((*it1).next_Arrival_Time>schedTime)
			{
				if((*it1).next_Arrival_Time!=-1)
				{
				   	if(File_LOG_Flag)
					{
						fout_LOG<<"Not receive next UL transmission with BSR before this scheduling"<<",CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",CRC:"<<(*it1).CRC_indication<<",round:"<<(*it1).round<<",first_Arrival_Time:"<<",sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<"next_Arrival_Time"<<(*it1).next_Arrival_Time<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<endl;
						continue;
					}
				}
				else if((*it1).next_Arrival_Time==-1)
				{
    		    	if(File_LOG_Flag)
					{
						fout_LOG<<"initilize next_Arrival_Time=-1, ready to schedule Msg5"<<endl;
						fout_LOG<<"CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",CRC:"<<(*it1).CRC_indication<<",round:"<<(*it1).round<<",first_Arrival_Time:"<<",sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<"next_Arrival_Time"<<(*it1).next_Arrival_Time<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<endl;
						// system("pause");
					}
				}
		    }
		    //Check if UE_is_to_be_scheduled, UL_Buffer_Size only update when receive DV and BSR
		    //Some UE info like UL_Buffer_Size,.. should be reset after each shcedule round.
            //If schedule Msg3 retransmission-->keep going
            //If BSR=0, delete UE
		    // if(((*it1).BSR==0)&&((*it1).schedMsg3==false))
		    if (((*it1).UL_Buffer_Size <= 0)&&((*it1).configured==true))//BSR=0
            {
                // resourceUtilization << numUE << "," << utilization/simTimes << endl;
                // AverageDelay << numUE << "," << utilization/simTimes << endl;
                if(((*it1).next_Arrival_Time-(*it1).first_Arrival_Time)>10000)//delay buget:10s
                {
                    // LOG("[BSR=0] CE_Level:%d,UE_id:%d,first_Arrival_Time:%d,EndTime:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).first_Arrival_Time,(*it1).next_Arrival_Time);
                    fout_LOG<<"[BSR=0],exceed delay budget.."<<"CE_Level:"<<(*it1).CE_Level<<" UE_id "<<(*it1).UE_id<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<" first_Arrival_Time:"<<(*it1).first_Arrival_Time<<" sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<" UL_Buffer_Size"<<(*it1).UL_Buffer_Size<<endl;
                    // system("pause");
                    /*Delete fail device*/
                    it1 = UE_Info_List.erase (it1);
                	--it1;
		    		continue;
                }
                // Sum_Delay=Sum_Delay+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
                Sum_Delay[(*it1).CE_Level]=Sum_Delay[(*it1).CE_Level]+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
                End_Time=(*it1).next_Arrival_Time;


                P_Sum_Delay[(*it1).I_payloadSize][(*it1).CE_Level]=P_Sum_Delay[(*it1).I_payloadSize][(*it1).CE_Level]+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
                P_CE_NumUE[(*it1).I_payloadSize][(*it1).CE_Level]++;

    	    	if(LOG_Flag)
				{
	                LOG("BSR==0 configured==true\n");
	                LOG("CE_Level:%d,UE_id:%d,CRC_indication:%d,round:%d,multi_tone:%d,first_Arrival_Time:%d,sche_Msg5_Time:%d,UL_Buffer_Size:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,(*it1).DV,(*it1).UL_Buffer_Size,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*it1).UL_Buffer_Size);
	                LOG("[Delete] CE_Level:%d,UE_id:%d,UL_Buffer_Size:%d,BSR:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).BSR);
	                LOG("End_Time:%d,Sum_Delay[%d]:%d\n",End_Time,(*it1).CE_Level,Sum_Delay[(*it1).CE_Level]);
	                // system("pause");
	            }
			   	if(File_LOG_Flag)
				{
    				fout_LOG<<"BSR==0 configured==true"<<",CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",CRC:"<<(*it1).CRC_indication<<",round:"<<(*it1).round<<",first_Arrival_Time:"<<(*it1).first_Arrival_Time<<",sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<",End_Time:"<<End_Time<<endl;
					// system("pause");
				}
                it1 = UE_Info_List.erase (it1);
                --it1;
		    	continue;
		    }


			//Step 1: DCI resource determination(NCCE resource allocation)
			HI_DCI0_request_t DCI_Info={0};
            //Get Aggregation level base on Rmax/R and CE level
            DCI_Info.DCI_Format.DCI_UL_PDU.Aggregation_L=get_aggregation(CE_Level,T_SearchSpace,DCI_Rep[CE_Level]);
			bool Find_S=true;//find start time of DCI only once
            uint32_t DCI_S=0;
            //cntSearchspace:Index of search sapce,T_SearchSpace:The length of search sapce
	    	while(cntSearchspace<=T_SearchSpace/2)
    		{
    			// fout_LOG<<"[Step1]"<<"UE_Id:"<<(*it1).UE_id<<endl;
                // LOG("scheH_SFN:%d,scheFrame:%d,scheSubframe:%d\n",scheH_SFN,scheFrame,scheSubframe);
	    		DCI_S=check_if_DL_subframe(scheH_SFN,scheFrame,scheSubframe,MIB_NB_S,SIB1_NB_S);
                // LOG("CurrentTime:%d,cntSearchspace:%d\n",DCI_S,cntSearchspace);
	    		if(DCI_S!=0)//Get current time of DL subframe
	    		{
					if(DCI_List.empty())
					{
						if(Find_S)
						{
							//j is the maximun DCI size
							for (int j=0;j<locationS[CE_Level].size(); j++)//locationS: 3X8 array
							{
                                //If Rmax/R is the same for CSS/USS in the same CE level, locationS keep the same
                                //If Rmax/R is different for CSS/USS in the same CE level, locationS will changed...
                                //If a NB-IoT UE has a NPUSCH transmission ending in subframe n , the UE is not required to monitor NPDCCH in any subframe starting from subframe n+1 to subframe n+3
                                if((DCI_S-(*it1).first_Arrival_Time>=4)&&(DCI_S-(*it1).next_Arrival_Time>=4))
                                {
                                	//locationS:Possible DCI location in search space
                                	//(0,1,2,3,4,5,6,7)
                                	////(0,2,4,6,8,10,12,14), (0,4,8,12,16,20,24,28)
    								if(locationS[CE_Level][j]==cntSearchspace)
    								{
                                        //Start from NCCE index 0, this could be AL = 1 or 2
                                        DCI_Info.DCI_Format.DCI_UL_PDU.NCCE_index=0;
    						  			DCI_Info.DCI_Format.DCI_UL_PDU.startTime=DCI_S;
    						  			Find_S=false;
    								}
                                }
							}
						}
						if(!Find_S)   DCI_Info.DCI_Format.DCI_UL_PDU.cntR++;
						if(DCI_Info.DCI_Format.DCI_UL_PDU.cntR==DCI_Rep[CE_Level])
						{
							DCI_Info.DCI_Format.DCI_UL_PDU.endTime=DCI_S;
                            if(DCI_Info.DCI_Format.DCI_UL_PDU.Aggregation_L==2)
                            {
                                T_DCI_Resource[0]=DCI_S;
                                T_DCI_Resource[1]=DCI_S;
							}
                            else    T_DCI_Resource[0]=DCI_S;
                            //num_DCI used by PHY loop generate DCI top
                            // DCI_Info.num_DCI++;
                            num_DCI++;
							DCI_List.push_back  (DCI_Info);
							(*it1).schedStatus=true;
							// ++cntSearchspace;//This should be update with scheFrame/subframes in sync.
							break;
						}
					}
					else//DCI_List is not empty...
					{
                        uint32_t t_NCCE=0;//temp NCCE Index
                        if(DCI_Info.DCI_Format.DCI_UL_PDU.Aggregation_L==1)//For now Rmax/R setting will not go here
                        {
                            //Find the earlier time of NCCE0 or NCCE1 and record CCE index
                            if(T_DCI_Resource[0]>T_DCI_Resource[1])
                            {
                                t_NCCE=1;
                                //Check if available DCI resource >= start search space
                                if((DCI_S > T_DCI_Resource[1])&&(cntSearchspace==0))
                                {
                                    //If start search space > Time_NCCE0 set NCCE index=0
                                    if(DCI_S > T_DCI_Resource[0])    t_NCCE=0;
                                    T_DCI_Resource[t_NCCE]=DCI_S;
                                }
                            }
                            else
                            {
                                t_NCCE=0;
                                //Check if available DCI resource >= start search space
                                if((DCI_S > T_DCI_Resource[0])&&(cntSearchspace==0))
                                {
                                    // don't need this condition here as I always start from ncce 0
                                    // if(DCI_S > T_DCI_Resource[1])    t_NCCE=1;
                                    T_DCI_Resource[t_NCCE]=DCI_S;
                                }
                            }
                            //CCE allocation for DCI's AL=1
                            if((DCI_S > T_DCI_Resource[t_NCCE])||((DCI_S==T_DCI_Resource[t_NCCE])&&(cntSearchspace==0)))
                            {
                                if(Find_S)
                                {
                                    for (int j=0;j<locationS[CE_Level].size(); j++)//locationS: 3X8 array
                                    {
                                        //If a NB-IoT UE has a NPUSCH transmission ending in subframe n , the UE is not required to monitor NPDCCH in any subframe starting from subframe n+1 to subframe n+3
                                        if((DCI_S-(*it1).first_Arrival_Time>=4)&&(DCI_S-(*it1).next_Arrival_Time>=4))
                                        {
                                            if(locationS[CE_Level][j]==cntSearchspace)
                                            {
                                                DCI_Info.DCI_Format.DCI_UL_PDU.NCCE_index=t_NCCE;
                                                DCI_Info.DCI_Format.DCI_UL_PDU.startTime=DCI_S;
                                                Find_S=false;
                                            }
                                        }
                                    }
                                }
                                if(!Find_S)  DCI_Info.DCI_Format.DCI_UL_PDU.cntR++;//Bug fixed: Add if(!Find_S)
                                if(DCI_Info.DCI_Format.DCI_UL_PDU.cntR==DCI_Rep[CE_Level])
                                {
                                    T_DCI_Resource[DCI_Info.DCI_Format.DCI_UL_PDU.NCCE_index]=DCI_S;
                                    DCI_Info.DCI_Format.DCI_UL_PDU.endTime=DCI_S;
                                    // DCI_Info.num_DCI++;
                                    num_DCI++;
                                    DCI_List.push_back  (DCI_Info);
                                    (*it1).schedStatus=true;
                                    // ++cntSearchspace;//This should be update with scheFrame/subframes in sync.
                                    break;
                                }
                            }
                        }
                        else if(DCI_Info.DCI_Format.DCI_UL_PDU.Aggregation_L==2)
                        {
                            //Find the DCI's biggest endTime in DCI_list
                            DCI_List.sort(compareMyType4);
                            // for (list<HI_DCI0_request_t>::iterator DCI_it1=DCI_List.begin(); DCI_it1 != DCI_List.end(); ++DCI_it1)
                            // {
                                // LOG("startTime:%d,endTime%d,AL:%d\n",(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.Aggregation_L);
                            // }
                            // typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.end();
                            // --DCI_it1;
                            typename list<HI_DCI0_request_t>::iterator DCI_it1 = DCI_List.begin();
    						if((*DCI_it1).DCI_Format.DCI_UL_PDU.endTime < DCI_S)
    						{
    							if(Find_S)
    							{
    								for (int j=0;j<locationS[CE_Level].size(); j++)//locationS: 3X8 array
    								{
                                        if((DCI_S-(*it1).first_Arrival_Time>=4)&&(DCI_S-(*it1).next_Arrival_Time>=4))
                                        {
                                            //(0,2,4,6,8,10,12,14), (0,4,8,12,16,20,24,28)
        									if(locationS[CE_Level][j]==cntSearchspace)
        									{
                                                DCI_Info.DCI_Format.DCI_UL_PDU.NCCE_index=t_NCCE;
        							  			DCI_Info.DCI_Format.DCI_UL_PDU.startTime=DCI_S;
        							  			Find_S=false;
        									}
                                        }
    								}
    							}
    							if(!Find_S)  DCI_Info.DCI_Format.DCI_UL_PDU.cntR++;//Bug fixed: Add if(!Find_S)
    							if(DCI_Info.DCI_Format.DCI_UL_PDU.cntR==DCI_Rep[CE_Level])
    							{
    								DCI_Info.DCI_Format.DCI_UL_PDU.endTime=DCI_S;
                                    T_DCI_Resource[0]=DCI_S;
                                    T_DCI_Resource[1]=DCI_S;
    								// DCI_Info.num_DCI++;
                                    num_DCI++;
    								DCI_List.push_back  (DCI_Info);
    								(*it1).schedStatus=true;
    								// ++cntSearchspace;//This should be update with scheFrame/subframes in sync.
    								break;
    							}
    						}
                        }
					}
					++cntSearchspace;
		    		// LOG("[NB_schedule_dlsch]: frame:%d,subframes:%d,DL_subframe\n",scheFrame,scheSubframe);
		    		// system("pause");
	    		}
	    		else	++cnt_default;//Non-DL subframe
	    		// local timing for ulsch
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
			}// end of while
            //Recover DCIs squence base on earlier start time of DCIs in DCI_List .
            DCI_List.sort(compareMyType5);
			//Check if DCI have available reosurce in this pp.
			if((*it1).schedStatus==false)
            {
 			// fout_LOG<<"No DCIs available reosurce in this pp..."<<"CE_Level:"<<(*it1).CE_Level<<" UE_id "<<(*it1).UE_id<<" schedMsg3:"<<(*it1).schedMsg3<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<" first_Arrival_Time:"<<" sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<" UL_Buffer_Size"<<(*it1).UL_Buffer_Size<<endl;
                // LOG("No DCIs available reosurce in this pp...\n");
                // system("pause");
                continue;
            }
		   	if(LOG_Flag)
			{
				LOG("DCI_List(1,2....m):\n");
				for (list<HI_DCI0_request_t>::iterator DCI_it1=DCI_List.begin(); DCI_it1 != DCI_List.end(); ++DCI_it1)
				{
					LOG("startTime:%d,endTime%d,AL:%d,NCCE_index:%d,Time_NCCE0:%d,Time_NCCE1:%d\n",(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.Aggregation_L,(*DCI_it1).DCI_Format.DCI_UL_PDU.NCCE_index,T_DCI_Resource[0],T_DCI_Resource[1]);
				}
				// system("pause");
			}
			if(File_LOG_Flag)
			{
				fout_LOG<<"DCI_List(1,2....m)"<<endl;
				for (list<HI_DCI0_request_t>::iterator DCI_it1=DCI_List.begin(); DCI_it1 != DCI_List.end(); ++DCI_it1)
				{
					fout_LOG<<"startTime:"<<(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime<<",endTime:"<<(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime<<",AL:"<<(*DCI_it1).DCI_Format.DCI_UL_PDU.Aggregation_L<<",NCCE_index:"<<(*DCI_it1).DCI_Format.DCI_UL_PDU.NCCE_index<<",Time_NCCE0:"<<T_DCI_Resource[0]<<",Time_NCCE1:"<<T_DCI_Resource[1]<<endl;
				}
			}
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
			// i is the number of scheduling delay elements, which can be a constant
			for (int i = 0; i < sizeof(shcedulingdelay)/sizeof(uint32_t); ++i)
			{
				// size of the pattern for all ce levels
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
			   	if(File_LOG_Flag)
				{
					fout_LOG<<"Can't find available UL resource skip this UE scheduling in this pp..."<<"CE_Level:"<<(*it1).CE_Level<<" UE_id "<<(*it1).UE_id<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<" first_Arrival_Time:"<<" sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<" UL_Buffer_Size"<<(*it1).UL_Buffer_Size<<endl;
                }
			   	if(LOG_Flag)
				{
					LOG("Can't find available UL resource skip this UE scheduling in this pp...\n");
                	system("pause");
                }
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
			// TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
			// LOG("mcs:%d,mapped_mcs:%d,TBS:%d\n",mcs,mapped_mcs[(*it1).CE_Level][mappedMcsIndex],TBS);
            // if((*it1).schedMsg3)
            // {
            //     mcs=2;//Imcs=010;,pi/4 QPSK,Nru=1,TBS=88 bits
            //     TBS=88>>3;
            //     ru_index=0;
            // }
            // else
            // {
    			// while((5<mcs)||((TBS>(*it1).UL_Buffer_Size&&mcs>=1)))
				// while((mapped_mcs[(*it1).CE_Level][mappedMcsIndex]<mcs)||((TBS>(*it1).UL_Buffer_Size&&mcs>=1)))
    // 			{
    // 				// LOG("XX");
    // 				// system("pause");
    // 				--mcs;
    // 				TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
    // 				// LOG("mcs:%d,TBS:%d\n",mcs,TBS);
    // 			}
				// mcs=mapped_mcs[(*it1).CE_Level][mappedMcsIndex];
				mcs=mapped_mcs[(*it1).PHR];
				TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
				// mcs=6;
				// TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
    			while((TBS<(*it1).UL_Buffer_Size)&&(ru_index<=7))
    			{
    				// LOG("YY");
    				// system("pause");
    				ru_index++;
    				TBS=get_TBS_UL(mcs,(*it1).multi_tone,ru_index,max_Iru);
    				if(max_Iru)	break;
    			}
            // }
			(*it1).allocate_Buffer_Size=TBS;

			// fout_LOG<<"[Finish Step1~3]"<<"UE_Id:"<<(*it1).UE_id<<endl;
			//Step 4: Pre UL resource mapping
			uint32_t Nrep[3]={SIB2_NB_S.rep[0],SIB2_NB_S.rep[1],SIB2_NB_S.rep[2]};
            uint32_t Nulslots=num_ULslots(Isc);
            //1 UL slots=0.5ms
			N=RU_table[ru_index] * Nrep[(*it1).CE_Level] * Nulslots * 0.5;
			// (*it1).Resource=N;
			//If ReTx, delete previous allocated UL data resource
            // if((*it1).CRC_indication)	Sum_Occupied_resource__U=Sum_Occupied_resource__U-N;
            if((*it1).CRC_indication==0)	Sum_Occupied_resource__U=Sum_Occupied_resource__U+N;
            else if((*it1).CRC_indication==1)	Sum_Occupied_resource__U=Sum_Occupied_resource__U-N;
            //How many reosurce occupied in this UE(single-tone)
            // Sum_Occupied_resource__U=Sum_Occupied_resource__U+N;

            //TS. 36.211 resource mapping
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
					N--; // cause discontinue allocation
				}
				// ++UL_ChannelTime;
			}
			// fout_LOG<<"[Finish Step1~4]"<<"UE_Id:"<<(*it1).UE_id<<endl;
			//Sched UL transmission will end in UL_ChannelTime-->record to (*it1).next_Arrival_Time.(BSR)
			(*it1).next_Arrival_Time=UL_ChannelTime;
			N=RU_table[ru_index] * Nrep[(*it1).CE_Level] * Nulslots * 0.5;
		   	if(LOG_Flag)
			{
				LOG("CE_Level:%d,UE_id:%d,FirstMsg3ArrivalTime:%d,ACKforMsg4Time:%d,DCI_StartTime/EndTime:%d/%d,ScheDelay:%d,N:%d,Sched UL transmission startTime/EndTime:%d/%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).first_Arrival_Time,(*it1).sche_Msg5_Time,(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime,(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime,ScheDelay,N,UL_Channel[Isc],(*it1).next_Arrival_Time);
				LOG("\n");
				LOG("CE_Level:%d,UE_id:%d,CRC_indication:%d,round:%d,multi_tone:%d,PHR:%d,MSC:%d,DV:%d,BSR:%d,UL_Buffer_Size:%d,TBS:%d,Nru:%d,Nrep:%d,Nulslots:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).CRC_indication,(*it1).round,(*it1).multi_tone,(*it1).PHR,mcs,(*it1).DV,(*it1).BSR,(*it1).UL_Buffer_Size,TBS,RU_table[ru_index],Nrep[(*it1).CE_Level],Nulslots);
				LOG("\n");
				// system("pause");
			}
		   	if(File_LOG_Flag)
			{
				fout_LOG<<"CE_Level:"<<(*it1).CE_Level<<" UE_id:"<<(*it1).UE_id<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<" FirstMsg3ArrivalTime:"<<(*it1).first_Arrival_Time<<" ACKforMsg4Time:"<<(*it1).sche_Msg5_Time<<" DCI_StartTime/EndTime:"<<(*DCI_it1).DCI_Format.DCI_UL_PDU.startTime<<"/"<<(*DCI_it1).DCI_Format.DCI_UL_PDU.endTime<<" Sched UL transmission startTime/EndTime:"<<UL_Channel[Isc]<<"/"<<(*it1).next_Arrival_Time<<"PHR:"<<(*it1).PHR<<" UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<" TBS:"<<TBS<<endl;
 				fout_LOG<<"Sum_Occupied_resource__U:"<<Sum_Occupied_resource__U<<",N:"<<N<<",Nru:"<<RU_table[ru_index]<<",Nulslots:"<<Nulslots<<",Nrep:"<<Nrep[(*it1).CE_Level]<<endl;
 			}
			// UL_Channel[j] record last time of the Isc
			UL_Channel[Isc]=UL_ChannelTime;

			//Step 5: Fill above DCI filed(Isc,Idelay,msc,Iru) and flag/NDI/RV/DCI_Rep/Irep
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.type=1;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.Scheddly=ScheDelay;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.scind=Isc;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ResAssign=ru_index;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.mcs=mcs;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.RepNum=Nrep[(*it1).CE_Level];
			//The rv determination base on the formula defined in TS 36.213
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.rv=0;
			(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.DCIRep=get_DCI_Filed(T_SearchSpace,DCI_Rep[(*it1).CE_Level]);

			if((*it1).round>1)	(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi=(*it1).oldNDI_UL;
			else	(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi=1-(*it1).oldNDI_UL;
			(*it1).oldNDI_UL=(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi;
		   	if(LOG_Flag)
			{
				LOG("DCI format N0:\n\tflag:%d\n\tIdelay:%d\n\tIsc:%d\n\tIru:%d\n\tmcs:%d\n\tIrep:%d\n\tRV:%d\n\tNDI:%d\n\tDCIRep:%d\n",(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.type,ScheDelay,Isc,ru_index,mcs,Nrep[(*it1).CE_Level],(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.rv,(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi,(*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.DCIRep);
				// system("pause");
				LOG("\n");
			}
		   	if(File_LOG_Flag)
			{
				fout_LOG<<"DCI format N0:"<<"\n\tflag:"<<unsigned((*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.type)<<"\n\tIdelay:"<<ScheDelay<<"\n\tIsc:"<<Isc<<"\n\tIru:"<<ru_index<<"\n\tmcs:"<<mcs<<"\n\tIrep:"<<Nrep[(*it1).CE_Level]<<"\n\tRV:"<<unsigned((*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.rv)<<"\n\tNDI:"<<unsigned((*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.ndi)<<"\n\tDCIRep:"<<unsigned((*DCI_it1).DCI_Format.DCI_UL_PDU.DCIN0.DCIRep)<<endl;
			}
			//adjust total UL buffer status by TBS, wait for UL sdus to do final update
			// if((*it1).schedMsg3==false)
			if((*it1).configured==true)
			{
				if(((*it1).allocate_Buffer_Size >= (*it1).UL_Buffer_Size))//(*it1).allocate_Buffer_Size=TBS;
				{
					// (*it1).UL_Buffer_Size=0;
	                if(((*it1).next_Arrival_Time-(*it1).first_Arrival_Time)>10000)//delay buget:10s
	                {
	                	 fout_LOG<<"[TBS>=UL_Buffer_Size],exceed delay budget.."<<"CE_Level:"<<(*it1).CE_Level<<" UE_id "<<(*it1).UE_id<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<" first_Arrival_Time:"<<(*it1).first_Arrival_Time<<" sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<" EndTime:"<<(*it1).next_Arrival_Time<<" UL_Buffer_Size"<<(*it1).UL_Buffer_Size<<endl;
	                    // LOG("[Finish transmission exceed delay] CE_Level:%d,UE_id:%d,first_Arrival_Time:%d,EndTime:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).first_Arrival_Time,(*it1).next_Arrival_Time);
	                    // system("pause");
	                	 /*Delete fail device*/
	 	                it1 = UE_Info_List.erase (it1);
						--it1;
	                }
	                // Sum_Delay=Sum_Delay+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
	                Sum_Delay[(*it1).CE_Level]=Sum_Delay[(*it1).CE_Level]+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
	                End_Time=(*it1).next_Arrival_Time;

	                P_Sum_Delay[(*it1).I_payloadSize][(*it1).CE_Level]=P_Sum_Delay[(*it1).I_payloadSize][(*it1).CE_Level]+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
	                P_CE_NumUE[(*it1).I_payloadSize][(*it1).CE_Level]++;

	    		   	if(LOG_Flag)
					{
						LOG("[Delete] CE_Level:%d,UE_id:%d,UL_Buffer_Size:%d,TBS:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).allocate_Buffer_Size);
	                	LOG("End_Time:%d,Sum_Delay[%d]:%d\n",End_Time,(*it1).CE_Level,Sum_Delay[(*it1).CE_Level]);
	                	// system("pause");
					}
					if(File_LOG_Flag)
					{
						fout_LOG<<"[Delete] CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<",TBS:"<<(*it1).allocate_Buffer_Size<<endl;
						fout_LOG<<"End_Time:"<<End_Time<<",Sum_Delay:"<<Sum_Delay[(*it1).CE_Level]<<endl;
	                	// system("pause");
					}
	                it1 = UE_Info_List.erase (it1);
					--it1;
				}
				else
				{
	                // (*it1).remaining_Buffer_Sizes=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
	    		   	if(LOG_Flag)
					{
	                	LOG("[Reset] UE_id:%d,UL_Buffer_Size:%d, remaining_Buffer_Sizes:%d,BSRNextArrival:%d Update UL_Buffer_Size when receive DV or BSR\n",(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size,(*it1).next_Arrival_Time);
	                	// system("pause");
					}
					if(File_LOG_Flag)
					{
	                	fout_LOG<<"[Reset] UE_id:"<<(*it1).UE_id<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<", remaining_Buffer_Sizes:"<<(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size<<",BSRNextArrival:"<<(*it1).next_Arrival_Time<<endl;
					}
					(*it1).UL_Buffer_Size=0;
					// (*it1).allocate_Buffer_Size=0;
	                // (*it1).UL_Buffer_Size=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
				}
			}
		}//loop for UE List

        //Update traffic model here
		// LOG("Sched UEs done in UE_List: UL_Buffer_Size update when receive DV/BSR\n");
		// for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
		// {
		// 	if(((*it1).schedStatus==true)&&((*it1).schedMsg3==false))//ScheduleDone this pp, reset UL Buffer size, it only update when receive DV and BSR
		// 	{
		// 		// (*it1).remaining_Buffer_Sizes=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
		// 		// if((*it1).remaining_Buffer_Sizes<=0), Bug: can't judge nagatove value..
		// 		if(((*it1).allocate_Buffer_Size >= (*it1).UL_Buffer_Size))
		// 		{
		// 			// (*it1).UL_Buffer_Size=0;
  //                   if(((*it1).next_Arrival_Time-(*it1).first_Arrival_Time)>10000)//delay buget:10s
  //                   {
  //                   	 // fout_LOG<<"[TBS>=UL_Buffer_Size],exceed delay budget.."<<"CE_Level:"<<(*it1).CE_Level<<" UE_id "<<(*it1).UE_id<<" schedMsg3:"<<(*it1).schedMsg3<<" CRC:"<<(*it1).CRC_indication<<" round:"<<(*it1).round<<" first_Arrival_Time:"<<" sche_Msg5_Time:"<<(*it1).sche_Msg5_Time<<" EndTime:"<<(*it1).next_Arrival_Time<<" UL_Buffer_Size"<<(*it1).UL_Buffer_Size<<endl;
  //                       LOG("[TBS>=UL_Buffer_Size] CE_Level:%d,UE_id:%d,first_Arrival_Time:%d,EndTime:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).first_Arrival_Time,(*it1).next_Arrival_Time);
  //                       system("pause");
  //                   }
  //                   // Sum_Delay=Sum_Delay+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
  //                   Sum_Delay[(*it1).CE_Level]=Sum_Delay[(*it1).CE_Level]+(*it1).next_Arrival_Time-(*it1).first_Arrival_Time;
  //                   End_Time=(*it1).next_Arrival_Time;
  //       		   	if(LOG_Flag)
		// 			{
		// 				LOG("[Delete] CE_Level:%d,UE_id:%d,UL_Buffer_Size:%d,TBS:%d\n",(*it1).CE_Level,(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).allocate_Buffer_Size);
  //                   	LOG("End_Time:%d,Sum_Delay[%d]:%d\n",End_Time,(*it1).CE_Level,Sum_Delay[(*it1).CE_Level]);
  //                   	// system("pause");
		// 			}
		// 			if(File_LOG_Flag)
		// 			{
		// 				fout_LOG<<"[Delete] CE_Level:"<<(*it1).CE_Level<<",UE_id:"<<(*it1).UE_id<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<",TBS:"<<(*it1).allocate_Buffer_Size<<endl;
		// 				fout_LOG<<"End_Time:"<<End_Time<<",Sum_Delay:"<<Sum_Delay[(*it1).CE_Level]<<endl;
  //                   	// system("pause");
		// 			}
  //                   it1 = UE_Info_List.erase (it1);
		// 			--it1;
		// 		}
		// 		else
		// 		{
  //                   // (*it1).remaining_Buffer_Sizes=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
  //       		   	if(LOG_Flag)
		// 			{
  //                   	LOG("[Reset] UE_id:%d,UL_Buffer_Size:%d, remaining_Buffer_Sizes:%d,BSRNextArrival:%d Update UL_Buffer_Size when receive DV or BSR\n",(*it1).UE_id,(*it1).UL_Buffer_Size,(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size,(*it1).next_Arrival_Time);
  //                   	// system("pause");
		// 			}
		// 			if(File_LOG_Flag)
		// 			{
  //                   	fout_LOG<<"[Reset] UE_id:"<<(*it1).UE_id<<",UL_Buffer_Size:"<<(*it1).UL_Buffer_Size<<", remaining_Buffer_Sizes:"<<(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size<<",BSRNextArrival:"<<(*it1).next_Arrival_Time<<endl;
		// 			}
		// 			(*it1).UL_Buffer_Size=0;
		// 			// (*it1).allocate_Buffer_Size=0;
  //                   // (*it1).UL_Buffer_Size=(*it1).UL_Buffer_Size-(*it1).allocate_Buffer_Size;
		// 		}
		// 	}
		// }

	   	if(LOG_Flag)
		{
			LOG("UE_Info_List size:%d\n",UE_Info_List.size());
		}
        for (int i = 0; i < 3; ++i)
        {
            list<UE_TEMPLATE_NB> & check_UE_Info_List=UL_Indicaiton.UL_UE_Info_List[i];
            // typename list<UE_TEMPLATE_NB>::iterator it1;// distinguish Template and type
            if(check_UE_Info_List.empty ())
            {
               if(EndPoint!=3)  ++EndPoint;
            }
        }
        if(highOfferedLoad==0)
        {
		   	if(LOG_Flag)
			{
            	LOG("EndPoint:%d,TotalNumUE[runCase]:%d,totalNumUE:%d\n",EndPoint,TotalNumUE[runCase],totalNumUE);
			}
		   	if(File_LOG_Flag)
			{
            	fout_LOG<<"EndPoint: "<<EndPoint<<" totalNumUE:"<<totalNumUE<<endl;
			}
            if((EndPoint!=3)||((TotalNumUE[runCase])!=totalNumUE))  EndPoint=0;
        }
        else if(highOfferedLoad==1)
        {
		   	if(LOG_Flag)
			{
            	LOG("EndPoint:%d,TotalNumUE[runCase]:%d,totalNumUE:%d\n",EndPoint,TotalNumUE_H[runCase],totalNumUE);
            }
		   	if(File_LOG_Flag)
			{
            	fout_LOG<<"EndPoint: "<<EndPoint<<" totalNumUE:"<<totalNumUE<<endl;
			}
            if((EndPoint!=3)||(TotalNumUE_H[runCase]!=totalNumUE))  EndPoint=0;
  	    }
	   	if(File_LOG_Flag)
		{
  	    	fout_LOG<<"EndPoint: "<<EndPoint<<" totalNumUE:"<<totalNumUE<<endl;
		}
		// if(EndPoint==3)	Sum_Occupied_resource__U=Sum_Occupied_resource__U+N;
    }
}
