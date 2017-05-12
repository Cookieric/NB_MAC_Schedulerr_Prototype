#ifndef NB_Default_Channel_Config_H


#define NB_Default_Channel_Config_H
#define NUMBER_OF_UE_MAX 10
#define num_UL_Subcarrier 48
#define num_DL_Subcarrier 12
//Msg Identity
// #define preamble 1
// #define Msg2 2
// #define Msg3 3
// #define Msg4 4
// #define DCI 5


typedef struct {
	int **resourceStruc;
} UL_Anchor_Channel_Structure;

typedef struct {
	int **resourceStruc;
} DL_Anchor_Channel_Structure;

// typedef struct {
// 	int UE_id;		//BS identify UE by UE_id
// 	int Priority;	//BS determine priority depends on Buffer_Size or CE level
// 	int CE_Level;	//After RA Succeed, UE will know own CE level.
// 	int Buffer_Size;	//BS know UE's DL Buffer Data Size.(CP/UP solution)
// } estimateResource;

typedef struct {
	int UE_id;		//BS identify UE by UE_id
	int Priority;	//BS determine priority depends on DV/BSR, more tone be used or CE level
	int CE_Level;	//After RA Succeed, UE will know own CE level.
	int BSR;	//BS know UE's UL Buffer Data Size.(UP solution)
	int DV;	//BS know UE's UL Buffer Data Size.(CP solution)
	int UL_Buffer_Sizes;
	int PHR;	//BS know UE's Power budget.
	int multi_tone;// 0: not support; 1:support
	int mcs;//mcs Index
	int round;
	int remaining_Buffer_Sizes;
	//NPUSCH RU Table
	double freq_Space;
	int num_subcarrier_one_RU;
	int num_UL_Slot;
}UL_UE_list;

typedef struct {
	int UE_id;		//BS identify UE by UE_id
	int Priority;	//BS determine priority depends on Buffer_Size or CE level
	int CE_Level;	//After RA Succeed, UE will know own CE level.
	int Buffer_Size;	//BS know UE's DL Buffer Data Size.(CP/UP solution)
} DL_UE_list;


typedef struct {
	int Flag_N0;//0:N0, 1:N1
	int subcarrierIndication;
	int resourceAssignment;
	int schedulingDelay;
	int mcs;//Fixed
	int RV;//Fixed
	int repetitionNumber;
	int NDI;//Not used
	int DCI_repetitionNumber;//Fixed setting by NPDCCH-Config
} DCI_N0;

typedef struct {
	int Flag_N0;//0:N0, 1:N1
	int NPDCCH_orderIndicator;//(not consider initial RA,set to 0)
	int resourceAssignment;
	int schedulingDelay;
	int mcs;//Fixed
	int repetitionNumber;
	int NDI;//Not used
	int HARQ_ACK_resource;//Not used
	int DCI_repetitionNumber;//Fixed setting by NPDCCH-Config
} DCI_N1;

//NB UL Anchor Carrier for default channel


// // NPRACH* RSRP_Threshold = new NPRACH[3];
// // delete[] RSRP_Threshold; // 不用時釋放
typedef struct {
	double target_SNR[3];
	int CE_Level[3];
	int rep[3];
	int period[3];//nprach-Periodicity
	int start_time[3];
	int first_end_time;//used for estimate start pos of UL data
	int num_Subcarrier[3];
	int subcarrier_Offset[3];
	//DCI N1 for RAR and Msg4, DCI N0 for Msg3 retransmission
	int npdcch_NumRepetitions_RA[3];//Maximum number of repetitions for NPDCCH common search space (CSS) for RAR, Msg3 retransmission and Msg4
	int npdcch_Offset_RA[3];//Fractional period offset of starting subframe for NPDCCH common search space (CSS Type 2)
	int npdcch_StartSF_CSS_RA[3];//Starting subframe configuration for NPDCCH common search space (CSS), including RAR, Msg3 retransmission, and Msg4
	int I_Preamble_RAR;//Interval from end pos of preamble to start of RAR or DCI N1(RAR)?
	int I_RAR_Msg3;//Interval from RAR to start pos of Ms3
	int I_Msg3_DCIN0;//Interval from end pos of Ms3 to start pos of DCI N0
} NPRACH;

// typedef struct {

// } NPDCCH;

typedef struct {
	int NRS; //transmit on All NPDSCH except for NPSS/NSSS/NPBCH.
	int ra_ResponseWindowSize;
	int ContentionResolutionTimer;
} NPDSCH;


//NB DL Anchor Carrier for default channel
// int NPSS(int x,int y);
// int NSSS(int x,int y);
// int PBCH(int x,int y);
// int NPDSCH_SIB(int x,int y);


//Initialization
void UE_Init(UL_UE_list &);
//NB UL Scheduler Function incldue resource allocation
int define_Channel_Structure(UL_Anchor_Channel_Structure *, DL_Anchor_Channel_Structure *, int);
int Set_NPRACH_Resource(NPRACH *, int, int);
int do_NPUSCH_Resource_Allocation(UL_UE_list **);
int do_NPUSCH_Scheduler(int,int,int,NPRACH *,UL_Anchor_Channel_Structure *);
int get_UL_Data_Bytes(int);
int get_num_subcarrier(void);
int get_num_Slot(int);
int get_TBS_UL(int,int);
int get_I_TBS(int);
int get_I_RU(int);
#endif
