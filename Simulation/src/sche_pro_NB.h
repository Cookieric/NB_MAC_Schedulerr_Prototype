#ifndef _SCHE_PRO_NB_H_
#define _SCHE_PRO_NB_H_

#include "rrc_NB.h"
#include "type_NB.h"
#include <stdbool.h>

typedef struct _RA_list_NB
{
	bool generate_RAR;
	bool generate_Msg4;
}RA_list_NB;


typedef struct _UE_list_NB
{
	//Get from SIB2-NB
	uint8_t CE_Level;	//After decode preamble, eNB will know UE's CE level.
	//Get from Msg1
	int multi_tone_Msg3;// 0: not support; 1:support
	//Msg3 MAC Control Element
	int UE_id;		//UE_id determine by TC-RNTI in RAR
	int DV;	//BS know UE's UL Buffer Data Size.(CP solution)
	int PHR;	//BS know UE's Power budget.
	//Msg3 content
	int multi_tone;// 0: not support; 1:support
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
}UE_list_NB;


/*Schedule Helper*/
//SIB1_Helper_Fun
uint32_t repetiitonSIB1(uint32_t);
uint32_t RFstartSIB1(uint32_t);
uint32_t getTBS_SIB1(uint32_t);
//SIB2_Helper_Fun
uint32_t get_si_windowStart(SIB1_NB *,frame_t);
uint32_t get_si_scheSubframe(uint32_t);

/*Schedule Function*/
void NB_shceudle_fixed(frame_t,sub_frame_t,uint32_t,uint32_t *);//schedule NPSS/NSSS/NPBCH
void NB_shceudle_SI(frame_t,sub_frame_t,uint32_t,uint32_t *,MIB_NB *,SIB1_NB *);//schedule SIB1/SIB2...
void NB_shceudle_RA(frame_t,sub_frame_t,uint32_t *,uint32_t **);
void NB_shceudle_ulsch(frame_t,sub_frame_t,uint32_t *,uint32_t **);
void NB_shceudle_dlsch(frame_t,sub_frame_t,uint32_t *);
#endif
