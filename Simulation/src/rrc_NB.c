#include "rrc_NB.h"
#include <stdio.h>
#include <stdlib.h>


// uint32_t NPDCCH_period=0;
uint32_t CSS_NPDCCH_period[3]={0,0,0};
uint32_t USS_NPDCCH_period[3]={0,0,0};

char NameOperationMode[num_operationMode][20] = {
	"inband-SamePCI",
	"inband-DifferentPCI",
	"guardband",
	"standalone"
};

int NB_eNB_Init_RRC(MIB_NB * MIB_NB_S, SIB1_NB * SIB1_NB_S, SIB2_NB * SIB2_NB_S,RRCCoonectionSetup_NB * Msg4_S)
{
	//Initial for MIB-NB
	MIB_NB_S->operationModeInfo=2;//guard band deployment
	MIB_NB_S->schedulingInfoSIB1=9;
	LOG("OperationMode:%s\n",NameOperationMode[MIB_NB_S->operationModeInfo]);
	//Initial for SIB1-NB
	SIB1_NB_S->si_WindowLength=160;
	SIB1_NB_S->si_RadioFrameOffset=1;// set to 1 if cell id is even, set to 0 if cell id is odd.
	SIB1_NB_S->si_Periodicity=640;//RF64
	SIB1_NB_S->si_RepetitionPattern=0x01;//0x01 means every2ndRF if (frame&0x01==0)
	SIB1_NB_S->si_TB=208;//208 bits(8 subfrmaes) for SIB2/3,sib2-r13=163 bits,sib3-r13=37 bits,Total: 200 bits
	//Initial for SIB2-NB
	if(Filter_SIB2_NB_Config(SIB2_NB_S)==1);	LOG("\tFilter and Set NPRACH Resource done!\n");
	if(checkFrequencyDomain(SIB2_NB_S)==0)	LOG("\tNPRACH Config CE(0,1,2) Pass FrequencyDomain limitation\n");
	//Initial for UE-Specific RRC Configuration
	Initial_UE_Specific_Config(Msg4_S,SIB2_NB_S);
	//All pp are the same for three CE levels
	// NPDCCH_period=SIB2_NB_S->npdcch_NumRepetitions_RA[0] * SIB2_NB_S->npdcch_StartSF_CSS_RA[0];
	CSS_NPDCCH_period[0]=SIB2_NB_S->npdcch_NumRepetitions_RA[0] * SIB2_NB_S->npdcch_StartSF_CSS_RA[0];
	CSS_NPDCCH_period[1]=SIB2_NB_S->npdcch_NumRepetitions_RA[1] * SIB2_NB_S->npdcch_StartSF_CSS_RA[1];
	CSS_NPDCCH_period[2]=SIB2_NB_S->npdcch_NumRepetitions_RA[2] * SIB2_NB_S->npdcch_StartSF_CSS_RA[2];
	USS_NPDCCH_period[0]=Msg4_S->UE_specificConfig.npdcch_NumRepetitions[0] * Msg4_S->UE_specificConfig.npdcch_StartSF_USS[0];
	USS_NPDCCH_period[1]=Msg4_S->UE_specificConfig.npdcch_NumRepetitions[1] * Msg4_S->UE_specificConfig.npdcch_StartSF_USS[1];
	USS_NPDCCH_period[2]=Msg4_S->UE_specificConfig.npdcch_NumRepetitions[2] * Msg4_S->UE_specificConfig.npdcch_StartSF_USS[2];
	return 1;
}

void printSIs(MIB_NB * MIB_NB_S, SIB1_NB * SIB1_NB_S, SIB2_NB * SIB2_NB_S,RRCCoonectionSetup_NB * Msg4_S)
{
	LOG("MIB-NB:\n\toperationModeInfo:%d\n",MIB_NB_S->operationModeInfo);
	LOG("\tschedulingInfoSIB1:%d\n",MIB_NB_S->schedulingInfoSIB1);
	LOG("SIB1_NB:\n\tsi_WindowLength:%d\n",SIB1_NB_S->si_WindowLength);
	LOG("\tsi_RadioFrameOffset:%d\n",SIB1_NB_S->si_RadioFrameOffset);
	LOG("\tsi_Periodicity:%d\n",SIB1_NB_S->si_Periodicity);
	LOG("\tsi_RepetitionPattern:%d\n",SIB1_NB_S->si_RepetitionPattern);
	LOG("\tsi_TB:%d\n",SIB1_NB_S->si_TB);
	int i,j;
	for (i = 0; i < 3; ++i)
	{
		LOG("SIB2 CE%d:\n",SIB2_NB_S->CE_Level[i]);
		LOG("\ttarget_SNR:%.2f\n",SIB2_NB_S->target_SNR[i]);
		LOG("\tperiod:%d\n",SIB2_NB_S->period[i]);
		LOG("\trep:%d\n",SIB2_NB_S->rep[i]);
		LOG("\tstart_time:%d\n",SIB2_NB_S->start_time[i]);
		LOG("\tnum_Subcarrier:%d\n",SIB2_NB_S->num_Subcarrier[i]);
		LOG("\tsubcarrier_Offset:%d\n",SIB2_NB_S->subcarrier_Offset[i]);
		LOG("\tnpdcch_NumRepetitions_RA:%d\n",SIB2_NB_S->npdcch_NumRepetitions_RA[i]);
		LOG("\tnpdcch_StartSF_CSS_RA:%d\n",SIB2_NB_S->npdcch_StartSF_CSS_RA[i]);
		LOG("\tnpdcch_Offset_RA:%.3f\n",SIB2_NB_S->npdcch_Offset_RA[i]);
		LOG("\trawindow:%d\n",SIB2_NB_S->rawindow[i]);
		LOG("\tcandidate:%d\n",SIB2_NB_S->candidate[i]);//DCI RepNum(00)-->rmax[m]/8=R(4)
	}
	for (j = 0; j < 3; ++j)
	{
		LOG("UEs_pecificConfig CE%d:\n",j);
		LOG("\tnpdcch_NumRepetitions:%d\n",Msg4_S->UE_specificConfig.npdcch_NumRepetitions[j]);
		LOG("\tpdcch_StartSF_USS:%d\n",Msg4_S->UE_specificConfig.npdcch_StartSF_USS[j]);
		LOG("\tpdcch_Offset_USS:%.3f\n",Msg4_S->UE_specificConfig.npdcch_Offset_USS[j]);
	}
	LOG("bsrConfig:\n");
	LOG("\tPeriodicBSR_Timer_NB:%d\n",Msg4_S->bsrConfig.PeriodicBSR_Timer_NB);
	LOG("\tRetxBSR_Timer_NB:%d\n",Msg4_S->bsrConfig.RetxBSR_Timer_NB);
}

void Initial_UE_Specific_Config(RRCCoonectionSetup_NB * Msg4_S,SIB2_NB * SIB2_NB_S)
{
	//CE 0
	Msg4_S->UE_specificConfig.npdcch_NumRepetitions[0]=SIB2_NB_S->npdcch_NumRepetitions_RA[0];
	Msg4_S->UE_specificConfig.npdcch_StartSF_USS[0]=SIB2_NB_S->npdcch_StartSF_CSS_RA[0];
	Msg4_S->UE_specificConfig.npdcch_Offset_USS[0]=SIB2_NB_S->npdcch_Offset_RA[0];
	//CE 1
	Msg4_S->UE_specificConfig.npdcch_NumRepetitions[1]=SIB2_NB_S->npdcch_NumRepetitions_RA[1];;
	Msg4_S->UE_specificConfig.npdcch_StartSF_USS[1]=SIB2_NB_S->npdcch_StartSF_CSS_RA[1];
	Msg4_S->UE_specificConfig.npdcch_Offset_USS[1]=SIB2_NB_S->npdcch_Offset_RA[1];
	//CE 2
	Msg4_S->UE_specificConfig.npdcch_NumRepetitions[2]=SIB2_NB_S->npdcch_NumRepetitions_RA[2];;
	Msg4_S->UE_specificConfig.npdcch_StartSF_USS[2]=SIB2_NB_S->npdcch_StartSF_CSS_RA[2];
	Msg4_S->UE_specificConfig.npdcch_Offset_USS[2]=SIB2_NB_S->npdcch_Offset_RA[2];
	//BSR
	Msg4_S->bsrConfig.PeriodicBSR_Timer_NB=2;// 2pp is smallest value, 8 pp is default value in 36.331 spec.
	Msg4_S->bsrConfig.RetxBSR_Timer_NB=-1;//disable the timer in NB-IoT
}

int Filter_SIB2_NB_Config(SIB2_NB * SIB2_NB_S)
{
	int rachperiod[8]={40,80,160,240,320,640,1280,2560};
	int rachstart[8]={8,16,32,64,128,256,512,1024};
	int rachrepeat[8]={1,2,4,8,16,32,64,128};
	int rawindow[8]={2,3,4,5,6,7,8,10};// Not include in MAC_Command in shell script
	int rmax[12]={1,2,4,8,16,32,64,128,256,512,1024,2048};
	float gvalue[8]={1.5,2,4,8,16,32,48,64};
	int candidate[4]={1,2,4,8};


	float pdcchoffset[4]={0,0.125,0.25,0.375};


	int dlcqi[13]={0,1,2,3,4,5,6,7,8,9,10,11,12};
	int dlrepeat[16]={1,2,4,8,16,32,64,128,192,256,384,512,768,1024,1536,2048};
	int rachscofst[7]={0,12,24,36,2,18,34};
	int rachnumsc[4]={12,24,36,48};

	//limitCondition1
	int L_rachperiod=sizeof(rachperiod)/sizeof(int);
	int L_rachrepeat=sizeof(rachrepeat)/sizeof(int);
	int L_rachstart=sizeof(rachstart)/sizeof(int);

	//limitCondition2
	int L_candidate=sizeof(candidate)/sizeof(int);
	int L_rmax=sizeof(rmax)/sizeof(int);
	// LOG("L_candidate:%d,L_rmax:%d",L_candidate,L_rmax);
	//Not limitCondition
	int L_rawindow=sizeof(rawindow)/sizeof(int);
	int L_dlcqi=sizeof(dlcqi)/sizeof(int);
	int L_dlrepeat=sizeof(dlrepeat)/sizeof(int);
	float L_gvalue=sizeof(gvalue)/sizeof(float);

	int limitCondition1[500][3];
	int cnt=0;
	int i,j,k;
	/*limitation condition*/
	for (i = 0; i < L_rachperiod; ++i)
	{
		for (j = 0; j < L_rachrepeat; ++j)
			for (k = 0; k < L_rachstart; ++k)
				if(rachperiod[i]>1.4 * 4 * rachrepeat[j]&&rachperiod[i]>rachstart[k])
				{
					limitCondition1[cnt][0]=rachperiod[i];
					limitCondition1[cnt][1]=rachrepeat[j];
					limitCondition1[cnt][2]=rachstart[k];
					// LOG("%d,%d,%d\n",limitCondition1[cnt][0],limitCondition1[cnt][1],limitCondition1[cnt][2]);
					// if(limitCondition1[cnt][0]==80&&limitCondition1[cnt][1]==2&&limitCondition1[cnt][2]==8)
					// 	LOG("Test for limitation\n");
					++cnt;
				}
	}
	// printf("%d\n", cnt);
	printf("Filter Running:\n");
	int setdone=0;
	// int cnt1=0;
	//number_of_target_SNR=3;
	float OffsetSNR[3]={0,10,20};
	int l,m,n;
	for (l= 0; l < cnt; ++l)
	{
		for (m = 0; m < L_candidate; ++m)
			for (n = 0; n < L_rmax; ++n)
				if(candidate[m]<=rmax[n])
					{
						if(setdone==3)
						{
							// SIB2_NB_S->flag_NPRACH_Change=true;
							return 1;
						}
						if(limitCondition1[l][0]==1280&&limitCondition1[l][1]==1&&limitCondition1[l][2]==8&&rmax[n]==8&&candidate[m]==8)
						{
							// LOG("Test CE0\n");
							SIB2_NB_S->target_SNR[0]=14.25-OffsetSNR[0];
							SIB2_NB_S->CE_Level[0]=0;
							SIB2_NB_S->period[0]=limitCondition1[l][0];
							SIB2_NB_S->rep[0]=limitCondition1[l][1];
							SIB2_NB_S->start_time[0]=limitCondition1[l][2];
							SIB2_NB_S->num_Subcarrier[0]=24;
							SIB2_NB_S->subcarrier_Offset[0]=0;
							SIB2_NB_S->npdcch_NumRepetitions_RA[0]=rmax[n];
							SIB2_NB_S->npdcch_StartSF_CSS_RA[0]=8;//G
							SIB2_NB_S->npdcch_Offset_RA[0]=0;//Alpha offset
							SIB2_NB_S->rawindow[0]=2;//2pp
							SIB2_NB_S->candidate[0]=candidate[m];//8, DCI RepNum(00)-->rmax[m]/8=R(1)
							setdone++;
						}
						else if(limitCondition1[l][0]==1280&&limitCondition1[l][1]==2&&limitCondition1[l][2]==8&&rmax[n]==16&&candidate[m]==8)
						{
							// LOG("Test CE1\n");
							SIB2_NB_S->target_SNR[1]=14.25-OffsetSNR[1];
							SIB2_NB_S->CE_Level[1]=1;
							SIB2_NB_S->period[1]=limitCondition1[l][0];
							SIB2_NB_S->rep[1]=limitCondition1[l][1];
							SIB2_NB_S->start_time[1]=limitCondition1[l][2];
							SIB2_NB_S->num_Subcarrier[1]=12;
							SIB2_NB_S->subcarrier_Offset[1]=24;
							SIB2_NB_S->npdcch_NumRepetitions_RA[1]=rmax[n];
							SIB2_NB_S->npdcch_StartSF_CSS_RA[1]=8;//G
							SIB2_NB_S->npdcch_Offset_RA[1]=0;
							SIB2_NB_S->rawindow[1]=2;
							SIB2_NB_S->candidate[1]=candidate[m];//8, DCI RepNum(00)-->rmax[m]/8=R(2)
							setdone++;
						}
						else if(limitCondition1[l][0]==1280&&limitCondition1[l][1]==4&&limitCondition1[l][2]==8&&rmax[n]==32&&candidate[m]==8)
						{
							// LOG("Test CE2\n");
							SIB2_NB_S->target_SNR[2]=14.25-OffsetSNR[2];
							SIB2_NB_S->CE_Level[2]=2;
							SIB2_NB_S->period[2]=limitCondition1[l][0];
							SIB2_NB_S->rep[2]=limitCondition1[l][1];
							SIB2_NB_S->start_time[2]=limitCondition1[l][2];
							SIB2_NB_S->num_Subcarrier[2]=12;
							SIB2_NB_S->subcarrier_Offset[2]=36;
							SIB2_NB_S->npdcch_NumRepetitions_RA[2]=rmax[n];
							SIB2_NB_S->npdcch_StartSF_CSS_RA[2]=8;//G
							SIB2_NB_S->npdcch_Offset_RA[2]=0;
							SIB2_NB_S->rawindow[2]=2;
							SIB2_NB_S->candidate[2]=candidate[m];//8, DCI RepNum(00)-->rmax[m]/8=R(4)
							setdone++;
						}
						// ++cnt1;
					}
	}
	LOG("Program should Not execute here\n");
	return 0;
}

int checkFrequencyDomain(SIB2_NB * SIB2_NB_S)
{
	int checkFreqArray[48]={0};
	int errorReason=-1;
	int i,j;
	for (i = 0; i < 3; ++i)
	{
		if(i==0)
		{
			for (j = SIB2_NB_S->subcarrier_Offset[i]; j < SIB2_NB_S->subcarrier_Offset[i]+SIB2_NB_S->num_Subcarrier[i]; ++j)
			{
				if(checkFreqArray[j]==0)	checkFreqArray[j]=1;
				else	errorReason=0;
			}
		}
		else if(i==1)
		{
			for (j = SIB2_NB_S->subcarrier_Offset[i]; j < SIB2_NB_S->subcarrier_Offset[i]+SIB2_NB_S->num_Subcarrier[i]; ++j)
			{
				if(checkFreqArray[j]==0)	checkFreqArray[j]=2;
				else	errorReason=1;
			}
		}
		else if(i==2)
		{
			for (j = SIB2_NB_S->subcarrier_Offset[i]; j < SIB2_NB_S->subcarrier_Offset[i]+SIB2_NB_S->num_Subcarrier[i]; ++j)
			{
				if(checkFreqArray[j]==0)	checkFreqArray[j]=3;
				else	errorReason=2;
			}
		}
		if(errorReason!=-1)
		{
			LOG("Error: the NPRACH-Config(CE%d) exceed FreqDomain resource!!\n",errorReason);
			system("pause");
			exit(1);
		}
	}
	return 0;
}


