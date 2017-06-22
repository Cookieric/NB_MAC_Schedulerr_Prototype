// array::begin example
#include <iostream>
#include <array>
#include <list>

typedef struct _UE_TEMPLATE_NB
{
	//Get from preamble base on NPRACH Config
	uint8_t CE_Level;	//After decode preamble, eNB will know UE's CE level.
	//C-RNTI of UE
	// rnti_t rnti;
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

// typedef struct UL_IND
// {
// 	list<struct _UE_TEMPLATE_NB> UL_UE_Info_List;
// 	//ACK/NACK
// 	//preamble_List
// }UL_IND_t;

typedef list<UE_TEMPLATE_NB> UE_TEMPLATE_NB_LIST;
array<UE_TEMPLATE_NB_LIST> arr(3);

// UL_IND_t UL_Indicaiton;
int main ()
{
  	// UL_Indicaiton.UL_UE_Info_List;
  	typename list<UE_TEMPLATE_NB>::iterator it1;
  	list<UE_TEMPLATE_NB> & UE_Info_List=arr[0];
  	UE_TEMPLATE_NB UL_UE_Info;
  	int new_num_UE=6,UE_id=0;
	for (int i = 0; i < new_num_UE; ++i)
	{
		UE_Info_List.push_back (UL_UE_Info);//Add to UE list
	}
	for (it1=UE_Info_List.begin(); it1!=UE_Info_List.end();++it1)
	{
		(*it1).UE_id=UE_id;
		++UE_id;
	}

	  std::cout << "myarray contains:";
	  for ( auto it1 = arr[0].begin(); it1 != arr[0].end(); ++it1 )
	    std::cout << ' ' << *it1;
	  std::cout << '\n';

  return 0;
}


