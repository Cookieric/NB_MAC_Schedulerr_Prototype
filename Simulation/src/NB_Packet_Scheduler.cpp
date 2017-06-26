#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <list>
#include <fstream>
// #include "testArrivalModel.cpp"
using namespace std;

#define LOG(...) printf(__VA_ARGS__)
// #define _HAS_ITERATOR_DEBUGGING 0






typedef struct {
	double target_SNR[3];
	int CE_Level[3];
	int rep[3];//nprach-Repetition
	int period[3];//nprach-Periodicity
	int start_time[3];
	int num_Subcarrier[3];
	int subcarrier_Offset[3];
	//DCI N1 for RAR and Msg4, DCI N0 for Msg3 retransmission
	int npdcch_NumRepetitions_RA[3];//Maximum number of repetitions for NPDCCH common search space (CSS) for RAR, Msg3 retransmission and Msg4
	double npdcch_Offset_RA[3];//Fractional period offset of starting subframe for NPDCCH common search space (CSS Type 2)
	int npdcch_StartSF_CSS_RA[3];//Starting subframe configuration for NPDCCH common search space (CSS), including RAR, Msg3 retransmission, and Msg4
	int rawindow[3];//SIB2-->RadioResourceConfigCommonSIB-NB-r13-->RACH-ConfigCommon-NB
	int ContentionResolutionTimer[3];//SIB2-->RadioResourceConfigCommonSIB-NB-r13-->RACH-ConfigCommon-NB
	int candidate[3];//=Rmax/R, R defined by DCI repetition number in DCI
	int I_SI_NPRACH;//44ms, a variale gap
	//The following value depend on NPDCCH config and DCI format
	int I_Wait_to_NPDCCH;//Interval from end pos of preamble to start DCI N1(RAR) and RAR
	int I_NPDCCH_RAR;//DCI_repetition+ko
	int I_RAR_Msg3;//Interval from end of RAR to start of Msg3
	int I_Msg3_DCIN0;//Interval from end pos of Ms3 to start pos of DCI N0
	int I_DCIN0_ULPacket;//Interval from start of DCI0,DCI Rep to start of ULPacket
	int DL_repetiitonLevel;
} NB_SIB2;

typedef struct _Msg2_Struc{
	int start_time;
	int Repetition;
} Msg2_Struc;

//Msg4-->RadioResourceConfigDedicated-NB-r13-->physicalConfigDedicated-r13-->npdcch-ConfigDedicated-r13
typedef struct _npdcch_ConfigDedicated_r13{
	int npdcch_NumRepetitions;
	double npdcch_StartSF_USS;
	double npdcch_Offset_USS;
} npdcch_ConfigDedicated_r13;

//Msg4-->RadioResourceConfigDedicated-NB-r13-->MAC-MainConfig-NB-->ul-SCH-Config-r13
typedef struct _ULSCH_Config_r13{
	int PeriodicBSR_Timer_NB_r13;//set to 8 pp?
	int RetxBSR_Timer_NB_r13;//set to infinity
} ULSCH_Config_r13;

typedef struct _Msg4_Struc{
	npdcch_ConfigDedicated_r13 ueSpecificConfig;
	ULSCH_Config_r13 bsrConfig;
} Msg4_Struc;

typedef struct _DCIFormat0{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1
  uint32_t type;
  /// Subcarrier indication
  uint32_t scind;
  /// Resourse Assignment (RU Assignment)
  uint32_t ResAssign;
  /// Modulation and Coding Scheme and Redundancy Version
  uint32_t mcs;
  /// New Data Indicator
  uint32_t ndi;
  /// Scheduling Delay
  uint32_t Scheddly;
  /// Repetition Number
  uint32_t RepNum;
  /// Redundancy version for HARQ (only use 0 and 2)
  uint32_t rv;
  /// DCI subframe repetition Number
  uint32_t DCIRep;
} DCIFormat0;

///  DCI Format Type N1
typedef struct _DCIFormat1{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1
  uint32_t type;
  //NPDCCH order indicator (set to 0)
  uint32_t orderIndicator;
  // Scheduling Delay
  uint32_t Scheddly;
  // Resourse Assignment (RU Assignment)
  uint32_t ResAssign;
  // Modulation and Coding Scheme and Redundancy Version
  uint32_t mcs;
  // Repetition Number
  uint32_t RepNum;
  // New Data Indicator
  uint32_t ndi;
  // HARQ-ACK resource
  uint32_t HARQackRes;
  // DCI subframe repetition Number
  uint32_t DCIRep;
} DCIFormat1;

///  DCI Format Type N1 for RAR
typedef struct _DCIFormat1_RAR{
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1
  uint32_t type;
  //NPDCCH order indicator (set to 0)
  uint32_t orderIndicator;
  // Scheduling Delay
  uint32_t Scheddly;
  // Resourse Assignment (RU Assignment)
  uint32_t ResAssign;
  // Modulation and Coding Scheme and Redundancy Version
  uint32_t mcs;
  // Repetition Number
  uint32_t RepNum;
  // Reserved 5 bits
  uint32_t Reserved;
  // DCI subframe repetition Number
  uint32_t DCIRep;
} DCIFormat1_RAR;
// typedef struct DCIN0 DCIN0_t;
// #define sizeof_DCIN0_t 23

typedef struct _DCI{
	DCIFormat0 DCIN0;
	DCIFormat0 BSRgrant;
	DCIFormat1 DCIN1;
	DCIFormat1_RAR DCIN1_RAR;
}DCI;

typedef struct _NPDSCH{
	Msg2_Struc Msg2;
	Msg4_Struc Msg4;
}NPDSCH;

class UE_info
{
public:
	//Get from SIB2-NB
	int CE_Level;	//After RA Succeed, UE will know own CE level.
	//Get from Msg1
	int multi_tone_Msg3;// 0: not support; 1:support
	bool RAR;
	//Msg3 MAC Control Element
	int UE_id;		//UE_id determine by TC-RNTI in RAR
	int DV;	//BS know UE's UL Buffer Data Size.(CP solution)
	int PHR;	//BS know UE's Power budget.
	//Msg3 content
	int multi_tone;// 0: not support; 1:support
	bool Msg4;
	//BSR
	int shortBSR;
	//Scheduled Parameters
	int Priority;	 //BS determine priority depends on DV/BSR, more tone be used or CE level
	int UL_Buffer_Sizes;
	int allocate_Buffer_Sizes;
	int mcs;//mcs Index
	int Qm;
	int round;
	int remaining_Buffer_Sizes;
	int startFreqPos;
	//NPUSCH RU Table
	int startScheMsg3;
	int num_tone;
	int num_UL_Slot;
	int UE_num_RU;
	int remainging_subframe;
};

// class UE_BSR
// {
// public:
// 	int UE_id;		//BS identify UE by UE_id
// 	int Priority;	//BS determine priority depends on DV/BSR, more tone be used or CE level
// 	int CE_Level;	//After RA Succeed, UE will know own CE level.
// 	int allocate_Buffer_Sizes;
// 	int multi_tone;// 0: not support; 1:support
// 	int mcs;//mcs Index
// 	int Qm;
// 	int round;
// 	int remaining_Buffer_Sizes;
// 	//NPUSCH RU Table
// 	int num_subcarrier_perRU;
// 	int num_UL_Slot;
// 	int UE_num_RU;
// 	int remainging_subframe;
// };

int UL_TBS_Table[13][8]={{16,2,56,88,120,152,208,256},
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

int InitialTimerConfig(NB_SIB2 &,NPDSCH &);

int Auto_Filter_NB_SIB2Config(double **, double **);
int Set_NPRACH_Resource(double **, NB_SIB2 &, int);
int checkFrequencyDomain(NB_SIB2 &);
int nprachResourceMapping(int,NB_SIB2 &,int **);

int UE_Init(UE_info &);//not used for now
int transfer_PHR(UE_info &, int);
int transfer_MCS(UE_info &);
int transfer_DV(UE_info &);
int get_UL_Data_Bytes(int);

int resourceAllocate(UE_info &);
int get_num_Slot(int);
int get_TBS_UL(int,int,int);
int get_I_TBS(int);
int get_I_RU(int);

int get_num_subcarrier_perRU(int);
int get_startFreqPos(int **,int ,int);
int preSubframeMapping(int, UE_info &,NB_SIB2 &,int **);
// bool lock=false;

int main(int argc, char const *argv[])
{
	// if(argc < 2 || *(argv+2)== NULL)
	// {
	// 	printf("No Argument Parameters\n");
	// 	system("pause");
	// 	return 0;
	// 	printf("%s\n", *argv);
	// 	printf("%s\n", *(argv+1));
 //    	perror("perror");
 //    	printf("strerror(errno)=%s\n", strerror(errno));
	// }
	// system("pause");
	srand(time(NULL));
	int number_of_target_SNR=3;
	// int number_of_target_SNR=atoi(argv[1]);//Convert string to integer,int atoi (const char * str);
	double **passSetting;
	double **indicateSIB;
	NB_SIB2 NB_SIB2_Struc= {0};//Directly Initialize
	NPDSCH NPDSCH_Struc= {0};
	DCI DCIFormat= {0};
	// InitialTimerConfig(NB_SIB2_Struc,NPDSCH_Struc);
	// DCIFormat.DCIN0.RepNum=1;
	// NPDSCH_Struc.Msg2_Struc.Rep=1;
	// LOG("%d\n",NPDSCH_Struc.Msg2_Struc.Rep);
	// LOG("%d\n",DCIFormat.DCIN0.RepNum);
	// system("pause");
	int size=400000;
	passSetting = new double*[size];
	indicateSIB = new double*[number_of_target_SNR];
	for (int i = 0; i < size; ++i)	passSetting[i] = new double [7];
	for (int i = 0; i < number_of_target_SNR; ++i)	indicateSIB[i] = new double [7];
	// system("pause"); Time domain limitation adjust will coredump...
	if(Auto_Filter_NB_SIB2Config(passSetting, indicateSIB)==1)
	{
		LOG("NB_SIB2 Config CE(0,1,2) set done and Pass Time domain limitation!\n");
		system("pause");
	}
	// LOG("%d",number_of_target_SNR);
	// for (int i = 0; i < 3; ++i)
	// {
	// 	for (int j = 0; j < 7; ++j)
	// 	{
	// 		LOG("%d ",(int)indicateSIB[i][j]);
	// 	}
	// 	LOG("\n");
	// }
	// system("pause");
	if(Set_NPRACH_Resource(indicateSIB, NB_SIB2_Struc, number_of_target_SNR)==-1)
	{
		LOG("No NPRACH Configuration: BS Failed!\n");
		return 0;
	}
	LOG("Setting three CE levels NPRACH Parameters\n");
	delete []passSetting;
	delete []indicateSIB;

	int period_NPRACH=NB_SIB2_Struc.period[0];
	for (int i = 0; i < 2; ++i)
	{
		if(period_NPRACH<NB_SIB2_Struc.period[i+1])	period_NPRACH=NB_SIB2_Struc.period[i+1];
	}
	cout<<"MAX period_NPRACH: "<<period_NPRACH<<endl;
	system("pause");
	// for (int i = 0; i < period_NPRACH; ++i)
	// {
	// 	for (int j = 0; j < 48; ++j)
	// 	{
	// 		cout<<UL_Channle_Struc[j][i]<<' ';
	// 	}
	// 	cout<<endl;
	// }
	// system("pause");
	// return 0;

	int pcmax=23;//Use for calculate PHR in UE
	int measuredpuschpower=0;//UE Measured quantity value (dB) in R4-1610577

	// //Set simulation times?
	// const int user=10;
	// int * arrivalTime = new int[user];
	// list<int> arrivalPerdSubframe;
	// define_system_arrival(arrivalPerdSubframe,user,arrivalTime);
	// // for (list<int>::iterator it=arrivalPerdSubframe.begin(); it!=arrivalPerdSubframe.end(); ++it)
	// // 	cout << *it <<endl;
	// for (int i = 0; i < user; ++i)
	// {
	// 	// arrivalTime[i]=arrivalTime[i]+Gdelay;
	// 	cout<<"First Arrival:"<<arrivalTime[i]<<endl;
	// }
	// system("pause");
	// return 0;
	//First Arrival Process for UEs
	//Add_ue timing point is fill_rar in OAI, after add_ue will do generate_eNB_ulsch_params_from_rar & get_Msg3_alloc, it means sucessfully decode preamble then add UE
	//Arrive at the same time.
	list<UE_info> UL_Sche_UE_List;
	list<UE_info>::iterator it1;
	UE_info UL_Sche_UE;
	// double numUE=10;
	double numUE[3]={10,20,30};
	int delta=3;//3 for 0.66, 10 for 0.1, other value for 0.9 single-tone ratio
	ofstream oneThird_singleTone,oneTen_singleTone,nineTen_singleTone;

	if(delta==3)	oneThird_singleTone.open("Matlab_Result_Test\\oneThird_singleTone.csv", ios::out);
	else if(delta==10)	oneTen_singleTone.open("Matlab_Result_Test\\oneTen_singleTone.csv", ios::out);
	else	nineTen_singleTone.open("Matlab_Result_Test\\nineTen_singleTone.csv", ios::out);

	bool singleMost=false;
	int simTimes=1;
	// double utilization=0;
	// double singleToneRatio = 0;

	for (int z = 0; z < 3; ++z)
	{
		LOG("# of UE:%lf\n",numUE[z]);
		// system("pause");
		double utilization=0;
		double singleToneRatio = 0;
		for (int i = 0; i < simTimes; ++i)
		{
			// LOG("simulation Time:%d\n",i+1);
			if(delta!=3&&delta!=10)	singleMost=true;
			int UE_id=10;
			int cntTone=0;
			//NPRACH Period:80, NPRACH resource=12*6(CE0)+12*12(CE1)+24*23(CE2),availResource=80 * 48-NPRACH resource
			int availResource=3072;
			int occupiedResource=0;
			int sumAvailResource=0;
			int sumOccupiedResource=0;
			int finalPacketPos=-1;
			// bool recordTone=false;
			for (int i = 0; i < (int)numUE[z]; ++i)	UL_Sche_UE_List.push_back (UL_Sche_UE);
			for (it1=UL_Sche_UE_List.begin(); it1!=UL_Sche_UE_List.end(); ++it1)
			{
				UE_info *i = &*it1;//need to convert from iterator to (UE_info *)
				//UE_Init(*i);
				// UL_Sche_UE_List.UE_id=UE_id;
				i->UE_id=UE_id;
				i->UE_num_RU=0;
				i->num_tone=0;
				i->startFreqPos=0;
				i->remainging_subframe=0;
				++UE_id;
				i->round=1;
				i->CE_Level=rand() % 3; //decided by UE's RSRP in RA procedure
				int puschPowerEstimate=1+rand() % 46;// Needed NPUSCH:1~46
				measuredpuschpower=pcmax-puschPowerEstimate;
				i->PHR=transfer_PHR(*i,measuredpuschpower);
				i->UL_Buffer_Sizes=100+rand()%181; //100~280 bytes for each UE
				transfer_DV(*i);	//decided by UE's UL buffer
				i->multi_tone_Msg3=rand() % 3; //decided by Msg3's content
				if(singleMost)
				{
					if((double)rand()/RAND_MAX>0.1)	i->multi_tone=0;
					else	i->multi_tone=1;
				}
				else	i->multi_tone=rand() % delta;//Fixed to change variable single-tone ration, delta:3 or 10
				if(i->multi_tone>2)	i->multi_tone=1;//Fixed to 1 or 2, not exceed 2 or bug exist in getFreq Fun.
				if(i->multi_tone==0)	cntTone++;
				transfer_MCS(*i);//Transfer from PHR to MCS
				//modulation order=2 if multitoneSupport
				if(i->multi_tone!=0)	i->Qm=2;
				else
				{
					if(i->mcs>1)	i->Qm=2;
					else i->Qm=1;
				}
				// i->mcs=2+rand()%9;//Qm=2,MCS:2-10 36213 Table 16.5.1.2-1
				// int I_RAR=NPRACH_Struc.rep[i]=* (rand()%successPreambleCE0);
				resourceAllocate(*i);
				cout<<"UE_id: " <<i->UE_id<<"; CE_Level: "<<i->CE_Level<<"; multi_tone: "<<i->multi_tone<<"; Buffer size: "<<i->UL_Buffer_Sizes<<"; DV: "<<i->DV<<"; PHR: "<<i->PHR<<"; MCS: "<<i->mcs<<"; Qm: "<<i->Qm<<"; TBS: "<<i->allocate_Buffer_Sizes<<"; num_RU: "<<i->UE_num_RU<<"; Remaining_Buffer_Sizes: "<<i->remaining_Buffer_Sizes<<"; Round: "<<i->round<<endl;
			}

			// cout<<"New Arrival Allocate Parts Of DCI setting done..."<<endl;
			// system("pause");
			int num_UL_Subcarrier=48;
			while(!UL_Sche_UE_List.empty())
			{
				int **UL_Channle_Struc = new int*[num_UL_Subcarrier];
				for (int i = 0; i < num_UL_Subcarrier; ++i)
					UL_Channle_Struc[i] = new int [period_NPRACH];
				for (int i = 0; i < num_UL_Subcarrier; ++i)
					for (int j = 0; j < period_NPRACH; ++j)
						UL_Channle_Struc[i][j]=0;
				if(nprachResourceMapping(number_of_target_SNR,NB_SIB2_Struc,UL_Channle_Struc)==0)
					// cout<<"nprachResourceMapping done"<<endl;
					// system("pause");
				// for (int i = 0; i < period_NPRACH; ++i)
				// {
				// 	for (int j = 0; j < 48; ++j)
				// 	{
				// 		cout<<UL_Channle_Struc[j][i]<<' ';
				// 	}
				// 	cout<<endl;
				// }
				// system("pause");
				// return 0;
				//Do subframeMapping per subframe in a specific arraySize.
				for (int j = 0; j < period_NPRACH; ++j)
				{
					for (it1=UL_Sche_UE_List.begin(); it1!=UL_Sche_UE_List.end(); ++it1)
					{
						UE_info *i = &*it1;
						int flag=preSubframeMapping(j,*i,NB_SIB2_Struc,UL_Channle_Struc);
						if(i->remaining_Buffer_Sizes<=0&&i->remainging_subframe==0&&flag!=-1)
						{
							// cout<<"UE_id: "<<i->UE_id<<" has finished packet transmission"<<endl;
							it1 = UL_Sche_UE_List.erase (it1);
							--it1;
						}
						if(i->remaining_Buffer_Sizes>0&&i->remainging_subframe==0&&flag!=-1)
						{
							// cout<<"DCI for this round done, let BSR as new arrival for BSR_UE struc after Bdelay..."<<endl;
							// cout<<"UE_id: "<<i->UE_id<<"; remaining_Buffer_Sizes: "<<i->remaining_Buffer_Sizes<<endl;
							// system("pause");
							it1 = UL_Sche_UE_List.erase (it1);
							--it1;
							if(UL_Sche_UE_List.empty())
							{
								finalPacketPos=j;
								// cout<<"finalPacketPos= "<<j<<endl;
							}
						}
						// it1 = mylist.erase (it1);
					}
				}
				// for (it1=UL_Sche_UE_List.begin(); it1!=UL_Sche_UE_List.end(); ++it1)
				// {
				// 	UE_info *i = &*it1;//need to convert from iterator to (UE_info *)
				// 	// if(i->remaining_Buffer_Sizes>0) i->round++;
				// 	if(i->remainging_subframe==0)	it1 = UL_Sche_UE_List.erase (it1);
				// }
				// cout<<"subframe-base scheduing for RA period done..."<<endl;
				// cout<<endl;
				// system("pause");
				/*Print out NPUSCH/NPRACH Bitmap*/
				for (int i = 0; i < period_NPRACH; ++i)
				{
					for (int j = 0; j < 48; ++j)
					{
						if(UL_Channle_Struc[j][i]>=10)//detect UE_id in this resource unit
								occupiedResource++;
						if(0<=UL_Channle_Struc[j][i]&&UL_Channle_Struc[j][i]<=3)
							cout<<UL_Channle_Struc[j][i]<<"  ";
						else
							cout<<UL_Channle_Struc[j][i]<<' ';
					}
					cout<<endl;
				}
				delete []UL_Channle_Struc;
				if(finalPacketPos==-1)
				{
					sumOccupiedResource=sumOccupiedResource+occupiedResource;
					sumAvailResource=sumAvailResource+availResource;
					// cout<<"sumOccupiedResource: "<<sumOccupiedResource<<"/sumAvailResource: "<<sumAvailResource<<endl;
				}
				else
				{
					sumOccupiedResource=sumOccupiedResource+occupiedResource;
					if(0<=finalPacketPos&&finalPacketPos<=7)
						sumAvailResource=sumAvailResource+(finalPacketPos+1) * 48;
					else if(14<=finalPacketPos&&finalPacketPos<=19)
						sumAvailResource=sumAvailResource+(finalPacketPos+1) * 12-12 * 6;//48-24-12
					else if(20<=finalPacketPos&&finalPacketPos<=30)
						sumAvailResource=sumAvailResource+(finalPacketPos+1) * 12-12 * 6+(finalPacketPos-19)* 12;
					else
						sumAvailResource=sumAvailResource+(finalPacketPos+1) * 48-768;
					// cout<<"sumOccupiedResource: "<<sumOccupiedResource<<"/sumAvailResource: "<<sumAvailResource<<endl;
				}
				occupiedResource=0;
				// cout<<"Remaining UE List:"<<endl;
				for (it1=UL_Sche_UE_List.begin(); it1!=UL_Sche_UE_List.end(); ++it1)
				{
					UE_info *i = &*it1;//need to convert from iterator to (UE_info *)
					// i->round++;
					// lock=false;//May need to fixed, lock timing set flase for New Arrival and > 1 round UEs....
					// cout<<"UE_id: " <<i->UE_id<<"; multi_tone"<<i->multi_tone<<"; Buffer size: "<<i->UL_Buffer_Sizes<<"; TBS: "<<i->allocate_Buffer_Sizes<<"; Remaining_Buffer_Sizes: "<<i->remaining_Buffer_Sizes<<"; num_RU: "<<i->UE_num_RU<<"; num_ULslots: "<<get_num_Slot(i->num_tone)<<" ;Repetition: "<<pow(2,i->CE_Level)<<"; Remaining_Subframe: "<<i->remainging_subframe<<"; Round: "<<i->round<<endl;
				}
			}
			// cout<<"sumOccupiedResource: "<<sumOccupiedResource<<"/sumAvailResource: "<<sumAvailResource<<endl;
			singleToneRatio = singleToneRatio + cntTone/numUE[z];
			utilization= utilization + (double)sumOccupiedResource/sumAvailResource;
			// cout<<"single-tone UEs/TotoalUEs= "<<singleToneRatio<<endl;
			// cout<<"utilization: "<<utilization<<endl;
		}
		cout<<"Average single-tone UEs/TotoalUEs= "<<singleToneRatio/simTimes<<endl;
		// cout<<"Number of UE: "<<numUE[z]<<endl;
		cout<<"Average utilization: "<<utilization/simTimes<<endl;
		cout<<"-------------------------------------------"<<endl;
		if(delta==3)	oneThird_singleTone << numUE << "," << utilization/simTimes << endl;
		else if(delta==10)	oneTen_singleTone << numUE << "," << utilization/simTimes << endl;
		else	nineTen_singleTone << numUE[z] << "," << utilization/simTimes << endl;
		system("pause");
	}
	if(delta==3)	oneThird_singleTone.close();
	else if(delta==10)	oneTen_singleTone.close();
	else	nineTen_singleTone.close();


	// for (int i = 0; i < period_NPRACH; ++i)
	// {
	// 	for (int j = 0; j < 48; ++j)
	// 	{
	// 		if(0<=UL_Channle_Struc[j][i]&&UL_Channle_Struc[j][i]<=3)
	// 			cout<<UL_Channle_Struc[j][i]<<"  ";
	// 		else
	// 			cout<<UL_Channle_Struc[j][i]<<' ';
	// 	}
	// 	cout<<endl;
	// }

		// for (int i = 0; i < num_UL_Subcarrier; ++i)
		// for (int j = 0; j < period_NPRACH; ++j)
		// 	UL_Channle_Struc[i][j]=0;
	cout<<"MappingResult Check done..."<<endl;
	// system("pause");
	// delete []UL_Channle_Struc;
	system("pause");
	return 0;
}

int preSubframeMapping(int subframe, UE_info &UL_Sche_UE,NB_SIB2 &NB_SIB2_Struc,int ** UL_Channle_Struc)
{
	//start_time of three CE level are same
	int skipPrachCE0=NB_SIB2_Struc.start_time[0];
	int skipPrachCE1=NB_SIB2_Struc.start_time[1];
	int skipPrachCE2=NB_SIB2_Struc.start_time[2];
	// static int startTimeSixTone=0;
	// static int startTimeThreeTone=0;
	// static int startTimeSingleTone[3]={0,0,0};
	if(UL_Sche_UE.multi_tone==0)
	{
		int skipTTI=ceil(1.4 * 4 * NB_SIB2_Struc.rep[0]);//Skip NPRACH resource from start to end of CE0
		if(skipPrachCE0<=subframe&&subframe<skipPrachCE0+skipTTI)	return -1;
		// static int startFreqPos=0;
		if(UL_Sche_UE.num_tone==0)// if tone set done will fixed.
		{
			int startFreqPos=get_startFreqPos(UL_Channle_Struc,subframe,UL_Sche_UE.multi_tone);
			if(startFreqPos==-1)	return -1;//No resource for this UE cause other UEs in high priority has used in the subframe
			int num_subcarrier_perRU=get_num_subcarrier_perRU(startFreqPos);
			UL_Sche_UE.num_tone=num_subcarrier_perRU;
			UL_Sche_UE.startFreqPos=startFreqPos;
		}
		else
		{
			for (int i = UL_Sche_UE.startFreqPos; i < UL_Sche_UE.startFreqPos+4; ++i)
			{
				if(UL_Channle_Struc[i][subframe]==0)
				{
					UL_Sche_UE.startFreqPos=i;
					break;
				}
				else	return -1;
			}
			// int startFreqPos=get_startFreqPos(UL_Channle_Struc,subframe,UL_Sche_UE.startFreqPos);
			// if(startFreqPos==-1)	return -1;
		}
		if(UL_Sche_UE.remainging_subframe==0)// set for the first time
		{
			int Repetition=pow(2,UL_Sche_UE.CE_Level);//0,1,2->Repetition=1,2,4
			// cout<<"Repetition: "<<Repetition<<endl;
			UL_Sche_UE.remainging_subframe = Repetition * UL_Sche_UE.UE_num_RU * get_num_Slot(UL_Sche_UE.num_tone) * 0.5;//Rep*Num of RU*ULslot(0.5 ms per slot), Rep may need to adjust depend on mcs or CE level or SINR.
			// lock=true;
			// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; Remainging_subframe: "<< UL_Sche_UE.remainging_subframe <<endl;
		}
		int occupy_Freq=UL_Sche_UE.num_tone * 4;//(3.75 * 4) ->15 kHz
		for (int i = UL_Sche_UE.startFreqPos; i < UL_Sche_UE.startFreqPos+occupy_Freq; ++i)//resourceMapping per subframe
		{
			UL_Channle_Struc[i][subframe]=UL_Sche_UE.UE_id;
		}
		UL_Sche_UE.remainging_subframe--;
		// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; subframe: "<<subframe<<"; startFreqPos: "<<startFreqPos<<"Remainging_subframe:"<<UL_Sche_UE.remainging_subframe<<endl;
		// system("pause");
	}
	else// UE support multi-tone
	{
		// static int startFreqPos=0;
		if(UL_Sche_UE.num_tone==0)
		{
			int startFreqPos=get_startFreqPos(UL_Channle_Struc,subframe,UL_Sche_UE.multi_tone);
			if(startFreqPos==-1)	return -1;
			int num_subcarrier_perRU=get_num_subcarrier_perRU(startFreqPos);
			UL_Sche_UE.num_tone=num_subcarrier_perRU;
		}
		// int occupy_Freq=num_subcarrier_perRU * 4;//(3.75 * 4) ->15 kHz
		if(UL_Sche_UE.num_tone==3)
		{
			int startFreqPos=get_startFreqPos(UL_Channle_Struc,subframe,UL_Sche_UE.num_tone);
			if(startFreqPos==-1)	return -1;
			// int skipTTI=ceil(1.4 * 4 * NB_SIB2_Struc.rep[1]);//Skip CE 1 NPRACH resource
			// if(skipPrachCE1<=subframe&&subframe<skipPrachCE1+skipTTI)	return -1;
			// int startFreqPos=get_startFreqPos(UL_Channle_Struc,subframe,UL_Sche_UE.multi_tone);
			// if(startFreqPos==-1)	return -1;//No resource for UE cause other UEs in high priority has used in the subframe
			// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; subframe: "<<subframe<<"; startFreqPos: "<<startFreqPos<<endl;
			// system("pause");
			if(UL_Sche_UE.remainging_subframe==0)
			{
				int Repetition=pow(2,UL_Sche_UE.CE_Level);//0,1,2->Repetition=1,2,4
				// cout<<"Repetition: "<<Repetition<<endl;
				UL_Sche_UE.remainging_subframe = Repetition * UL_Sche_UE.UE_num_RU * get_num_Slot(UL_Sche_UE.num_tone) * 0.5;
				// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; Remainging_subframe: "<< UL_Sche_UE.remainging_subframe <<endl;
			}
			int occupy_Freq=UL_Sche_UE.num_tone * 4;//(3.75 * 4) ->15 kHz
			for (int i = startFreqPos; i < startFreqPos+occupy_Freq; ++i)
			{
				UL_Channle_Struc[i][subframe]=UL_Sche_UE.UE_id;
			}
			UL_Sche_UE.remainging_subframe--;
			// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; subframe: "<<subframe<<"; startFreqPos: "<<startFreqPos<<"Remainging_subframe:"<<UL_Sche_UE.remainging_subframe<<endl;
			// if(UL_Sche_UE.remainging_subframe==0)	it1 = UL_Sche_UE_List.erase (it1);
			// system("pause");
		}
		else if(UL_Sche_UE.num_tone==6)
		{
			int startFreqPos=get_startFreqPos(UL_Channle_Struc,subframe,UL_Sche_UE.num_tone);
			if(startFreqPos==-1)	return -1;
			// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; subframe: "<<subframe<<"; startFreqPos: "<<startFreqPos<<endl;
			// system("pause");
			// int skipTTI=ceil(1.4 * 4 * NB_SIB2_Struc.rep[2]);//Skip CE 2 NPRACH resource
			// if(skipPrachCE2<=subframe&&subframe<=skipPrachCE2+skipTTI)	return -1;
			if(UL_Sche_UE.remainging_subframe==0)
			{
				int Repetition=pow(2,UL_Sche_UE.CE_Level);//0,1,2->Repetition=1,2,4
				// cout<<"Repetition: "<<Repetition<<endl;
				UL_Sche_UE.remainging_subframe = Repetition * UL_Sche_UE.UE_num_RU * get_num_Slot(UL_Sche_UE.num_tone) * 0.5;
				// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; Remainging_subframe: "<< UL_Sche_UE.remainging_subframe <<endl;
			}
			int occupy_Freq=UL_Sche_UE.num_tone * 4;//(3.75 * 4) ->15 kHz
			for (int i = startFreqPos; i < startFreqPos+occupy_Freq; ++i)
			{
				UL_Channle_Struc[i][subframe]=UL_Sche_UE.UE_id;
			}
			UL_Sche_UE.remainging_subframe--;
			// cout<<"UE_id: "<<UL_Sche_UE.UE_id<<"; subframe: "<<subframe<<"; startFreqPos: "<<startFreqPos<<"Remainging_subframe:"<<UL_Sche_UE.remainging_subframe<<endl;
			// system("pause");
		}
	}
	return 0;
}

int nprachResourceMapping(int number_of_target_SNR,NB_SIB2 &NB_SIB2_Struc,int **UL_Channle_Struc)
{
	for (int i = 0; i < number_of_target_SNR; ++i)
	{
		int occupy_Subframe=ceil(NB_SIB2_Struc.rep[i] * 1.4 * 4);
		int occupy_Freq=NB_SIB2_Struc.num_Subcarrier[i];
		if(i==0)
		{
			for (int j = NB_SIB2_Struc.start_time[i]; j < NB_SIB2_Struc.start_time[i]+occupy_Subframe; ++j)
			{
				for (int z = NB_SIB2_Struc.subcarrier_Offset[i]; z < NB_SIB2_Struc.subcarrier_Offset[i]+occupy_Freq; ++z)
					UL_Channle_Struc[z][j]=1;// 1.occupied subcarrier
			}
		}
		else if(i==1)
		{
			for (int j = NB_SIB2_Struc.start_time[i]; j < NB_SIB2_Struc.start_time[i]+occupy_Subframe; ++j)
			{
				for (int z = NB_SIB2_Struc.subcarrier_Offset[i]; z < NB_SIB2_Struc.subcarrier_Offset[i]+occupy_Freq; ++z)
					UL_Channle_Struc[z][j]=2;// 1.occupied subcarrier
			}
		}
		else if(i==2)
		{
			for (int j = NB_SIB2_Struc.start_time[i]; j < NB_SIB2_Struc.start_time[i]+occupy_Subframe; ++j)
			{
				for (int z = NB_SIB2_Struc.subcarrier_Offset[i]; z < NB_SIB2_Struc.subcarrier_Offset[i]+occupy_Freq; ++z)
					UL_Channle_Struc[z][j]=3;// 1.occupied subcarrier
			}
		}
	}
	return 0;
}

int get_num_Slot(int x)
{
	if(x==1)	return 16;
	else if(x==3)	return 8;
	else if(x==6)	return 4;
	else if(x==12)	return 2;
	else	return -1;
}

int resourceAllocPattern0[5]={0,24,36,40,44};//freqOffset for resource mapping, 6 tone/3 tone/1*3 tone from bottom to up
int resourceAllocPattern1[5]={0,4,8,12,24};//freqOffset for resource mapping, 1*3 tone/3 tone/6 tone from bottom to up
int resourceAllocPattern2[8]={0,24,36,40,44,45,46,47};//freqOffset for resource mapping

int get_num_subcarrier_perRU(int get_startFreqPos)
{
	if(get_startFreqPos==resourceAllocPattern1[4])	return 6;
	else if(get_startFreqPos==resourceAllocPattern1[3])	return 3;
	else if(get_startFreqPos==resourceAllocPattern1[0]||get_startFreqPos==resourceAllocPattern1[1]||get_startFreqPos==resourceAllocPattern1[2])	return 1;

	return -1;
}

int get_startFreqPos(int ** UL_Channle_Struc,int subframe,int multiToneSupport)
{
	int FreqPos=-1;
	if(multiToneSupport==1||multiToneSupport==2)
	{
		for (int j = resourceAllocPattern1[3]; j < resourceAllocPattern1[3]+36; ++j)//12<=j<48, don't use singleTone resource
		{
			if(UL_Channle_Struc[j][subframe]==0)
			{
				FreqPos=j;
				break;
			}
		}
		// LOG("startTime%d,FreqPos:%d\n",startTime,FreqPos);
	}
	if(multiToneSupport==6)
	{
		for (int j = resourceAllocPattern1[4]; j < resourceAllocPattern1[4]+24; ++j)//12<=j<48, don't use singleTone resource
		{
			if(UL_Channle_Struc[j][subframe]==0)
			{
				FreqPos=j;
				break;
			}
		}
		// LOG("startTime%d,FreqPos:%d\n",startTime,FreqPos);
	}
	else if(multiToneSupport==3)
	{
		for (int j = resourceAllocPattern1[3]; j < resourceAllocPattern1[3]+12; ++j)//12<=j<48, don't use singleTone resource
		{
			if(UL_Channle_Struc[j][subframe]==0)
			{
				FreqPos=j;
				break;
			}
		}
		// LOG("startTime%d,FreqPos:%d\n",startTime,FreqPos);
	}
	else if(multiToneSupport==0)
	{
		for (int j = resourceAllocPattern1[0]; j < resourceAllocPattern1[0]+12; ++j)
		{
			if(UL_Channle_Struc[j][subframe]==0)
			{
				FreqPos=j;
				break;
			}
		}
		// LOG("startTime%d,FreqPos:%d\n",startTime,FreqPos);
	}
	// else if(multiToneSupport==4)
	// {
	// 	for (int j = resourceAllocPattern1[1]; j < resourceAllocPattern1[1]+4; ++j)
	// 	{
	// 		if(UL_Channle_Struc[multiToneSupport][subframe]==0)
	// 		{
	// 			FreqPos=j;
	// 			break;
	// 		}
	// 	}
	// }
	// else if(multiToneSupport==8)
	// {
	// 	for (int j = resourceAllocPattern1[2]; j < resourceAllocPattern1[2]+4; ++j)
	// 	{
	// 		if(UL_Channle_Struc[multiToneSupport][subframe]==0)
	// 		{
	// 			FreqPos=j;
	// 			break;
	// 		}
	// 	}
	// }
	return FreqPos;
}

int resourceAllocate(UE_info &UL_Sche_UE)
{
	int RU_unit=1;
	//First Round to calculate RU required by each UEs
	int num_RU=RU_unit;
	int TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,num_RU);
	// LOG("InitialTBS:%d\n",TBS);
	if(TBS==-1) return -1;
	while(TBS<UL_Sche_UE.UL_Buffer_Sizes)
	{
		num_RU=num_RU+RU_unit;
		if(UL_Sche_UE.mcs<=6&&num_RU>10)
		{
			TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,10);
			num_RU=10;
			// UL_Sche_UE.remaining_Buffer_Sizes=UL_Sche_UE.UL_Buffer_Sizes-TBS;
			// UL_Sche_UE.round++;
			break;
		}
		else if(UL_Sche_UE.mcs==7&&num_RU>8)
		{
			TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,8);
			num_RU=8;
			// UL_Sche_UE.remaining_Buffer_Sizes=UL_Sche_UE.UL_Buffer_Sizes-TBS;
			// UL_Sche_UE.round++;
			break;
		}
		else if(UL_Sche_UE.mcs==8&&num_RU>6)
		{
			TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,6);
			num_RU=6;
			// UL_Sche_UE.remaining_Buffer_Sizes=UL_Sche_UE.UL_Buffer_Sizes-TBS;
			// UL_Sche_UE.round++;
			break;
		}
		else if(UL_Sche_UE.mcs==9&&num_RU>6)
		{
			TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,6);
			num_RU=6;
			// UL_Sche_UE.remaining_Buffer_Sizes=UL_Sche_UE.UL_Buffer_Sizes-TBS;
			// UL_Sche_UE.round++;
			break;
		}
		else if(UL_Sche_UE.mcs==10&&num_RU>6)
		{
			TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,6);
			num_RU=6;
			// UL_Sche_UE.remaining_Buffer_Sizes=UL_Sche_UE.UL_Buffer_Sizes-TBS;
			// UL_Sche_UE.round++;
			break;
		}
		TBS=get_TBS_UL(UL_Sche_UE.mcs,UL_Sche_UE.multi_tone,num_RU);
		// LOG("TBS:%d\n",TBS);
		// system("pause");
	}
	UL_Sche_UE.remaining_Buffer_Sizes=UL_Sche_UE.UL_Buffer_Sizes-TBS;
	UL_Sche_UE.UE_num_RU=num_RU;
	UL_Sche_UE.allocate_Buffer_Sizes=TBS;
	return 0;
}

int get_I_TBS(int x,int y)
{
	int I_TBS;
	if(y==0) I_TBS=x;
	else
	{
		if(x==1)	I_TBS=2;
		else if(x==2)	I_TBS=1;
		else
		{
			I_TBS=x;
		}
	}
	return I_TBS;
}
int get_I_RU(int x)
{
	int I_RU;
	if(x<=6)	I_RU=x-1;
	else if(x==8)	I_RU=x-2;
	else if(x==10)	I_RU=x-3;
	else return -1;
	return I_RU;
}
int get_TBS_UL(int mcs,int multi_tone,int num_RU)
{
	int TBS,check_I_RU;
	check_I_RU=get_I_RU(num_RU);
	if(check_I_RU==-1)	return -1;
	TBS=UL_TBS_Table[get_I_TBS(mcs,multi_tone)][check_I_RU];
	// LOG("%d\n",TBS);
	return TBS>>3;
}

int transfer_MCS(UE_info &UL_Sche_UE)
{
	if(UL_Sche_UE.PHR==0)
		UL_Sche_UE.mcs=rand()%2;
	else if(UL_Sche_UE.PHR==1)
		UL_Sche_UE.mcs=2+rand()%4;
	else if(UL_Sche_UE.PHR==2)
		UL_Sche_UE.mcs=6+rand()%4;
	else if(UL_Sche_UE.PHR==3)
		UL_Sche_UE.mcs=10;
	return 0;
}

int transfer_PHR(UE_info &UL_Sche_UE, int measuredpuschpower)
{
	if(UL_Sche_UE.CE_Level==0)
	{
		if(-23<=measuredpuschpower&&measuredpuschpower<5)	return 0;
		else if(5<=measuredpuschpower&&measuredpuschpower<8)	return 1;
		else if(8<=measuredpuschpower&&measuredpuschpower<11)	return 2;
		else if(measuredpuschpower>=11)	return 3;
		else	return -1;
	}
	else
	{
		if(-23<=measuredpuschpower&&measuredpuschpower<-10)	return 0;
		else if(-10<=measuredpuschpower&&measuredpuschpower<-2)	return 1;
		else if(-2<=measuredpuschpower&&measuredpuschpower<6)	return 2;
		else if(measuredpuschpower>=6)	return 3;
		else	return -1;
	}
}

int transfer_DV(UE_info &UL_Sche_UE)
{
	int temp = UL_Sche_UE.UL_Buffer_Sizes;
	// cout<<temp<<endl;
	UL_Sche_UE.DV=-1;
	if(temp==0)
	{
		UL_Sche_UE.DV=0;
	}
	else if(0 < temp && temp <= 10)
	{
		UL_Sche_UE.DV=1;
	}
	else if(10 < temp && temp <= 14)
	{
		UL_Sche_UE.DV=2;
	}
	else if(14 < temp && temp <= 19)
	{
		UL_Sche_UE.DV=3;
	}
	else if(19 < temp && temp <= 26)
	{
		UL_Sche_UE.DV=4;
	}
	else if(26 < temp && temp <= 36)
	{
		UL_Sche_UE.DV=5;
	}
	else if(36 < temp && temp <= 49)
	{
		UL_Sche_UE.DV=6;
	}
	else if(49 < temp && temp <= 67)
	{
		UL_Sche_UE.DV=7;
	}
	else if(67 < temp && temp <= 91)
	{
		UL_Sche_UE.DV=8;
	}
	else if(91 < temp && temp <= 125)
	{
		UL_Sche_UE.DV=9;
	}
	else if(125 < temp && temp <= 171)
	{
		UL_Sche_UE.DV=10;
	}
	else if(171 < temp && temp <= 234)
	{
		UL_Sche_UE.DV=11;
	}
	else if(234 < temp && temp <= 321)
	{
		UL_Sche_UE.DV=12;
	}
	else if(321 < temp && temp <= 768)
	{
		UL_Sche_UE.DV=13;
	}
	else if(768 < temp && temp <= 1500)
	{
		UL_Sche_UE.DV=14;
	}
	else if(temp > 1500)
	{
		UL_Sche_UE.DV=15;
	}
	// cout<<UL_Sche_UE.DV<<endl;
	// system("pause");
	if(UL_Sche_UE.DV==-1)	return -1;
	else	return 0;
	// return DV;
}
int get_UL_Data_Bytes(int DV)
{
	int UL_Data_Bytes=0;
	switch(DV)
	{
		case 0:
			UL_Data_Bytes=0;
			break;
		case 1:
			UL_Data_Bytes=rand()%10+1;//0 < DV <= 10
			break;
		case 2:
			UL_Data_Bytes=11+rand()%4;//10 < DV <= 14
			break;
		case 3:
			UL_Data_Bytes=15+rand()%5;//14 < DV <= 19
			break;
		case 4:
			UL_Data_Bytes=20+rand()%7;//19 < DV <= 26
			break;
		case 5:
			UL_Data_Bytes=27+rand()%10;//26 < DV <= 36
			break;
		case 6:
			UL_Data_Bytes=37+rand()%13;//36 < DV <= 49
			break;
		case 7:
			UL_Data_Bytes=50+rand()%18;//49 < DV <= 67
			break;
		case 8:
			UL_Data_Bytes=68+rand()%24;//67 < DV <= 91
			break;
		case 9:
			UL_Data_Bytes=92+rand()%34;//91 < DV <= 125
			break;
		case 10:
			UL_Data_Bytes=126+rand()%46;//125 < DV <= 171
			break;
		case 11:
			UL_Data_Bytes=172+rand()%63;//171 < DV <= 234
			break;
		case 12:
			UL_Data_Bytes=235+rand()%87;//234 < DV <= 321
			break;
		case 13:
			UL_Data_Bytes=322+rand()%447;//321 < DV <= 768
			break;
		case 14:
			UL_Data_Bytes=268+rand()%733;//768 < DV <= 1500-->fixed to no more 1000
			break;
		case 15:
			UL_Data_Bytes=1501;//DV > 1500
			break;
		default:
			break;
	}
	return UL_Data_Bytes;
}
int Set_NPRACH_Resource(double **indicateSIB, NB_SIB2 &NB_SIB2_Struc, int number_of_target_SNR)
{
	NB_SIB2_Struc.I_Wait_to_NPDCCH=4;//at least 3ms from system model
	NB_SIB2_Struc.I_NPDCCH_RAR=13;//scheduling delay, could modify
	NB_SIB2_Struc.I_RAR_Msg3=13;//13ms,could modify
	NB_SIB2_Struc.I_Msg3_DCIN0=8;//8ms,could modify
	double OffsetSNR[3]={0,10,20};
	for (int i = 0; i < number_of_target_SNR; ++i)
	{
		NB_SIB2_Struc.target_SNR[i]=14.25-OffsetSNR[i];//14.25 dB, 4.25 dB, -5.75 dB
		// LOG("%d,%lf\n",number_of_target_SNR,NB_SIB2_Struc.target_SNR[i]);
		// system("pause");
		if(NB_SIB2_Struc.target_SNR[i]==14.25)//MCL:144 DB
		{
			NB_SIB2_Struc.CE_Level[i]=0;
			NB_SIB2_Struc.rep[i]=indicateSIB[0][1]; //1.4*4=5.6ms
			NB_SIB2_Struc.period[i]=indicateSIB[0][0];
			NB_SIB2_Struc.start_time[i]=indicateSIB[0][5];//offset from start of NPRACH period
			NB_SIB2_Struc.num_Subcarrier[i]=12;
			NB_SIB2_Struc.subcarrier_Offset[i]=0;
			NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]=indicateSIB[0][2];
			NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]=indicateSIB[0][3];
			NB_SIB2_Struc.npdcch_Offset_RA[i]=0.375;
			NB_SIB2_Struc.rawindow[i]=indicateSIB[0][4];
			NB_SIB2_Struc.candidate[i]=indicateSIB[0][6];
		}
		else if(NB_SIB2_Struc.target_SNR[i]==4.25)//MCL:154 DB
		{
			NB_SIB2_Struc.CE_Level[i]=1;
			NB_SIB2_Struc.rep[i]=indicateSIB[1][1]; //16 ms
			NB_SIB2_Struc.period[i]=indicateSIB[1][0];
			NB_SIB2_Struc.start_time[i]=indicateSIB[1][5];
			NB_SIB2_Struc.num_Subcarrier[i]=12;
			NB_SIB2_Struc.subcarrier_Offset[i]=12;
			NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]=indicateSIB[1][2];
			NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]=indicateSIB[1][3];
			NB_SIB2_Struc.npdcch_Offset_RA[i]=0.375;
			NB_SIB2_Struc.rawindow[i]=indicateSIB[1][4];
			NB_SIB2_Struc.candidate[i]=indicateSIB[1][6];
		}
		else if(NB_SIB2_Struc.target_SNR[i]==-5.75)//MCL:164 DB
		{
			NB_SIB2_Struc.CE_Level[i]=2;
			NB_SIB2_Struc.rep[i]=indicateSIB[2][1];	//32 ms
			NB_SIB2_Struc.period[i]=indicateSIB[2][0];
			NB_SIB2_Struc.start_time[i]=indicateSIB[2][5];
			NB_SIB2_Struc.num_Subcarrier[i]=24;
			NB_SIB2_Struc.subcarrier_Offset[i]=24;
			NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]=indicateSIB[2][2];
			NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]=indicateSIB[2][3];
			NB_SIB2_Struc.npdcch_Offset_RA[i]=0.375;
			NB_SIB2_Struc.rawindow[i]=indicateSIB[2][4];
			NB_SIB2_Struc.candidate[i]=indicateSIB[2][6];
		}
		else
			return -1;
	}
	checkFrequencyDomain(NB_SIB2_Struc);
	cout<<"NPRACH Config CE(0,1,2) Pass FrequencyDomain limitation"<<endl;
	system("pause");
	return 0;
}

int checkFrequencyDomain(NB_SIB2 &NB_SIB2_Struc)
{
	int checkFreqArray[48]={0};
	int errorReason=-1;
	for (int i = 0; i < 3; ++i)
	{
		if(i==0)
		{
			for (int j = NB_SIB2_Struc.subcarrier_Offset[i]; j < NB_SIB2_Struc.subcarrier_Offset[i]+NB_SIB2_Struc.num_Subcarrier[i]; ++j)
			{
				if(checkFreqArray[j]==0)	checkFreqArray[j]=1;
				else	errorReason=0;
			}
		}
		else if(i==1)
		{
			for (int j = NB_SIB2_Struc.subcarrier_Offset[i]; j < NB_SIB2_Struc.subcarrier_Offset[i]+NB_SIB2_Struc.num_Subcarrier[i]; ++j)
			{
				if(checkFreqArray[j]==0)	checkFreqArray[j]=2;
				else	errorReason=1;
			}
		}
		else if(i==2)
		{
			for (int j = NB_SIB2_Struc.subcarrier_Offset[i]; j < NB_SIB2_Struc.subcarrier_Offset[i]+NB_SIB2_Struc.num_Subcarrier[i]; ++j)
			{
				if(checkFreqArray[j]==0)	checkFreqArray[j]=3;
				else	errorReason=2;
			}
		}
		if(errorReason!=-1)
		{
			cout << "Error: the NPRACH-Config(CE"<<errorReason<<") exceed FreqDomain resource!!" << endl;
			system("pause");
			exit(1);
		}
	}
	return 0;
}

int Auto_Filter_NB_SIB2Config(double **passSetting, double **indicateSIB)
{
	int rachperiod[8]={40,80,160,240,320,640,1280,2560};
	int rachstart[8]={8,16,32,64,128,256,512,1024};
	int rachrepeat[8]={1,2,4,8,16,32,64,128};
	int rawindow[8]={2,3,4,5,6,7,8,10};// Not include in MAC_Command in shell script
	int rmax[12]={1,2,4,8,16,32,64,128,256,512,1024,2048};
	double gvalue[8]={1.5,2,4,8,16,32,48,64};
	int candidate[4]={1,2,4,8};

	/*Not include shell script*/
	double pdcchoffset[4]={0,0.125,0.25,0.375};// Not include in MAC_Command in shell script

	/*Not include shell script*/
	int dlcqi[13]={0,1,2,3,4,5,6,7,8,9,10,11,12};
	int dlrepeat[16]={1,2,4,8,16,32,64,128,192,256,384,512,768,1024,1536,2048};
	int rachscofst[7]={0,12,24,36,2,18,34};
	int rachnumsc[4]={12,24,36,48};

	//limitCondition1
	int L_rachperiod=sizeof(rachperiod)/sizeof(int);
	int L_rawindow=sizeof(rawindow)/sizeof(int);
	int L_rmax=sizeof(rmax)/sizeof(int);
	double L_gvalue=sizeof(gvalue)/sizeof(double);
	int L_rachrepeat=sizeof(rachrepeat)/sizeof(int);

	//limitCondition2
	int L_candidate=sizeof(candidate)/sizeof(int);
	int L_rachstart=sizeof(rachstart)/sizeof(int);

	//limitCondition3
	int L_dlcqi=sizeof(dlcqi)/sizeof(int);
	int L_dlrepeat=sizeof(dlrepeat)/sizeof(int);

	double limitCondition1[12000][3];
	int cnt=0;
	/* III limitation condition
	for (int i = 0; i < L_rachperiod; ++i)
	{
		for (int j = 0; j < L_rachrepeat; ++j)
			for (int k = 0; k < L_rmax; ++k)
				for (int l = 0; l < L_gvalue; ++l)
					for (int m = 0; m < L_rawindow; ++m)
					{
						if(rachperiod[i]>1.4 * 4 * rachrepeat[j]+3+rmax[k] * gvalue[l] * rawindow[m])
						// if(rachperiod[i]>1.4 * 4 * rachrepeat[j])
						{
							limitCondition1[cnt][0]=rachperiod[i];
							limitCondition1[cnt][1]=rachrepeat[j];
							limitCondition1[cnt][2]=rmax[k];
							limitCondition1[cnt][3]=gvalue[l];
							limitCondition1[cnt][4]=rawindow[m];
							// cout << limitCondition1[cnt][3] <<endl;
							// cout << limitCondition1[cnt][0] <<endl;
							++cnt;
						}
					}
	}*/
	// cout << "cnt: " << cnt << endl;
	for (int i = 0; i < L_rachperiod; ++i)
	{
		for (int j = 0; j < L_rachrepeat; ++j)
		{
			for (int k = 0; k < L_rmax; ++k)
			{
				limitCondition1[cnt][0]=rachperiod[i];
				limitCondition1[cnt][1]=rachrepeat[j];
				limitCondition1[cnt][2]=rmax[k];
				++cnt;
			}

		}
	}
	// cout << "cnt: " << cnt << endl;
	// system("pause");
	// double **passSetting;
	// double **indicateSIB;
	// int Ans=number_of_target_SNR;
	// int size=400000;
	// passSetting = new double*[size];
	// indicateSIB = new double*[Ans];
	cout << "Filter Running: \n"<< endl;
	int setdone=0;
	// for (int i = 0; i < size; ++i)	passSetting[i] = new double [7];
	// for (int i = 0; i < number_of_target_SNR; ++i)	indicateSIB[i] = new double [7];
	int cnt1=0;
	for (int i = 0; i < cnt; ++i)
	{
		for (int j = 0; j < L_rachstart; ++j)
			for (int k = 0; k < L_candidate; ++k)
				if(limitCondition1[i][0]>rachstart[j]&&candidate[k]<=limitCondition1[i][2])
					{
						if(setdone==3)	return 1;
						passSetting[cnt1][0]=limitCondition1[i][0];
						passSetting[cnt1][1]=limitCondition1[i][1];
						passSetting[cnt1][2]=limitCondition1[i][2];
						// passSetting[cnt1][3]=limitCondition1[i][3];
						// passSetting[cnt1][4]=limitCondition1[i][4];
						passSetting[cnt1][5]=rachstart[j];
						passSetting[cnt1][6]=candidate[k];
						// if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==1&&passSetting[cnt1][2]==2&&passSetting[cnt1][3]==2&&passSetting[cnt1][4]==2&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==2)
						if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==1&&passSetting[cnt1][2]==2&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==2)
						{
							indicateSIB[0][0]=passSetting[cnt1][0];
							indicateSIB[0][1]=passSetting[cnt1][1];
							indicateSIB[0][2]=passSetting[cnt1][2];
							// indicateSIB[0][3]=passSetting[cnt1][3];
							// indicateSIB[0][4]=passSetting[cnt1][4];
							indicateSIB[0][3]=2;
							indicateSIB[0][4]=2;
							indicateSIB[0][5]=passSetting[cnt1][5];
							indicateSIB[0][6]=passSetting[cnt1][6];
							setdone++;
							cout<<indicateSIB[0][0]<<','<<indicateSIB[0][1]<<','<<indicateSIB[0][2]<<','<<indicateSIB[0][3]<<','<<indicateSIB[0][4]<<','<<indicateSIB[0][5]<<','<<indicateSIB[0][6]<<endl;
							getchar();
						}
						// else if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==2&&passSetting[cnt1][2]==4&&passSetting[cnt1][3]==2&&passSetting[cnt1][4]==2&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==4)
						else if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==2&&passSetting[cnt1][2]==4&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==4)
						{
							indicateSIB[1][0]=passSetting[cnt1][0];
							indicateSIB[1][1]=passSetting[cnt1][1];
							indicateSIB[1][2]=passSetting[cnt1][2];
							// indicateSIB[1][3]=passSetting[cnt1][3];
							// indicateSIB[1][4]=passSetting[cnt1][4];
							indicateSIB[1][3]=2;
							indicateSIB[1][4]=2;
							indicateSIB[1][5]=passSetting[cnt1][5];
							indicateSIB[1][6]=passSetting[cnt1][6];
							setdone++;
							cout<<indicateSIB[1][0]<<','<<indicateSIB[1][1]<<','<<indicateSIB[1][2]<<','<<indicateSIB[1][3]<<','<<indicateSIB[1][4]<<','<<indicateSIB[1][5]<<','<<indicateSIB[1][6]<<endl;
							getchar();
						}
						// else if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==4&&passSetting[cnt1][2]==8&&passSetting[cnt1][3]==2&&passSetting[cnt1][4]==2&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==8)
						else if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==4&&passSetting[cnt1][2]==8&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==8)
						{
							indicateSIB[2][0]=passSetting[cnt1][0];
							indicateSIB[2][1]=passSetting[cnt1][1];
							indicateSIB[2][2]=passSetting[cnt1][2];
							// indicateSIB[2][3]=passSetting[cnt1][3];
							// indicateSIB[2][4]=passSetting[cnt1][4];
							indicateSIB[2][3]=2;
							indicateSIB[2][4]=2;
							indicateSIB[2][5]=passSetting[cnt1][5];
							indicateSIB[2][6]=passSetting[cnt1][6];
							setdone++;
							cout<<indicateSIB[2][0]<<','<<indicateSIB[2][1]<<','<<indicateSIB[2][2]<<','<<indicateSIB[2][3]<<','<<indicateSIB[2][4]<<','<<indicateSIB[2][5]<<','<<indicateSIB[2][6]<<endl;
							getchar();
						}
						// cout<<passSetting[cnt1][0]<<','<<passSetting[cnt1][1]<<','<<passSetting[cnt1][2]<<','<<passSetting[cnt1][3]<<','<<passSetting[cnt1][4]<<','<<passSetting[cnt1][5]<<','<<passSetting[cnt1][6]<<endl;
						// getchar();
						++cnt1;
					}
	}
	cout << "# of SIB2 Config Pass limitation : " << cnt1 << endl;
	return 0;
}
