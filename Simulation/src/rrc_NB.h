#ifndef _RRC_NB_H_
#define _RRC_NB_H_

#include <stdbool.h>
#include "type_NB.h"
#define MAX_NUM_CCs 1
#define NUMBER_OF_UE_MAX 1000

//MIB-NB
typedef struct _MIB_NB
{
	uint32_t operationModeInfo;//inband-SamePCI-r13,inband-DifferentPCI-r13,guardband-r13,standalone-r13
	uint32_t schedulingInfoSIB1;//intilize to 9
}MIB_NB;

//SIB1-NB
typedef struct _SIB1_NB
{
	// uint16_t	downlinkBitmap;//Not used for now
	//Only set this value in inband,ENUMERATED {n1, n2, n3}
	// uint8_t eutraControlRegionSize;//Not used for now
	//Common for all SIBs
	uint32_t si_WindowLength;//ENUMERATED{ms160,ms320,ms480,ms640,ms960,ms1280,ms1600,spare1}
	//si-window starting frame offset
	uint32_t si_RadioFrameOffset;//INTEGER (1..15)
	//SchedulingInfo-NB-r13 for SIB2-NB
	uint32_t si_Periodicity;
	uint32_t si_RepetitionPattern;
	// uint32_t sib_MappingInfo_r13;-->sibType3-NB-r13
	uint32_t si_TB;
}SIB1_NB;

//SIB2-NB-->RadioResourceConfigCommonSIB-NB-r13-->NPRACH-ConfigSIB-NB-r13-->NPRACH-Parameters-NB-r13
//Size of NPRACH Prarameter List(1....maxNPRACH-Resources-NB-r13(2))
//Not Included Parts: maxNumPreambleAttemptCE-r13
typedef struct _SIB2_NB{
	float target_SNR[3];//14.25,MCL:144 DB(CE 0)/ 4.25,MCL:154 DB(CE 1)/ -5.75,MCL:164 DB(CE 2)
	uint32_t CE_Level[3];//CE (0,1,2)

	/*NPRACH Prarameter List:*/
	uint32_t rep[3];//numRepetitionsPerPreambleAttempt-r13
	uint32_t period[3];//nprach-Periodicity-r13
	uint32_t start_time[3];//nprach-StartTime-r13
	uint32_t num_Subcarrier[3];//nprach-NumSubcarriers-r13
	uint32_t subcarrier_Offset[3];//nprach-SubcarrierOffset-r13
	uint32_t subcarrierMSG3[3];//nprach-SubcarrierMSG3-RangeStart-r13
	//DCI N1 for RAR and Msg4, DCI N0 for Msg3 retransmission
	uint32_t npdcch_NumRepetitions_RA[3];//Max # of repetitions for NPDCCH common search space (CSS) for RAR, Msg3 retransmission and Msg4
	uint32_t npdcch_StartSF_CSS_RA[3];//Starting subframe configuration for NPDCCH common search space (CSS), including RAR, Msg3 retransmission, and Msg4
	float npdcch_Offset_RA[3];//Fractional period offset of starting subframe for NPDCCH common search space (CSS Type 2)
	// bool flag_NPRACH_Change;
	/*SIB2-->RadioResourceConfigCommonSIB-NB-r13-->RACH-ConfigCommon-NB:*/
	uint32_t rawindow[3];
	uint32_t ContentionResolutionTimer[3];

	//candicate =Rmax/R, R defined by DCI repetition number in DCI
	uint32_t candidate[3];
}SIB2_NB;

//Msg4-->RadioResourceConfigDedicated-NB-r13-->physicalConfigDedicated-r13-->npdcch-ConfigDedicated-r13
typedef struct _npdcch_ConfigDedicated_NB{
	uint32_t npdcch_NumRepetitions[3];
	uint32_t npdcch_StartSF_USS[3];
	float npdcch_Offset_USS[3];
} npdcch_ConfigDedicated_NB;

//Msg4-->RadioResourceConfigDedicated-NB-r13-->MAC-MainConfig-NB-->ul-SCH-Config-r13
typedef struct _ULSCH_Config_NB{
	int PeriodicBSR_Timer_NB;//RRC default set to 8 pp
	int RetxBSR_Timer_NB;//RRC default set to infinity
}ULSCH_Config_NB;

typedef struct _RRCCoonectionSetup_NB{
	npdcch_ConfigDedicated_NB UE_specificConfig;
	ULSCH_Config_NB bsrConfig;
}RRCCoonectionSetup_NB;


#ifdef __cplusplus
extern "C" {
#endif
int NB_eNB_Init_RRC(MIB_NB *,SIB1_NB *,SIB2_NB *,RRCCoonectionSetup_NB *);
void printSIs(MIB_NB * MIB_NB_S, SIB1_NB * SIB1_NB_S, SIB2_NB * SIB2_NB_S,RRCCoonectionSetup_NB *);
#ifdef __cplusplus
}
#endif

int Filter_SIB2_NB_Config(SIB2_NB *);
int checkFrequencyDomain(SIB2_NB *);
void Initial_UE_Specific_Config(RRCCoonectionSetup_NB *,SIB2_NB *);
#endif
