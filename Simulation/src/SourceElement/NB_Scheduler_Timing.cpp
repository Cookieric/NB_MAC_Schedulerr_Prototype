#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <list>
#include "testArrivalModel.cpp"
using namespace std;

#define LOG(...) printf(__VA_ARGS__)




typedef struct _NB_SIB2{
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
typedef struct _schedulingInfo_NB
{
	int si_periodicity_r13;
	int si_repetiitonPattern_r13;
	// int sib_mappingInfo_r13; //Not used for SIB2
	int si_TB_r13;
	int SIB2_Nsf;
} schedulingInfo_NB;

typedef struct _NB_SIB1
{
	int subframe_SIB1;
	int si_WindowSize;
	schedulingInfo_NB sche_SIB2;
} NB_SIB1;

//Msg4-->RadioResourceConfigDedicated-NB-r13-->physicalConfigDedicated-r13-->npdcch-ConfigDedicated-r13
typedef struct _npdcch_ConfigDedicated_r13{
	int npdcch_NumRepetitions;
	double npdcch_StartSF_USS;
	double npdcch_Offset_USS;
} npdcch_ConfigDedicated_r13;

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

typedef struct _defaultInvalidSubfrmae{
	int subframe_NPSS;
	int subframe_NSSS;
	int subframe_NPBCH;
} defaultInvalidSubfrmae;

typedef struct _AnchorCarrierChannel
{
	int **UL_Channel_Struc;
	int **DL_Channel_Struc;
} AnchorCarrierChannel;


int InitialTimerConfig(NB_SIB1 &,defaultInvalidSubfrmae &);

int Auto_Filter_NB_SIB2Config(double **, double **);
int Set_NPRACH_Resource(double **, NB_SIB2 &, int);
int checkFrequencyDomain(NB_SIB2 &);


int generate_PBCH(int, int **);
int nprachResourceMapping(int,NB_SIB2 &,int **);




int main(int argc, char const *argv[])
{
	srand(time(NULL));
	int number_of_target_SNR=3;

	double **passSetting;
	double **indicateSIB;
	defaultInvalidSubfrmae defaultInvalidSubfrmae_Struc={0};//Directly Initialize
	NB_SIB1 NB_SIB1_Struc={0};
	DCI DCIFormat= {0};
	AnchorCarrierChannel AnchorCarrier_Struc={0};
	InitialTimerConfig(NB_SIB1_Struc,defaultInvalidSubfrmae_Struc);

	NB_SIB2 NB_SIB2_Struc= {0};
	npdcch_ConfigDedicated_r13 dedicatedNPDCCH={0};

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
	//Include setting common NPDCCH configuration in Set_NPRACH_Resource.
	if(Set_NPRACH_Resource(indicateSIB, NB_SIB2_Struc, number_of_target_SNR)==-1)
	{
		LOG("No NPRACH Configuration: BS Failed!\n");
		return 0;
	}
	LOG("Setting done for three CE levels NPRACH Parameters\n");

	delete []passSetting;
	delete []indicateSIB;

	int period_NPRACH=NB_SIB2_Struc.period[0];
	for (int i = 0; i < 2; ++i)
	{
		if(period_NPRACH<NB_SIB2_Struc.period[i+1])	period_NPRACH=NB_SIB2_Struc.period[i+1];
	}
	cout<<"MAX period_NPRACH: "<<period_NPRACH<<endl;//1280ms
	system("pause");

	int num_UL_Subcarrier=48;
	int num_DL_subcarrier=12;

	//Initial done, next are scheduling timing parts
	int hyperSFN=0,frame=0,subframe=0;//hyperSFN:10 bits,frame:10 bits
	char Fun;
	while(1)
	{
		if(subframe==10)
		{
			subframe=0;
			++frame;
			if(frame%1024==0)
			{
				frame=0;
				++hyperSFN;
			}
		}
		if((frame * 10+subframe)%period_NPRACH==0)
		{
		    cout<<"To be continue, plz enter 1 or others to exit";
		    Fun = getchar();
		    if(Fun!='1')	exit(0);

			AnchorCarrier_Struc.UL_Channel_Struc = new int*[num_UL_Subcarrier];
			for (int i = 0; i < num_UL_Subcarrier; ++i)
				AnchorCarrier_Struc.UL_Channel_Struc[i] = new int [period_NPRACH]();//( ) to intialize array to 0

			AnchorCarrier_Struc.DL_Channel_Struc= new int*[num_DL_subcarrier];
			for (int i = 0; i < num_DL_subcarrier; ++i)
				AnchorCarrier_Struc.UL_Channel_Struc[i] = new int [period_NPRACH]();

			nprachResourceMapping(number_of_target_SNR,NB_SIB2_Struc,AnchorCarrier_Struc.UL_Channel_Struc);
			cout<<"nprachResourceMapping done"<<endl;
			system("pause");
		}
		if(subframe==0)	generate_PBCH(frame,AnchorCarrier_Struc.DL_Channel_Struc);
		subframe++;
	}
	for (int i = 0; i < period_NPRACH; ++i)
	{
		for (int j = 0; j < 48; ++j)
		{
			cout<<AnchorCarrier_Struc.UL_Channel_Struc[j][i]<<' ';
		}
		cout<<endl;
	}

	system("pause");
	return 0;

}

int generate_PBCH(int frame, int **DL_Channel_Struc)
{
	// for (int i = 0; i < 12; ++i)
	// {
	// 	DL_Channel_Struc[]
	// }
}

int InitialTimerConfig(NB_SIB1 &NB_SIB1_Struc,defaultInvalidSubfrmae &defaultInvalidSubfrmae_Struc)
{
	defaultInvalidSubfrmae_Struc.subframe_NPSS=5;
	defaultInvalidSubfrmae_Struc.subframe_NSSS=9;
	defaultInvalidSubfrmae_Struc.subframe_NPBCH=0;
	NB_SIB1_Struc.subframe_SIB1=4;
	NB_SIB1_Struc.si_WindowSize=160;
	NB_SIB1_Struc.sche_SIB2.si_periodicity_r13=640;//rf64
	NB_SIB1_Struc.sche_SIB2.si_repetiitonPattern_r13=2;
	NB_SIB1_Struc.sche_SIB2.si_TB_r13=120;
	NB_SIB1_Struc.sche_SIB2.SIB2_Nsf=2;//120 bits(TBS) Itbs(4) to Isf(1)=Nsf = 2
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
			NB_SIB2_Struc.num_Subcarrier[i]=48;
			NB_SIB2_Struc.subcarrier_Offset[i]=0;
			NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]=indicateSIB[0][2];
			NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]=indicateSIB[0][3];
			NB_SIB2_Struc.npdcch_Offset_RA[i]=0;
			NB_SIB2_Struc.rawindow[i]=indicateSIB[0][4];
			NB_SIB2_Struc.candidate[i]=indicateSIB[0][6];
				cout<<"CE_Level 0: "<<NB_SIB2_Struc.period[i]<<','<<NB_SIB2_Struc.rep[i]<<','<<NB_SIB2_Struc.start_time[i]<<','<<NB_SIB2_Struc.num_Subcarrier[i]<<','<<NB_SIB2_Struc.subcarrier_Offset[i]<<','<<NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]<<','<<NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]<<','<<NB_SIB2_Struc.npdcch_Offset_RA[i]<<','<<NB_SIB2_Struc.rawindow[i]<<endl;
		}
		else if(NB_SIB2_Struc.target_SNR[i]==4.25)//MCL:154 DB
		{
			NB_SIB2_Struc.CE_Level[i]=1;
			NB_SIB2_Struc.rep[i]=indicateSIB[1][1]; //16 ms
			NB_SIB2_Struc.period[i]=indicateSIB[1][0];
			NB_SIB2_Struc.start_time[i]=indicateSIB[1][5];
			NB_SIB2_Struc.num_Subcarrier[i]=48;
			NB_SIB2_Struc.subcarrier_Offset[i]=0;
			NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]=indicateSIB[1][2];
			NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]=indicateSIB[1][3];
			NB_SIB2_Struc.npdcch_Offset_RA[i]=0.125;
			NB_SIB2_Struc.rawindow[i]=indicateSIB[1][4];
			NB_SIB2_Struc.candidate[i]=indicateSIB[1][6];
			cout<<"CE_Level 1: "<<NB_SIB2_Struc.period[i]<<','<<NB_SIB2_Struc.rep[i]<<','<<NB_SIB2_Struc.start_time[i]<<','<<NB_SIB2_Struc.num_Subcarrier[i]<<','<<NB_SIB2_Struc.subcarrier_Offset[i]<<','<<NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]<<','<<NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]<<','<<NB_SIB2_Struc.npdcch_Offset_RA[i]<<','<<NB_SIB2_Struc.rawindow[i]<<endl;
		}
		else if(NB_SIB2_Struc.target_SNR[i]==-5.75)//MCL:164 DB
		{
			NB_SIB2_Struc.CE_Level[i]=2;
			NB_SIB2_Struc.rep[i]=indicateSIB[2][1];	//32 ms
			NB_SIB2_Struc.period[i]=indicateSIB[2][0];
			NB_SIB2_Struc.start_time[i]=indicateSIB[2][5];
			NB_SIB2_Struc.num_Subcarrier[i]=48;
			NB_SIB2_Struc.subcarrier_Offset[i]=0;
			NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]=indicateSIB[2][2];
			NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]=indicateSIB[2][3];
			NB_SIB2_Struc.npdcch_Offset_RA[i]=0.375;
			NB_SIB2_Struc.rawindow[i]=indicateSIB[2][4];
			NB_SIB2_Struc.candidate[i]=indicateSIB[2][6];
			cout<<"CE_Level 2: "<<NB_SIB2_Struc.period[i]<<','<<NB_SIB2_Struc.rep[i]<<','<<NB_SIB2_Struc.start_time[i]<<','<<NB_SIB2_Struc.num_Subcarrier[i]<<','<<NB_SIB2_Struc.subcarrier_Offset[i]<<','<<NB_SIB2_Struc.npdcch_NumRepetitions_RA[i]<<','<<NB_SIB2_Struc.npdcch_StartSF_CSS_RA[i]<<','<<NB_SIB2_Struc.npdcch_Offset_RA[i]<<','<<NB_SIB2_Struc.rawindow[i]<<endl;
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
	if(NB_SIB2_Struc.start_time[0]+NB_SIB2_Struc.rep[0] * 5.6<NB_SIB2_Struc.start_time[1]+NB_SIB2_Struc.rep[1] * 5.6)
	{
		if(NB_SIB2_Struc.start_time[1]+NB_SIB2_Struc.rep[1] * 5.6<NB_SIB2_Struc.start_time[2]+NB_SIB2_Struc.rep[2] * 5.6)
		{
			return 0;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		if(i==0)
		{
			int duration=ceil(NB_SIB2_Struc.rep[i] * 1.4 * 4);
			for (int j = NB_SIB2_Struc.start_time[i]; j < NB_SIB2_Struc.start_time[i]+duration; ++j)
			{
				for (int k = NB_SIB2_Struc.subcarrier_Offset[i]; k < NB_SIB2_Struc.subcarrier_Offset[i]+NB_SIB2_Struc.num_Subcarrier[i]; ++k)
				{

					// else	errorReason=0;
				}
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
	// int cnt=0;
	 // III limitation condition
	// for (int i = 0; i < L_rachperiod; ++i)
	// {
	// 	for (int j = 0; j < L_rachrepeat; ++j)
	// 		for (int k = 0; k < L_rmax; ++k)
	// 			for (int l = 0; l < L_gvalue; ++l)
	// 				for (int m = 0; m < L_rawindow; ++m)
	// 				{
	// 					if(rachperiod[i]>1.4 * 4 * rachrepeat[j]+3+rmax[k] * gvalue[l] * rawindow[m])
	// 					// if(rachperiod[i]>1.4 * 4 * rachrepeat[j])
	// 					{
	// 						limitCondition1[cnt][0]=rachperiod[i];
	// 						limitCondition1[cnt][1]=rachrepeat[j];
	// 						limitCondition1[cnt][2]=rmax[k];
	// 						limitCondition1[cnt][3]=gvalue[l];
	// 						limitCondition1[cnt][4]=rawindow[m];
	// 						// cout << limitCondition1[cnt][3] <<endl;
	// 						// cout << limitCondition1[cnt][0] <<endl;
	// 						++cnt;
	// 					}
	// 				}
	// }
	// cout << "cnt: " << cnt << endl;
	double limitCondition1[1000][3];
	cout << "Filter Running: \n"<< endl;
	int setdone=0, cnt=0;
	for (int i = 0; i < L_rachperiod; ++i)
		for (int j = 0; j < L_rachrepeat; ++j)
			for (int k = 0; k < L_rachstart; ++k)
				if(rachperiod[i]>rachstart[k]&&rachperiod[i]>1.4 * 4 * rachrepeat[j])
				{
					limitCondition1[cnt][0]=rachperiod[i];
					limitCondition1[cnt][1]=rachrepeat[k];
					limitCondition1[cnt][2]=rachstart[j];
					// cout<<limitCondition1[cnt][0]<<','<<limitCondition1[cnt][1]<<','<<limitCondition1[cnt][2]<<endl;
					++cnt;
				}
	// cout << "cnt: " << cnt << endl;
	system("pause");
	int cnt1=0;
	for (int j = 0; j < L_rmax; ++j)
		for (int k = 0; k < L_candidate; ++k)
			if(candidate[k]<=rmax[j])
			{
				for (int i = 0; i < cnt; ++i)
				{
					if(setdone==3)	return 1;
					passSetting[cnt1][0]=limitCondition1[i][0];
					passSetting[cnt1][1]=limitCondition1[i][1];
					passSetting[cnt1][2]=limitCondition1[i][2];
					// passSetting[cnt1][3]=limitCondition1[i][3];
					// passSetting[cnt1][4]=limitCondition1[i][4];
					passSetting[cnt1][5]=rmax[j];
					passSetting[cnt1][6]=candidate[k];
					if(passSetting[cnt1][0]==1280&&passSetting[cnt1][1]==1&&passSetting[cnt1][2]==8&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==8)
					{
						indicateSIB[0][0]=passSetting[cnt1][0];
						indicateSIB[0][1]=passSetting[cnt1][1];
						indicateSIB[0][2]=passSetting[cnt1][2];
						// indicateSIB[0][3]=passSetting[cnt1][3];
						// indicateSIB[0][4]=passSetting[cnt1][4];
						indicateSIB[0][3]=4;//G
						indicateSIB[0][4]=2;//rar-window
						indicateSIB[0][5]=passSetting[cnt1][5];
						indicateSIB[0][6]=passSetting[cnt1][6];
						setdone++;
						// cout<<indicateSIB[0][0]<<','<<indicateSIB[0][1]<<','<<indicateSIB[0][2]<<','<<indicateSIB[0][3]<<','<<indicateSIB[0][4]<<','<<indicateSIB[0][5]<<','<<indicateSIB[0][6]<<endl;
						// getchar();
					}
					// else if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==2&&passSetting[cnt1][2]==4&&passSetting[cnt1][3]==2&&passSetting[cnt1][4]==2&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==4)
					else if(passSetting[cnt1][0]==1280&&passSetting[cnt1][1]==2&&passSetting[cnt1][2]==16&&passSetting[cnt1][5]==16&&passSetting[cnt1][6]==8)
					{
						indicateSIB[1][0]=passSetting[cnt1][0];
						indicateSIB[1][1]=passSetting[cnt1][1];
						indicateSIB[1][2]=passSetting[cnt1][2];
						// indicateSIB[1][3]=passSetting[cnt1][3];
						// indicateSIB[1][4]=passSetting[cnt1][4];
						indicateSIB[1][3]=8;
						indicateSIB[1][4]=2;
						indicateSIB[1][5]=passSetting[cnt1][5];
						indicateSIB[1][6]=passSetting[cnt1][6];
						setdone++;
						// cout<<indicateSIB[1][0]<<','<<indicateSIB[1][1]<<','<<indicateSIB[1][2]<<','<<indicateSIB[1][3]<<','<<indicateSIB[1][4]<<','<<indicateSIB[1][5]<<','<<indicateSIB[1][6]<<endl;
						// getchar();
					}
					// else if(passSetting[cnt1][0]==80&&passSetting[cnt1][1]==4&&passSetting[cnt1][2]==8&&passSetting[cnt1][3]==2&&passSetting[cnt1][4]==2&&passSetting[cnt1][5]==8&&passSetting[cnt1][6]==8)
					else if(passSetting[cnt1][0]==1280&&passSetting[cnt1][1]==4&&passSetting[cnt1][2]==32&&passSetting[cnt1][5]==32&&passSetting[cnt1][6]==8)
					{
						indicateSIB[2][0]=passSetting[cnt1][0];
						indicateSIB[2][1]=passSetting[cnt1][1];
						indicateSIB[2][2]=passSetting[cnt1][2];
						// indicateSIB[2][3]=passSetting[cnt1][3];
						// indicateSIB[2][4]=passSetting[cnt1][4];
						indicateSIB[2][3]=16;
						indicateSIB[2][4]=2;
						indicateSIB[2][5]=passSetting[cnt1][5];
						indicateSIB[2][6]=passSetting[cnt1][6];
						setdone++;
						// cout<<indicateSIB[2][0]<<','<<indicateSIB[2][1]<<','<<indicateSIB[2][2]<<','<<indicateSIB[2][3]<<','<<indicateSIB[2][4]<<','<<indicateSIB[2][5]<<','<<indicateSIB[2][6]<<endl;
						// getchar();
					}
					// cout<<passSetting[cnt1][0]<<','<<passSetting[cnt1][1]<<','<<passSetting[cnt1][2]<<','<<passSetting[cnt1][3]<<','<<passSetting[cnt1][4]<<','<<passSetting[cnt1][5]<<','<<passSetting[cnt1][6]<<endl;
					// getchar();
					++cnt1;
				}
			}
	cout << "# of SIB2 Config Pass limitation : " << cnt1 << endl;
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
			// cout<<"NB_SIB2_Struc.start_time[0]: "<<NB_SIB2_Struc.start_time[i]<<endl;
			// cout<<"occupy_Subframe: "<<occupy_Subframe<<endl;
			// cout<<"NB_SIB2_Struc.subcarrier_Offset"<<NB_SIB2_Struc.subcarrier_Offset[i]<<endl;
			// cout<<"occupy_Freq: "<<occupy_Freq<<endl;
			// system("pause");
			for (int j = NB_SIB2_Struc.start_time[i]; j < NB_SIB2_Struc.start_time[i]+occupy_Subframe; ++j)
			{
				// cout<<"subfrme: "<<j<<endl;
				// system("pause");
				for (int z = NB_SIB2_Struc.subcarrier_Offset[i]; z < NB_SIB2_Struc.subcarrier_Offset[i]+occupy_Freq; ++z)
				{
					UL_Channle_Struc[z][j]=1;// 1.occupied subcarrier
				}
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
