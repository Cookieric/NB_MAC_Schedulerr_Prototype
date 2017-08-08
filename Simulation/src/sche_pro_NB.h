#ifndef _SCHE_PRO_NB_H_
#define _SCHE_PRO_NB_H_

#include "rrc_NB.h"
#include "type_NB.h"
#include <stdbool.h>

#include "interface_NB.h"


typedef struct _UE_TEMPLATE_NB
{
	//Get from preamble base on NPRACH Config
	uint32_t CE_Level;	//After decode preamble, eNB will know UE's CE level.
	//C-RNTI of UE
	rnti_t rnti;
	uint32_t UE_id;
	// NDI from last DL scheduling
  	uint8_t oldNDI;// not used for now.
  	// NDI from last UL scheduling
  	uint8_t oldNDI_UL;
  	uint32_t CRC_indication;
	// Flag to indicate UL has been scheduled at least once
 	// bool ul_active;
	// Flag to indicate UE has been configured (ACK from RRCConnectionSetup received)
	bool configured;//After receive Msg3 in subframe n, recevice ACK for Msg4 in subframe n+pp
	bool schedStatus;
	// bool schedMsg3;
	int PHR;	//BS know UE's Power budget.
	//Msg3 content
	uint32_t multi_tone;// 0: not support; 1:support
	int DV;//DV
	int BSR;//shortBSR
	//Scheduled Parameters
	uint8_t	Priority;// not used for now, Priority base on size of DV/BSR...
	int UL_Buffer_Size;
	int allocate_Buffer_Size;
	uint8_t mcs;//mcs Index
	uint8_t Qm;
	int round;
	// int remaining_Buffer_Sizes;
	int startFreqPos;//Isc
	//NPUSCH RU Table
	int num_tone;
	int num_UL_Slot;
	int num_RU;
	int remainging_subframe;
	//Sche_Ctrl
	uint32_t first_Arrival_Time;//Msg3 arrival time
	int sche_Msg5_Time;
	uint32_t next_Arrival_Time;
	int payloadSize;//Used for virtual UE
	uint32_t I_payloadSize;
}UE_TEMPLATE_NB;

typedef struct _Pattern_base {
	int startFreq;
	int endFreq;
	int FreqUnit;
	int assignedPrio;
}Pattern_base;

#ifdef __cplusplus
extern "C" {
#endif
/*Schedule Helper*/
//SIB1_Helper_Fun
uint32_t repetiitonSIB1(uint32_t);
uint32_t RFstartSIB1(uint32_t);
uint32_t getTBS_SIB1(uint32_t);
//SIB2_Helper_Fun
uint32_t get_si_windowStart(SIB1_NB &,frame_t);
uint32_t get_si_scheSubframe(uint32_t);

/*Schedule Function*/
void NB_schedule_MIB(frame_t,sub_frame_t,uint32_t,uint32_t *,bool);//schedule NPBCH
void NB_schedule_SI(frame_t,sub_frame_t,uint32_t,uint32_t *,MIB_NB *,SIB1_NB *,bool);//sche SIB1/23

void NB_schedule_MIB(frame_t,sub_frame_t,uint32_t,uint32_t *,bool);//Send schedConfig and MIB PDU to PHY
void NB_schedule_SI(frame_t,sub_frame_t,uint32_t,uint32_t *,MIB_NB *,SIB1_NB *,bool);//Send schedConfig, SIB1/23 PDU to PHY

void NB_schedule_RA(frame_t,sub_frame_t,uint32_t *,uint32_t **);
// void NB_schedule_ulsch(frame_t,sub_frame_t,uint32_t,uint32_t *,uint32_t **,SIB2_NB *,UL_IND_t &);
void NB_schedule_ulsch(uint32_t,frame_t,sub_frame_t,uint32_t,MIB_NB &,SIB1_NB &,SIB2_NB &,RRCCoonectionSetup_NB &,UL_IND_t &);
void NB_schedule_dlsch(frame_t,sub_frame_t,uint32_t *);

bool compareMyType5 (const HI_DCI0_request_t &, const HI_DCI0_request_t &);
bool compareMyType4 (const HI_DCI0_request_t &, const HI_DCI0_request_t &);
uint32_t get_aggregation(uint32_t,uint32_t,uint32_t);
uint32_t get_nprah_resource(int,SIB2_NB &);
uint32_t get_DCI_Filed(const uint32_t,uint32_t);
uint32_t num_ULslots(uint32_t);
uint32_t get_I_RU(uint32_t);
uint32_t get_I_TBS(uint32_t,uint32_t);
int get_TBS_UL(uint32_t,uint32_t,uint32_t &,bool &);
uint32_t check_if_NPRACH(SIB2_NB &,uint32_t,uint32_t);
bool compareMyType3 (const UE_TEMPLATE_NB &, const UE_TEMPLATE_NB &);
uint32_t check_if_DL_subframe(uint32_t,uint32_t,uint32_t,MIB_NB &,SIB1_NB &);
// uint8_t DCIs_resource_determinaiton(uint32_t,uint32_t,SIB2_NB *,list<UE_TEMPLATE_NB> &,Sche_RES_t & ,uint32_t *);
uint8_t DCIs_resource_determinaiton(uint32_t,uint32_t,uint32_t,uint32_t,MIB_NB &,SIB1_NB &,SIB2_NB &,list<UE_TEMPLATE_NB> &,list<HI_DCI0_request_t> &);
// uint32_t resourceAllocation(SIB2_NB *,uint32_t **,list<UE_TEMPLATE_NB> &);
uint32_t resourceAllocation(SIB2_NB &,list<UE_TEMPLATE_NB> &);
#ifdef __cplusplus
}
#endif

#endif
