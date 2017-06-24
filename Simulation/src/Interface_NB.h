#ifndef _INTERFACE_NB_H_
#define _INTERFACE_NB_H_
#include "dci_NB.h"
#include "sche_pro_NB.h"
#include <list>


using namespace std;



typedef struct UL_IND
{
	list<struct _UE_TEMPLATE_NB> UL_UE_Info_List[3];
	//ACK/NACK
	//preamble_List
}UL_IND_t;

typedef struct Sche_RES
{
	// HI_DCI0_request_t HI_DCI0={0};
	list<struct HI_DCI0_request> DCI_L;
	// queue<HI_DCI0_request_t> DCI_Q;
	//DL config request
	//preamble_List
}Sche_RES_t;

uint32_t Sche_res(frame_t,sub_frame_t,Sche_RES_t &);
uint32_t Ulsch_ind(frame_t,sub_frame_t,UL_IND_t &);


#endif
