#define LOG(...) printf(__VA_ARGS__)

typedef struct {
	double target_SNR[3];
	int CE_Level[3];
	int rep[3];//nprach-Periodicity
	int period[3];//nprach-Periodicity
	int start_time[3];
	int num_Subcarrier[3];
	int subcarrier_Offset[3];
	//DCI N1 for RAR and Msg4, DCI N0 for Msg3 retransmission
	int npdcch_NumRepetitions_RA[3];//Maximum number of repetitions for NPDCCH common search space (CSS) for RAR, Msg3 retransmission and Msg4
	int npdcch_Offset_RA[3];//Fractional period offset of starting subframe for NPDCCH common search space (CSS Type 2)
	int npdcch_StartSF_CSS_RA[3];//Starting subframe configuration for NPDCCH common search space (CSS), including RAR, Msg3 retransmission, and Msg4
	int rawindow[3];
	int candidate[3];
	int I_SI_NPRACH;//44ms, a gap, could be variale
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

//RadioResourceConfigDedicated-NB-r13-->physicalConfigDedicated-r13-->npdcch-ConfigDedicated-r13
typedef struct _npdcch_ConfigDedicated_r13{
	int npdcch_NumRepetitions;
	double npdcch_StartSF_USS;
	double npdcch_Offset_USS;
} npdcch_ConfigDedicated_r13;

//RadioResourceConfigDedicated-NB-r13-->MAC-MainConfig-NB-->ul-SCH-Config-r13
typedef struct _ULSCH_Config_r13{
	int PeriodicBSR_Timer_NB_r13;
	int RetxBSR_Timer_NB_r13;
} ULSCH_Config_r13;

typedef struct _Msg4_Struc{
	npdcch_ConfigDedicated_r13 ueSpecificConfig;
	ULSCH_Config_r13 bsrConfig;
} Msg4_Struc;

typedef struct _NPDSCH{
	Msg2_Struc Msg2;
	Msg4_Struc Msg4;
}NPDSCH;

///  DCI Format Type N0
struct DCIN0 {
  /// type = 0 => DCI Format N0, type = 1 => DCI Format N1
  uint32_t type:1;
  /// Subcarrier indication
  uint32_t scind:6;
  /// Resourse Assignment (RU Assignment)
  uint32_t ResAssign:3;
  /// Modulation and Coding Scheme and Redundancy Version
  uint32_t mcs:4;
  /// New Data Indicator
  uint32_t ndi:1;
  /// Scheduling Delay
  uint32_t Scheddly:2;
  /// Repetition Number
  uint32_t RepNum:3;
  /// Redundancy version for HARQ (only use 0 and 2)
  uint32_t rv:1;
  /// DCI subframe repetition Number
  uint32_t DCIRep:2;
} __attribute__ ((__packed__));

typedef struct DCIN0 DCIN0_t;
#define sizeof_DCIN0_t 23

