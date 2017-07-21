#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>

#include "rrc_NB.h"
#include "sche_pro_NB.h"
#include "interface_NB.h"
#include "type_NB.h"

#define LOG(...) printf(__VA_ARGS__)

using namespace std;

// extern uint32_t NPDCCH_period;
extern uint32_t CSS_NPDCCH_period[3];
extern uint32_t USS_NPDCCH_period[3];

// uint32_t n_pp=0;//number of  NPDCCH_period
// uint32_t **UL_Channel_bitmap;
// uint32_t * element;
// channel_t * DL_Channel_bitmap;

// uint32_t *DL_Channel_bitmap;
// channel_t **UL_Channel_bitmap;
// int num_DL_Subcarrier=12;
// int num_UL_Subcarrier=48;


// typedef struct UL_IND
// {
// 	list<UE_TEMPLATE_NB> UL_UE_Info_List;
// 	//ACK/NACK
// 	//preamble_List
// }UL_IND_t;


UL_IND_t UL_Indicaiton;
Sche_RES_t Sche_Response;

// list<UE_TEMPLATE_NB> UL_UE_Info_List


uint32_t MIB_period=640;
#define SI_entry 2//#1:SIB1-NB, #2:SIB23-NB
uint32_t Cell_id=0;//Fixed base on NPSS/NSSS Calculation
uint32_t H_SFN=0;//Hyper super frame = 1024 frame


//Argument of NB_eNB_dlsch_ulsch_scheduler should be:
//module_id_t module_idP,uint8_t cooperation_flag, frame_t frameP, sub_frame_t subframeP
void PossibleSearchspace(SIB2_NB &,vector<vector<uint32_t> > & );
uint32_t Get_pattern_base(SIB2_NB & ,vector<int> & );
vector<uint32_t> Searchspace(8,0);
//locationS: Possible DCI location in search space in pp
vector<vector<uint32_t> > locationS(3,Searchspace);
vector<uint32_t> Sfreq;
vector<uint32_t> UL_Channel;
// uint8_t CSS_flag=0;

int simTimes=0;
/* After simTimes, Sum total # of devices and delay for each CE level */
extern uint32_t Sum_Delay[3];
uint32_t CEi_NumUE[3]={0,0,0};
uint32_t Sum_End_Time=0;
uint32_t Sum_nprach_resource_U=0;
/*Parameters should be initilize before each simulation time...*/
int End_Time=0;
extern int EndPoint;
extern uint32_t totalNumUE;
extern bool simCtrl;
extern uint32_t Sum_Occupied_resource__U;
extern int UE_id;

extern uint32_t P_Sum_Delay[2][3];
extern uint32_t P_CE_NumUE[2][3];
double P_delay[6]={0.0};

//1: pp are same for 3 CE levels
//2: pp are different for 3 CE levels
//3: pp are different with different payload size (low offered load) 20~200
# define simCase 3
//0:low offered load-->6.. report/sec
//1:high offered load-->12 or 18.. report/sec
uint8_t highOfferedLoad=0;

//Simulation setting
uint8_t TotalNumUE[10]={30,12,18,24,30,36,42,48,54,60};
//{24,48,72,96,120,144,168,192,216,240};
//{12,24,36,48,60,72,84,96,108,120};
//{20,40,60,80,100,120,140,160,180,200};
uint32_t TotalNumUE_H[10]={50,20,120,80,100,120,140,160,180,200};
uint8_t runCase;//0,1,2,3,4,5,6,7,8,9
// extern int Sum_Delay;


//Evaluate BSR map to different range of BSR table in eNB.
//Case 1
ofstream resourceUtilization_pp_same,resourceUtilization_pp_same_H;
ofstream AverageDelay_pp_sameCE0,AverageDelay_pp_sameCE1,AverageDelay_pp_sameCE2;
ofstream AverageDelay_pp_sameCE0_H,AverageDelay_pp_sameCE1_H,AverageDelay_pp_sameCE2_H;
//Case 2
ofstream resourceUtilization_pp_not_same,resourceUtilization_pp_not_same_H;
ofstream AverageDelay_pp_not_sameCE0,AverageDelay_pp_not_sameCE1,AverageDelay_pp_not_sameCE2;
ofstream AverageDelay_pp_not_sameCE0_H,AverageDelay_pp_not_sameCE1_H,AverageDelay_pp_not_sameCE2_H;
//Case 3 for Evaluate different payload size
uint32_t simDataSize=0;
// uint32_t DataSize[10]={220,240,260,280,300,320,340,360,380,400};
uint32_t DataSize[10]={20,40,60,80,100,120,140,160,180,200};
// uint32_t DataSize[16]={200,220,240,260,280,300,320,340,359,380,400,420,440,460,480,500};
uint32_t indexDataSize=0;
//0:highest, 1:medium, 2:lowest
uint8_t mappingBufferSize=0;
uint8_t File_LOG_Flag=0;
uint8_t LOG_Flag=0;


ofstream AverageDelay_payloadSizeCE0,AverageDelay_payloadSizeCE1,AverageDelay_payloadSizeCE2,resourceUtilization__payloadSize;

ofstream Low_High_Payload_AverageDelay;


ofstream fout_LOG;

// //Case 3
// ofstream resourceUtilization_pp_same1,resourceUtilization_pp_same_H1;
// ofstream AverageDelay_pp_sameCE0_1,AverageDelay_pp_sameCE1_1,AverageDelay_pp_sameCE2_1;
// ofstream AverageDelay_pp_sameCE0_H1,AverageDelay_pp_sameCE1_H1,AverageDelay_pp_sameCE2_H1;
// //Case 4
// ofstream resourceUtilization_pp_not_same1,resourceUtilization_pp_not_same_H1;
// ofstream AverageDelay_pp_not_sameCE0_1,AverageDelay_pp_not_sameCE1_1,AverageDelay_pp_not_sameCE2_1;
// ofstream AverageDelay_pp_not_sameCE0_H1,AverageDelay_pp_not_sameCE1_H1,AverageDelay_pp_not_sameCE2_H1;

//Evaluation Performance
double T_AvailResource[10]={0};
double T_OccupiedResource[10]={0};
double T_Average_Delay[3][10]={0};

int main(int argc, char const *argv[])//design simulation base on different argv/argc form bat...
{
	clock_t t;
	t = clock();
	srand(time(NULL));
	if(argc!=2)
	{
		cout << "Usage: ./[exe] [runCase]" << endl;
		printf("Non Valid Argument Parameters\n");
		system("pause");
		return 0;
		// printf("%s\n", *argv);
		// printf("%s\n", *(argv+1));
    	// perror("perror");
    	// printf("strerror(errno)=%s\n", strerror(errno));
	}
	runCase = atoi(argv[1]);//Convert string to integer,int atoi (const char * str);
	LOG("runCase:%d\n",runCase);
	system("pause");

	fout_LOG.open("LOG.txt", ios::out);
	if(!fout_LOG.is_open())
	{
	    cout << "Error: the LOG file is not opened!!" << endl;
	    exit(1);
	}

	// oneThird_singleTone.open("Matlab_Result\\oneThird_singleTone.csv", ios::out);
	if(simCase==1)
	{
		if(highOfferedLoad==0)
		{
			// AverageDelay_pp_same.open("Matlab_Result\\AverageDelay_pp_same.csv", ios::app);
			// resourceUtilization_pp_same.open("Matlab_Result\\resourceUtilization_pp_same.csv", ios::app);

			resourceUtilization_pp_same.open("Matlab_Result\\resourceUtilization_pp_same.csv", ios::app);
			AverageDelay_pp_sameCE0.open("Matlab_Result\\AverageDelay_pp_sameCE0.csv", ios::app);
			AverageDelay_pp_sameCE1.open("Matlab_Result\\AverageDelay_pp_sameCE1.csv", ios::app);
			AverageDelay_pp_sameCE2.open("Matlab_Result\\AverageDelay_pp_sameCE2.csv", ios::app);
			// resourceUtilization_pp_same_H.open("Matlab_Result\\resourceUtilization_pp_same_H.csv", ios::app);
			// AverageDelay_pp_sameCE0_H.open("Matlab_Result\\AverageDelay_pp_sameCE0_H.csv", ios::app);
			// AverageDelay_pp_sameCE1_H.open("Matlab_Result\\AverageDelay_pp_sameCE1_H.csv", ios::app);
			// AverageDelay_pp_sameCE2_H.open("Matlab_Result\\AverageDelay_pp_sameCE2_H.csv", ios::app);
		}
		else if(highOfferedLoad==1)
		{
			resourceUtilization_pp_same_H.open("Matlab_Result\\resourceUtilization_pp_same_H.csv", ios::app);
			AverageDelay_pp_sameCE0_H.open("Matlab_Result\\AverageDelay_pp_sameCE0_H.csv", ios::app);
			AverageDelay_pp_sameCE1_H.open("Matlab_Result\\AverageDelay_pp_sameCE1_H.csv", ios::app);
			AverageDelay_pp_sameCE2_H.open("Matlab_Result\\AverageDelay_pp_sameCE2_H.csv", ios::app);
		}
	}
	else if(simCase==2)
	{
		if(highOfferedLoad==0)
		{
			// AverageDelay_pp_not_same.open("Matlab_Result\\AverageDelay_pp_not_same.csv", ios::app);
			// resourceUtilization_pp_not_same.open("Matlab_Result\\resourceUtilization_pp_not_same.csv", ios::app);

			resourceUtilization_pp_not_same.open("Matlab_Result\\resourceUtilization_pp_not_same.csv", ios::app);
			AverageDelay_pp_not_sameCE0.open("Matlab_Result\\AverageDelay_pp_not_sameCE0.csv", ios::app);
			AverageDelay_pp_not_sameCE1.open("Matlab_Result\\AverageDelay_pp_not_sameCE1.csv", ios::app);
			AverageDelay_pp_not_sameCE2.open("Matlab_Result\\AverageDelay_pp_not_sameCE2.csv", ios::app);
			// resourceUtilization_pp_not_same_H.open("Matlab_Result\\AverageDelay_pp_not_same.csv", ios::app);
			// AverageDelay_pp_not_sameCE0_H.open("Matlab_Result\\AverageDelay_pp_not_same.csv", ios::app);
			// AverageDelay_pp_not_sameCE1_H.open("Matlab_Result\\AverageDelay_pp_not_same.csv", ios::app);
			// AverageDelay_pp_not_sameCE2_H.open("Matlab_Result\\AverageDelay_pp_not_same.csv", ios::app);
		}
		else if(highOfferedLoad==1)
		{
			resourceUtilization_pp_not_same_H.open("Matlab_Result\\resourceUtilization_pp_not_same_H.csv", ios::app);
			AverageDelay_pp_not_sameCE0_H.open("Matlab_Result\\AverageDelay_pp_not_sameCE0_H.csv", ios::app);
			AverageDelay_pp_not_sameCE1_H.open("Matlab_Result\\AverageDelay_pp_not_sameCE1_H.csv", ios::app);
			AverageDelay_pp_not_sameCE2_H.open("Matlab_Result\\AverageDelay_pp_not_sameCE2_H.csv", ios::app);
		}
	}
	else if(simCase==3)
	{
		resourceUtilization__payloadSize.open("Matlab_Result\\resourceUtili_payloadSize.csv", ios::app);
		AverageDelay_payloadSizeCE0.open("Matlab_Result\\AverageDelay_payloadSizeCE0.csv", ios::app);
		AverageDelay_payloadSizeCE1.open("Matlab_Result\\AverageDelay_payloadSizeCE1.csv", ios::app);
		AverageDelay_payloadSizeCE2.open("Matlab_Result\\AverageDelay_payloadSizeCE2.csv", ios::app);

		Low_High_Payload_AverageDelay.open("Matlab_Result\\Low_High_Payload_AverageDelay.csv", ios::app);


	}



	MIB_NB	MIB_NB_S= {0};//Directly Initialize
	SIB1_NB	SIB1_NB_S= {0};
	SIB2_NB SIB2_NB_S= {0};
	RRCCoonectionSetup_NB Msg4_S={0};
	if(NB_eNB_Init_RRC(&MIB_NB_S, &SIB1_NB_S, &SIB2_NB_S,&Msg4_S))	LOG("Initialize RRC done\n");
	printSIs(&MIB_NB_S,&SIB1_NB_S,&SIB2_NB_S,&Msg4_S);
	// LOG("CE 0 NPDCCH_period:%d\n",NPDCCH_period);
	LOG("CE 0 CSS NPDCCH_period:%d\n",CSS_NPDCCH_period[0]);
	LOG("CE 1 CSS NPDCCH_period:%d\n",CSS_NPDCCH_period[1]);
	LOG("CE 2 CSS NPDCCH_period:%d\n",CSS_NPDCCH_period[2]);
	LOG("CE 0 USS NPDCCH_period:%d\n",USS_NPDCCH_period[0]);
	LOG("CE 1 USS NPDCCH_period:%d\n",USS_NPDCCH_period[1]);
	LOG("CE 2 USS NPDCCH_period:%d\n",USS_NPDCCH_period[2]);
	system("pause");
	// LOG("%d",channels_length);

	PossibleSearchspace(SIB2_NB_S,locationS);
	//Get pattern-base base on changed NPRACH configuration and multi-tone option
	// Pattern_base Pattern_base_S={0};
	// Get_pattern_base(SIB2_NB_S,Sfreq);
	// LOG("Pattern-base, Sfreq[%d]: ",Sfreq.size());
	// for (vector<int>::iterator it = Sfreq.begin() ; it != Sfreq.end(); ++it)
	// {
	// 	LOG("%d ",*it);
	// }
	// LOG("\n");
	//All UE support single-tone
	uint8_t numTone=12;
	Sfreq.assign (numTone,0);//Initialize to 0
	UL_Channel.assign (numTone,0);
	LOG("Pattern Sfreq: ");
	for (int i = 0; i < numTone; ++i)
	{
		Sfreq[i]=4*i;//{0,4,8,12,16,20,24,28,32,36,40,44}
		LOG("%d " ,Sfreq[i]);
	}
	LOG("\n");
	system("pause");


	uint32_t SI_period[SI_entry]={2560,SIB1_NB_S.si_Periodicity};

	sub_frame_t subframes=0;
	frame_t frame=0;

	//New architecture delete UL/DL virtual channel structure
	/*Initialization first UL/DL virtual channel structure before SF 0*/
	//	calloc initialize to 0 --> channel_t.NA
	// UL_Channel_bitmap= (uint32_t **)calloc(num_UL_Subcarrier,sizeof(uint32_t *));
	// // UL_Channel_bitmap= (uint32_t **)malloc(num_UL_Subcarrier * sizeof(uint32_t *));
	// element= (uint32_t *)calloc(num_UL_Subcarrier * NPDCCH_period,sizeof(uint32_t));
	// // element= (uint32_t *)malloc(num_UL_Subcarrier * NPDCCH_period * sizeof(uint32_t));
	// int i;
	// for (i=0; i<num_UL_Subcarrier; i++, element+=NPDCCH_period)
	// 	UL_Channel_bitmap[i]=element;
	// DL_Channel_bitmap=(uint32_t *)calloc(NPDCCH_period,sizeof(uint32_t));
	// DL_Channel_bitmap=(uint32_t *)malloc(NPDCCH_period * sizeof(uint32_t));

	//Initial schedule for MIB/SIB1/23 before H_SFN:0,SFN:0,SF:0 in RUN_Status
	// LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule next pp(%d~%d) for UL/DL at previous SF of each p\n",H_SFN,frame,subframes,(frame*10+subframes),(frame*10+subframes+NPDCCH_period));
	// system("pause");
	// reserve_schedule_MIB(frame,subframes,NPDCCH_period,DL_Channel_bitmap,true);//NPBCH
	// reserve_schedule_SI(frame,subframes,NPDCCH_period,DL_Channel_bitmap,&MIB_NB_S, &SIB1_NB_S,true);
	//NPDCCH Period base scheudling for MIB/SIB1/23 and UL;  RA and DL not done.
	//Start to schedule
	while(1)
	{
		// generate frame and subframe for the simulation
		// generate MAC/PHY frame and subframe for tx(+1) and rx (-1)
		// calcualte the ctime
		// all of the above is the same for all the procedures
		// check which procedure has to be done now: i.e. ra, si, mib, ulsch,  etc

		if(H_SFN * 10240+frame * 10+subframes==0)
		{
			LOG("Start Simulation:%d\n",simTimes);
			// fout_LOG<<"Start Simulation:"<<simTimes<<endl;
			// system("pause");
		}
		Ulsch_ind(frame,subframes,UL_Indicaiton);
		// LOG("Finish Ulsch_ind\n");
		// system("pause");
		//New architecture delete UL/DL virtual channel structure
		//Build UL/DL virtual channel structure at the start of previous SF of each pp.
		// if(((H_SFN * 10240+frame * 10+subframes+1)%NPDCCH_period)==0)
		// {
		// 	LOG("H_SFN:%d,frame:%d,subframes:%d,Build next pp(%d~%d)struc for UL/DL at previous SF of each pp\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+NPDCCH_period));
		// 	// system("pause");
		// 	n_pp++;
		// 	//	calloc initialize to 0 -> channel_t.NA
		// 	UL_Channel_bitmap= (uint32_t **)calloc(num_UL_Subcarrier,sizeof(uint32_t *));
		// 	// // UL_Channel_bitmap= (uint32_t **)malloc(num_UL_Subcarrier * sizeof(uint32_t *));
		// 	element= (uint32_t *)calloc(num_UL_Subcarrier * NPDCCH_period,sizeof(uint32_t));
		// 	// // element= (uint32_t *)malloc(num_UL_Subcarrier * NPDCCH_period * sizeof(uint32_t));
		// 	int i;
		// 	for (i=0; i<num_UL_Subcarrier; i++, element+=NPDCCH_period)
		// 		UL_Channel_bitmap[i]=element;
		// 	DL_Channel_bitmap=(uint32_t *)calloc(NPDCCH_period,sizeof(uint32_t));
		// 	// DL_Channel_bitmap=(uint32_t *)malloc(NPDCCH_period * sizeof(uint32_t));
		// 	// LOG("frame:%d,subframes:%d\n",frame,subframes);
		// 	// Check if UL_data/Msg3/Msg5/Other UL RRC Msg3 arrival before schedule ulsch/dlsch
		// }

		/*Start NB_eNB_dlsch_ulsch_scheduler*/
		//Schedule MIB base on period.
		if((H_SFN * 10240+frame * 10+subframes+1)%MIB_period==0)
		{
			// NB_schedule_MIB(frame,subframes,NPDCCH_period,DL_Channel_bitmap,false);//NPBCH
		}
		//Schedule SIs(SIB1) base on period.
		if(((H_SFN * 10240+frame * 10+subframes+1)%SI_period[0])==0)
		{
			// NB_schedule_SI(frame,subframes,NPDCCH_period,DL_Channel_bitmap,&MIB_NB_S, &SIB1_NB_S,false);
		}
		//Schedule SIs(SIB23) base on radio-frame offset and repetition pattern within SI window.
		for (int i = 1; i < SI_entry; ++i)
		{
			if((H_SFN * 10240+frame * 10+subframes+1)%SI_period[i]==0)
			{
				// NB_schedule_SI(frame,subframes,NPDCCH_period,DL_Channel_bitmap,&MIB_NB_S, &SIB1_NB_S,false);
			}
		}
		//Schedule RA/Paging at the previous SF of each common pp for three CE levels.
		for (int i = 0; i < 3; ++i)
		{
			if(((H_SFN * 10240+frame * 10+subframes+1)%CSS_NPDCCH_period[i])==0)
			{
				uint32_t scheH_SFN=0,scheSubframe=0,scheFrame=0;
				scheH_SFN=H_SFN;
				scheFrame=frame;
				scheSubframe=subframes+1;
				if(scheSubframe==10)
				{
					scheSubframe=0;
					++scheFrame;
					if(scheFrame==1024)
					{
						scheFrame=0;
						scheH_SFN++;
					}
				}
            	if(LOG_Flag)
    			{
					LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule DCIs in CSS of next pp(%d~%d) for UL/DL at previous SF of CE %d pp\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+CSS_NPDCCH_period[i]),i);
					// system("pause");
    			}
            	if(File_LOG_Flag)
    			{
					// fout_LOG<<"H_SFN:"<<scheH_SFN<<" frame:"<<scheFrame<<" subframes:"<<scheSubframe<<",Schedule DCIs in CSS of next pp"<<(scheH_SFN * 10240 + scheFrame * 10 + scheSubframe)<<"~"<<"CE_Level:"<<i<<endl;
				}
				// CSS_flag=1;
				// NB_schedule_RA(frame,subframes,DL_Channel_bitmap,UL_Channel_bitmap);
				// NB_schedule_dlsch(frame,subframes,DL_Channel_bitmap);
			}
		}
		for (int i = 2; i >= 0; --i)
		// for (int i = 0; i < 3; ++i)
		{
			if((((H_SFN * 10240+frame * 10+subframes+1)%USS_NPDCCH_period[i])==0)||(H_SFN * 10240+frame * 10+subframes)==0)
			{
				uint32_t scheH_SFN=0,scheSubframe=0,scheFrame=0;
				scheH_SFN=H_SFN;
				scheFrame=frame;
				scheSubframe=subframes+1;
				if(scheSubframe==10)
				{
					scheSubframe=0;
					++scheFrame;
					if(scheFrame==1024)
					{
						scheFrame=0;
						scheH_SFN++;
					}
				}
	           	if(LOG_Flag)
    			{
					LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule DCIs in USS of next pp(%d~%d) for UL/DL at previous SF of CE %d pp\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+USS_NPDCCH_period[i]),i);
					// system("pause");
				}
            	if(File_LOG_Flag)
    			{
					fout_LOG<<"H_SFN:"<<scheH_SFN<<" frame:"<<scheFrame<<" subframes:"<<scheSubframe<<",Schedule DCIs in USS of next pp"<<(scheH_SFN * 10240 + scheFrame * 10 + scheSubframe)<<"~"<<"CE_Level:"<<i<<endl;
				}
				// CSS_flag=0;
				// NB_schedule_RA(frame,subframes,DL_Channel_bitmap,UL_Channel_bitmap);
				NB_schedule_ulsch(scheH_SFN,scheFrame,scheSubframe,i,MIB_NB_S,SIB1_NB_S,SIB2_NB_S,Msg4_S,UL_Indicaiton);
				// NB_schedule_dlsch(frame,subframes,DL_Channel_bitmap);
			}
		}
		/*End NB_eNB_dlsch_ulsch_scheduler*/
		//Send HI_DCI0_request(1,2...m DCIs) message to PHY
		Sche_res(frame,subframes,Sche_Response);
		//New architecture delete UL/DL virtual channel structure
		// if(((H_SFN * 1024+frame * 10+subframes+2)%NPDCCH_period)==0)//The end of previous tow SF of each pp.
		// {
		// 	free(UL_Channel_bitmap[0]);
		// 	free(UL_Channel_bitmap);
		// 	free(DL_Channel_bitmap);
		// 	// free(element); //Don't need free element....Bug
		// 	LOG("Free This NPDCCH Period Memory space!\n");
		// 	system("pause");
		// }
		// LOG("H_SFN:%d,frame:%d,subframes:%d\n",H_SFN,frame,subframes);
		// system("pause");
		//UE_list for three CE levels already empty...and confirm DCI list already clear..
		// if((EndPoint==3)&&((H_SFN * 10240+frame * 10+subframes)==End_Time)) in NB_scheudle_ulsch
		if(EndPoint==3)
		{
			Sum_nprach_resource_U=Sum_nprach_resource_U+get_nprah_resource(End_Time,SIB2_NB_S);
			Sum_End_Time=Sum_End_Time+End_Time;

        	if(LOG_Flag)
			{
				LOG("UE_Info_List for three CE level are empty\n");
				LOG("H_SFN:%d,frame:%d,subframes:%d,End_Time:%d\n",H_SFN,frame,subframes,End_Time);
				LOG("Sum_End_Time:%d,Sum_nprach_resource_U:%d\n",Sum_End_Time,Sum_nprach_resource_U);
				LOG("CE0:%d,Sum_Delay[0]:%d,CE1:%d,Sum_Delay[1]:%d,CE2:%d,Sum_Delay[2]:%d",CEi_NumUE[0],Sum_Delay[0],CEi_NumUE[1],Sum_Delay[1],CEi_NumUE[2],Sum_Delay[2]);
			// system("pause");
			}
        	if(File_LOG_Flag)
			{
				fout_LOG<<"UE_Info_List for three CE level are empty"<<endl<<"H_SFN:"<<H_SFN<<"frame:"<<frame<<"subframes:"<<subframes<<"End_Time:"<<End_Time<<endl<<"Sum_End_Time:"<<Sum_End_Time<<"Sum_nprach_resource_U:"<<Sum_nprach_resource_U<<"CE0_NumUE:"<<CEi_NumUE[0]<<"CE1_NumUE:"<<CEi_NumUE[1]<<"CE2_NumUE:"<<CEi_NumUE[2]<<endl;
			}
			subframes=0;
			frame=0;
			H_SFN=0;
			End_Time=0;
			EndPoint=0;
			totalNumUE=0;
			simCtrl=true;
			UE_id=0;
			UL_Channel.clear();
			UL_Channel.assign (numTone,0);
			simTimes++;
			if(simTimes==10000)
			{
				if(simCase==3)
				{
				    T_Average_Delay[0][runCase]=(double)Sum_Delay[0]/CEi_NumUE[0];
				    T_Average_Delay[1][runCase]=(double)Sum_Delay[1]/CEi_NumUE[1];
				    T_Average_Delay[2][runCase]=(double)Sum_Delay[2]/CEi_NumUE[2];
				    T_AvailResource[runCase]=(double)(Sum_End_Time * 12 - Sum_nprach_resource_U);
					T_OccupiedResource[runCase]=(double)Sum_Occupied_resource__U;
					// double U=(double)(T_OccupiedResource[runCase]/T_AvailResource[runCase]);

					LOG("index:%d,paloadSize:%d\n",indexDataSize,simDataSize);
					LOG("T_Average_Delay[0]:%lf,T_Average_Delay[1]:%lf,T_Average_Delay[2]:%lf,U:%lf\n",T_Average_Delay[0][runCase],T_Average_Delay[1][runCase],T_Average_Delay[2][runCase],T_OccupiedResource[runCase]/T_AvailResource[runCase]);

				    resourceUtilization__payloadSize<<simDataSize<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
				    double S_Average_Delay=(T_Average_Delay[0][runCase]+T_Average_Delay[1][runCase]+T_Average_Delay[2][runCase])/3;

				    uint32_t T_M=CEi_NumUE[0]+CEi_NumUE[1]+CEi_NumUE[2];
				    double S_Throughput=(double)(T_M * simDataSize)/Sum_End_Time;

					AverageDelay_payloadSizeCE0<<simDataSize<<","<<T_Average_Delay[0][runCase]<<","<<S_Average_Delay<<","<<S_Throughput<<endl;
					AverageDelay_payloadSizeCE1<<simDataSize<<","<<T_Average_Delay[1][runCase]<<endl;
					AverageDelay_payloadSizeCE2<<simDataSize<<","<<T_Average_Delay[2][runCase]<<endl;

					// uint32_t P_Sum_Delay[2][3]={0};
					// uint32_t P_CE_NumUE[2][3]={0};
					// P_Sum_Delay[(*it1).I_payloadSize][(*it1).CE_Level]

					P_delay[0]=(double)P_Sum_Delay[0][0]/P_CE_NumUE[0][0];
					P_delay[1]=(double)P_Sum_Delay[1][0]/P_CE_NumUE[1][0];
					P_delay[2]=(double)P_Sum_Delay[0][1]/P_CE_NumUE[0][1];
					P_delay[3]=(double)P_Sum_Delay[1][1]/P_CE_NumUE[1][1];
					P_delay[4]=(double)P_Sum_Delay[0][2]/P_CE_NumUE[0][2];
					P_delay[5]=(double)P_Sum_Delay[1][2]/P_CE_NumUE[1][2];

					// Low_High_Payload_AverageDelay<<P_delay[0]<<","<<P_delay[1]<<","<<P_delay[2]<<","<<P_delay[3]<<","<<P_delay[4]<<","<<P_delay[5]<<endl;
					Low_High_Payload_AverageDelay<<T_Average_Delay[0][runCase]<<","<<T_Average_Delay[1][runCase]<<","<<T_Average_Delay[2][runCase]<<endl;




					// LOG("Sum_End_Time:%d,Sum_nprach_resource_U:%d,Sum_Occupied_resource__U:%d\n",Sum_End_Time,Sum_nprach_resource_U,Sum_Occupied_resource__U);
					// LOG("CE0:%d,Sum_Delay[0]:%d,CE1:%d,Sum_Delay[1]:%d,CE2:%d,Sum_Delay[2]:%d\n",CEi_NumUE[0],Sum_Delay[0],CEi_NumUE[1],Sum_Delay[1],CEi_NumUE[2],Sum_Delay[2]);
    	// 			LOG("T_Average_DelayCE0:%lf,T_Average_DelayCE1:%lf,T_Average_DelayCE2:%lf,T_AvailResource[runCase]:%d,T_OccupiedResource[runCase]:%d,U:%lf\n",T_Average_Delay[0][runCase],T_Average_Delay[1][runCase],T_Average_Delay[2][runCase],T_AvailResource[runCase],T_OccupiedResource[runCase],T_OccupiedResource[runCase]/T_AvailResource[runCase]);

    				fout_LOG<<"OccupiedResource:"<<T_OccupiedResource[runCase]<<"AvailResource:"<<T_AvailResource[runCase]<<endl;
    				fout_LOG<<"U:"<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
					fout_LOG<<"Sum_End_Time:"<<Sum_End_Time<<",Sum_nprach_resource_U:"<<Sum_nprach_resource_U<<",Sum_Occupied_resource__U:"<<Sum_Occupied_resource__U<<endl;


					for (int i = 0; i < 3; ++i)
					{
						Sum_Delay[i]=0;
						CEi_NumUE[i]=0;
					}
					Sum_End_Time=0;
					Sum_nprach_resource_U=0;
					Sum_Occupied_resource__U=0;
					T_Average_Delay[0][runCase]=0;
					T_Average_Delay[1][runCase]=0;
					T_Average_Delay[2][runCase]=0;
					T_AvailResource[runCase]=0;
					T_OccupiedResource[runCase]=0;
					simTimes=0;
					++indexDataSize;
					// ++runCase;
					if(indexDataSize==10)	break;
					// if(runCase==3)	break;
					continue;
				}
			}
			else continue;
		}
		// if((H_SFN * 1024+frame * 10+subframes)%5000==0)
		// {
		// 	LOG("EndPoint:%d\n",EndPoint);
		// 	system("pause");
		// }
		/*subframes_ind in FAPI*/
		++subframes;
		if(subframes==10)
		{
			subframes=0;
			++frame;
		}
		if(frame==1024)
		{
			frame=0;
			H_SFN++;
		}
		// system("pause");
	}
	// system("pause");
	// totalNumUE=TotalNumUE[runCase];
	if(simCase!=3)
	{
	    T_Average_Delay[0][runCase]=Sum_Delay[0]/CEi_NumUE[0];
	    T_Average_Delay[1][runCase]=Sum_Delay[1]/CEi_NumUE[1];
	    T_Average_Delay[2][runCase]=Sum_Delay[2]/CEi_NumUE[2];
	    T_AvailResource[runCase]=(Sum_End_Time * 12) - Sum_nprach_resource_U;
		T_OccupiedResource[runCase]=Sum_Occupied_resource__U;
    }
    // system("pause");
	if(simCase==1)
	{
		if(highOfferedLoad==0)
		{
		 //    AverageDelay_pp_same<<totalNumUE<<","<<T_Average_Delay[runCase]<<endl;
		 //    resourceUtilization_pp_same<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			// resourceUtilization_pp_same.close();
			// AverageDelay_pp_same.close();
			totalNumUE=TotalNumUE[runCase];
		    resourceUtilization_pp_same<<totalNumUE<<","<<(double)T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_same.close();

			AverageDelay_pp_sameCE0<<totalNumUE<<","<<T_Average_Delay[0][runCase]<<endl;
			AverageDelay_pp_sameCE1<<totalNumUE<<","<<T_Average_Delay[1][runCase]<<endl;
			AverageDelay_pp_sameCE2<<totalNumUE<<","<<T_Average_Delay[2][runCase]<<endl;
			AverageDelay_pp_sameCE0.close();
			AverageDelay_pp_sameCE1.close();
			AverageDelay_pp_sameCE2.close();
			// resourceUtilization_pp_same_H.close();
			// AverageDelay_pp_sameCE0_H.close();
			// AverageDelay_pp_sameCE1_H.close();
			// AverageDelay_pp_sameCE2_H.close();
		}
		else if(highOfferedLoad==1)
		{
			totalNumUE=TotalNumUE_H[runCase];
		    resourceUtilization_pp_same_H<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_same_H.close();

			AverageDelay_pp_sameCE0_H<<totalNumUE<<","<<T_Average_Delay[0][runCase]<<endl;
			AverageDelay_pp_sameCE1_H<<totalNumUE<<","<<T_Average_Delay[1][runCase]<<endl;
			AverageDelay_pp_sameCE2_H<<totalNumUE<<","<<T_Average_Delay[2][runCase]<<endl;
			AverageDelay_pp_sameCE0_H.close();
			AverageDelay_pp_sameCE1_H.close();
			AverageDelay_pp_sameCE2_H.close();
		}
	}
	else if(simCase==2)
	{
		if(highOfferedLoad==0)
		{
			totalNumUE=TotalNumUE[runCase];
		 //    AverageDelay_pp_not_same_H<<totalNumUE<<","<<T_Average_Delay[runCase]<<endl;
		 //    resourceUtilization_pp_not_same_H<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			// resourceUtilization_pp_not_same_H.close();
			// AverageDelay_pp_not_same_H.close();
		    resourceUtilization_pp_not_same<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_not_same.close();

			AverageDelay_pp_not_sameCE0<<totalNumUE<<","<<T_Average_Delay[0][runCase]<<endl;
			AverageDelay_pp_not_sameCE1<<totalNumUE<<","<<T_Average_Delay[1][runCase]<<endl;
			AverageDelay_pp_not_sameCE2<<totalNumUE<<","<<T_Average_Delay[2][runCase]<<endl;
			AverageDelay_pp_not_sameCE0.close();
			AverageDelay_pp_not_sameCE1.close();
			AverageDelay_pp_not_sameCE2.close();
		}
		else if(highOfferedLoad==1)
		{
			totalNumUE=TotalNumUE_H[runCase];
		    resourceUtilization_pp_not_same_H<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_not_same_H.close();

			AverageDelay_pp_not_sameCE0_H<<totalNumUE<<","<<T_Average_Delay[0][runCase]<<endl;
			AverageDelay_pp_not_sameCE1_H<<totalNumUE<<","<<T_Average_Delay[1][runCase]<<endl;
			AverageDelay_pp_not_sameCE2_H<<totalNumUE<<","<<T_Average_Delay[2][runCase]<<endl;
			AverageDelay_pp_not_sameCE0_H.close();
			AverageDelay_pp_not_sameCE1_H.close();
			AverageDelay_pp_not_sameCE2_H.close();
		}
	}
	else if(simCase==3)
	{
		// totalNumUE=TotalNumUE_H[runCase];
	 //    resourceUtilization__payloadSize<<simDataSize<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
		resourceUtilization__payloadSize.close();
		// AverageDelay_payloadSizeCE0<<simDataSize<<","<<T_Average_Delay[0][runCase]<<endl;
		// AverageDelay_payloadSizeCE1<<simDataSize<<","<<T_Average_Delay[1][runCase]<<endl;
		// AverageDelay_payloadSizeCE2<<simDataSize<<","<<T_Average_Delay[2][runCase]<<endl;
		AverageDelay_payloadSizeCE0.close();
		AverageDelay_payloadSizeCE1.close();
		AverageDelay_payloadSizeCE2.close();

		Low_High_Payload_AverageDelay.close();
	}
    // LOG("End_Time:%d,Total # of UE:%d,T_Average_DelayCE0:%d,T_Average_DelayCE1:%d,T_Average_DelayCE2:%d,T_AvailResource[runCase]:%lf,T_OccupiedResource[runCase]:%lf,U:%lf\n",Sum_End_Time,totalNumUE,T_Average_Delay[0][runCase],T_Average_Delay[1][runCase],T_Average_Delay[2][runCase],T_AvailResource[runCase],T_OccupiedResource[runCase],T_OccupiedResource[runCase]/T_AvailResource[runCase]);

	fout_LOG.close();
	t = clock() - t;
	LOG("Computing Time:%f seconds\n",((float)t)/CLOCKS_PER_SEC);
	system("pause");
	return 0;
}

