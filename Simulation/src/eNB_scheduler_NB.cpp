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


#define SI_entry 2//#1:SIB1-NB, #2:SIB23
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
uint8_t CSS_flag=0;
//Evaluation Performance
double T_AvailResource[10]={0};
double T_OccupiedResource[10]={0};
int T_Average_Delay[10]={0};

int End_Time=0;
uint8_t TotalNumUE[10]={12,18,24,30,36,42,48,54,60,66};
// uint8_t TotalNumUE_H[10]={12,24,36,48,60,72,84,96,108,120};
uint8_t TotalNumUE_H[10]={24,48,72,96,120,144,168,192,216,240};
bool simCtrl=true;
uint8_t runCase;//0,1,2,3,4,5,6,7,8
extern int Sum_Delay;
extern int EndPoint;
extern uint32_t totalNumUE;
int Sum_nprach_resource_U=0;
extern int Sum_Occupied_resource__U;


//0: pp are different for 3 CE levels
//1: pp are same for 3 CE levels
//2: CSS/USS are the same
//3: CSS/USS are different(alpha offset is changed..)
# define simCase 1
//0:low offered load-->6.. report/sec
//1:high offered load-->12 or 18.. report/sec
uint8_t highOfferedLoad=1;
// # define highOfferedLoad 0
/*Case1,2*/
ofstream resourceUtilization_pp_same,AverageDelay_pp_same,resourceUtilization_pp_same_H,AverageDelay_pp_same_H;
ofstream resourceUtilization_pp_not_same,AverageDelay_pp_not_same,resourceUtilization_pp_not_same_H,AverageDelay_pp_not_same_H;

/*Case3,4*/
ofstream resourceUtilization_pp_same1,AverageDelay_pp_same1,resourceUtilization_pp_same_H1,AverageDelay_pp_same_H1;
ofstream resourceUtilization_pp_not_same2,AverageDelay_pp_not_same2,resourceUtilization_pp_not_same_H2,AverageDelay_pp_not_same_H2
;
int main(int argc, char const *argv[])//design simulation base on different argv/argc form bat...
{
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

	// oneThird_singleTone.open("Matlab_Result\\oneThird_singleTone.csv", ios::out);
	if(simCase)
	{
		if(highOfferedLoad!=1)
		{
			AverageDelay_pp_same.open("Matlab_Result\\AverageDelay_pp_same.csv", ios::app);
			resourceUtilization_pp_same.open("Matlab_Result\\resourceUtilization_pp_same.csv", ios::app);
		}
		else
		{
			AverageDelay_pp_same_H.open("Matlab_Result\\AverageDelay_pp_same_H.csv", ios::app);
			resourceUtilization_pp_same_H.open("Matlab_Result\\resourceUtilization_pp_same_H.csv", ios::app);
		}
	}
	else
	{
		if(highOfferedLoad!=1)
		{
			AverageDelay_pp_not_same.open("Matlab_Result\\AverageDelay_pp_not_same.csv", ios::app);
			resourceUtilization_pp_not_same.open("Matlab_Result\\resourceUtilization_pp_not_same.csv", ios::app);
		}
		else
		{
			AverageDelay_pp_not_same_H.open("Matlab_Result\\AverageDelay_pp_not_same_H.csv", ios::app);
			resourceUtilization_pp_not_same_H.open("Matlab_Result\\resourceUtilization_pp_not_same_H.csv", ios::app);
		}
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
	// system("pause");


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
		// if(((frame*10+subframes)%NPDCCH_period)==0) n_pp++;
		Ulsch_ind(frame,subframes,UL_Indicaiton);
		Sche_res(frame,subframes,Sche_Response);
		//New architecture delete UL/DL virtual channel structure
		//Build UL/DL virtual channel structure at the start of previous SF of each pp.
		// if(((H_SFN * 1024+frame * 10+subframes+1)%NPDCCH_period)==0)
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
		if((H_SFN * 10240+frame * 10+subframes+1)%640==0)
		{
			// NB_schedule_MIB(frame,subframes,NPDCCH_period,DL_Channel_bitmap,false);//NPBCH
		}
		//Schedule SIs(SIB1,SIB23) base on period.
		for (int i = 0; i < SI_entry; ++i)
		{
			if((H_SFN * 10240+frame * 10+subframes+1)%SI_period[i]==0)
			{
				// NB_schedule_SI(frame,subframes,NPDCCH_period,DL_Channel_bitmap,&MIB_NB_S, &SIB1_NB_S,false);
			}
		}
		//Schedule RA/UL/DL at the previous SF of each pp for three CE levels.
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
				LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule DCIs in CSS of next pp(%d~%d) for UL/DL at previous SF of CE %d pp\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+CSS_NPDCCH_period[i]),i);
				// system("pause");
				CSS_flag=1;
				// NB_schedule_RA(frame,subframes,DL_Channel_bitmap,UL_Channel_bitmap);
				NB_schedule_ulsch(scheH_SFN,scheFrame,scheSubframe,i,MIB_NB_S,SIB1_NB_S,SIB2_NB_S,Msg4_S,UL_Indicaiton,CSS_flag);
				// NB_schedule_dlsch(frame,subframes,DL_Channel_bitmap);
			}
		}
		for (int i = 0; i < 3; ++i)
		{
			if(((H_SFN * 10240+frame * 10+subframes+1)%USS_NPDCCH_period[i])==0)
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
				LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule DCIs in USS of next pp(%d~%d) for UL/DL at previous SF of CE %d pp\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+USS_NPDCCH_period[i]),i);
				// system("pause");
				CSS_flag=0;
				// NB_schedule_RA(frame,subframes,DL_Channel_bitmap,UL_Channel_bitmap);
				NB_schedule_ulsch(scheH_SFN,scheFrame,scheSubframe,i,MIB_NB_S,SIB1_NB_S,SIB2_NB_S,Msg4_S,UL_Indicaiton,CSS_flag);
				// NB_schedule_dlsch(frame,subframes,DL_Channel_bitmap);
			}
		}
		/*End NB_eNB_dlsch_ulsch_scheduler*/

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
		//UE_list for three CE levels already empty...
		if(EndPoint==3)
		{
			LOG("UE_Info_List for three CE level are empty\n");
			// system("pause");
			LOG("H_SFN:%d,frame:%d,subframes:%d,End_Time:%d\n",H_SFN,frame,subframes,End_Time);
			Sum_nprach_resource_U=get_nprah_resource(End_Time,SIB2_NB_S);
			LOG("Sum_nprach_resource_U:%d\n",Sum_nprach_resource_U);
			break;
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
    T_Average_Delay[runCase]=Sum_Delay/totalNumUE;
    T_AvailResource[runCase]=(double)((End_Time * 12) - Sum_nprach_resource_U);
	T_OccupiedResource[runCase]=(double)(Sum_Occupied_resource__U);
    LOG("End_Time:%d,Total NUmber of UE:%d,T_Average_Delay:%d,T_AvailResource[runCase]:%lf,T_OccupiedResource[runCase]:%lf\n",End_Time,totalNumUE,T_Average_Delay[runCase],T_AvailResource[runCase],T_OccupiedResource[runCase]);
    // system("pause");
	if(simCase)
	{
		if(highOfferedLoad!=1)
		{
		    AverageDelay_pp_same<<totalNumUE<<","<<T_Average_Delay[runCase]<<endl;
		    resourceUtilization_pp_same<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_same.close();
			AverageDelay_pp_same.close();
		}
		else
		{
		    AverageDelay_pp_same_H<<totalNumUE<<","<<T_Average_Delay[runCase]<<endl;
		    resourceUtilization_pp_same_H<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_same_H.close();
			AverageDelay_pp_same_H.close();
		}
	}
	else
	{
		if(highOfferedLoad!=1)
		{
		    AverageDelay_pp_not_same_H<<totalNumUE<<","<<T_Average_Delay[runCase]<<endl;
		    resourceUtilization_pp_not_same_H<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_not_same_H.close();
			AverageDelay_pp_not_same_H.close();
		}
		else
		{
		    AverageDelay_pp_not_same<<totalNumUE<<","<<T_Average_Delay[runCase]<<endl;
		    resourceUtilization_pp_not_same<<totalNumUE<<","<<T_OccupiedResource[runCase]/T_AvailResource[runCase]<<endl;
			resourceUtilization_pp_not_same.close();
			AverageDelay_pp_not_same.close();
		}
	}
	system("pause");
	return 0;
}

