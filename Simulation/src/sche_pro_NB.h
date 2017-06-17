#ifndef _SCHE_PRO_NB_H_
#define _SCHE_PRO_NB_H_

#include "rrc_NB.h"
#include "type_NB.h"
#include <stdbool.h>

#include "interface_NB.h"

typedef struct _UE_TEMPLATE_NB
{
	//Get from preamble base on NPRACH Config
	uint8_t CE_Level;	//After decode preamble, eNB will know UE's CE level.
	//C-RNTI of UE
	rnti_t rnti;
	uint32_t UE_id;
	// NDI from last DL scheduling
  	uint8_t oldNDI;// not used for now.
  	// NDI from last UL scheduling
  	uint8_t oldNDI_UL;
  	uint8_t CRC_indication;
	// Flag to indicate UL has been scheduled at least once
 	bool ul_active;
	// Flag to indicate UE has been configured (ACK from RRCConnectionSetup received)
	bool configured;//After receive Msg3 in subframe n, recevice ACK for Msg4 in subframe n+pp
	uint8_t PHR;	//BS know UE's Power budget.
	//Msg3 content
	uint8_t multi_tone;// 0: not support; 1:support
	uint8_t DV;//DV
	uint8_t BSR;//shortBSR
	//Scheduled Parameters
	uint8_t	Priority;// not used for now, Priority base on size of DV/BSR...
	uint32_t UL_Buffer_Size;
	uint32_t allocate_Buffer_Size;
	uint8_t mcs;//mcs Index
	uint8_t Qm;
	int round;
	uint32_t remaining_Buffer_Sizes;
	int startFreqPos;//Isc
	//NPUSCH RU Table
	int num_tone;
	int num_UL_Slot;
	int num_RU;
	int remainging_subframe;
	//Sche_Ctrl
	uint32_t first_Arrival_Time;//Msg3 arrival time
	uint32_t sche_Msg5_Time;
	uint32_t next_Arrival_Time;
}UE_TEMPLATE_NB;


#ifdef __cplusplus
extern "C" {
#endif
/*Schedule Helper*/
//SIB1_Helper_Fun
uint32_t repetiitonSIB1(uint32_t);
uint32_t RFstartSIB1(uint32_t);
uint32_t getTBS_SIB1(uint32_t);
//SIB2_Helper_Fun
uint32_t get_si_windowStart(SIB1_NB *,frame_t);
uint32_t get_si_scheSubframe(uint32_t);

/*Schedule Function*/
void reserve_schedule_MIB(frame_t,sub_frame_t,uint32_t,uint32_t *,bool);//schedule NPBCH
void reserve_schedule_SI(frame_t,sub_frame_t,uint32_t,uint32_t *,MIB_NB *,SIB1_NB *,bool);//sche SIB1/23

void NB_schedule_MIB(frame_t,sub_frame_t,uint32_t,uint32_t *,bool);//Send schedConfig and MIB PDU to PHY
void NB_schedule_SI(frame_t,sub_frame_t,uint32_t,uint32_t *,MIB_NB *,SIB1_NB *,bool);//Send schedConfig, SIB1/23 PDU to PHY

void NB_schedule_RA(frame_t,sub_frame_t,uint32_t *,uint32_t **);
void NB_schedule_ulsch(frame_t,sub_frame_t,uint32_t,uint32_t *,uint32_t **,SIB2_NB *,UL_IND_t &);
void NB_schedule_dlsch(frame_t,sub_frame_t,uint32_t *);

uint8_t DCIs_resource_determinaiton(uint32_t,uint32_t,SIB2_NB *,list<UE_TEMPLATE_NB> &,Sche_RES_t & ,uint32_t *);
uint32_t resourceAllocation(SIB2_NB *,uint32_t **,list<UE_TEMPLATE_NB> &);
#ifdef __cplusplus
}
#endif

#endif
