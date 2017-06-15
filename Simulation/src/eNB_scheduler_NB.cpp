#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <list>
#include "rrc_NB.h"
#include "sche_pro_NB.h"
#include "interface_NB.h"
#include "type_NB.h"

using namespace std;

extern uint32_t NPDCCH_period;
uint32_t n_pp=0;//number of  NPDCCH_period
uint32_t **UL_Channel_bitmap;
uint32_t * element;
// channel_t * DL_Channel_bitmap;

uint32_t *DL_Channel_bitmap;
// channel_t **UL_Channel_bitmap;
// int num_DL_Subcarrier=12;
int num_UL_Subcarrier=48;


// typedef struct UL_IND
// {
// 	list<UE_TEMPLATE_NB> UL_UE_Info_List;
// 	//ACK/NACK
// 	//preamble_List
// }UL_IND_t;


UL_IND_t UL_Indicaiton;

// list<UE_TEMPLATE_NB> UL_UE_Info_List


uint32_t Cell_id=0;//Fixed base on NPSS/NSSS Calculation
uint32_t H_SFN=0;//Hyper super frame = 1024 frame

//Argument of NB_eNB_dlsch_ulsch_scheduler should be:
//module_id_t module_idP,uint8_t cooperation_flag, frame_t frameP, sub_frame_t subframeP
int main(void)
{
	srand(time(NULL));
	MIB_NB	MIB_NB_S= {0};//Directly Initialize
	SIB1_NB	SIB1_NB_S= {0};
	SIB2_NB SIB2_NB_S= {0};
	RRCCoonectionSetup_NB Msg4_S={0};
	if(NB_eNB_Init_RRC(&MIB_NB_S, &SIB1_NB_S, &SIB2_NB_S,&Msg4_S))	LOG("Initialize RRC done\n");
	printSIs(&MIB_NB_S,&SIB1_NB_S,&SIB2_NB_S,&Msg4_S);
	LOG("NPDCCH_period:%d\n",NPDCCH_period);
	// LOG("%d",channels_length);


	//Start to schedule
	sub_frame_t subframes=0;
	frame_t frame=0;



	/*Initialization first UL/DL virtual channel structure before SF 0*/
	//	calloc initialize to 0 --> channel_t.NA
	UL_Channel_bitmap= (uint32_t **)calloc(num_UL_Subcarrier,sizeof(uint32_t *));
	// // UL_Channel_bitmap= (uint32_t **)malloc(num_UL_Subcarrier * sizeof(uint32_t *));
	element= (uint32_t *)calloc(num_UL_Subcarrier * NPDCCH_period,sizeof(uint32_t));
	// // element= (uint32_t *)malloc(num_UL_Subcarrier * NPDCCH_period * sizeof(uint32_t));
	int i;
	for (i=0; i<num_UL_Subcarrier; i++, element+=NPDCCH_period)
		UL_Channel_bitmap[i]=element;
	DL_Channel_bitmap=(uint32_t *)calloc(NPDCCH_period,sizeof(uint32_t));
	// DL_Channel_bitmap=(uint32_t *)malloc(NPDCCH_period * sizeof(uint32_t));

	//Initial schedule for MIB/SIB1/23 before H_SFN:0,SFN:0,SF:0 in RUN_Status
	LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule next pp(%d~%d) for UL/DL at previous SF of each p\n",H_SFN,frame,subframes,(frame*10+subframes),(frame*10+subframes+NPDCCH_period));
	system("pause");
	NB_schedule_MIB(frame,subframes,NPDCCH_period,DL_Channel_bitmap,true);//NPBCH
	NB_schedule_SI(frame,subframes,NPDCCH_period,DL_Channel_bitmap,&MIB_NB_S, &SIB1_NB_S,true);
	//NPDCCH Period base scheudling for MIB/SIB1/23 and UL;  RA and DL not done.
	while(1)
	{
		if(((frame*10+subframes)%NPDCCH_period)==0) n_pp++;
		Ulsch_ind(frame,subframes,UL_Indicaiton);
		//Build UL/DL virtual channel structure at the start of previous SF of each pp.
		if(((H_SFN * 10240+frame * 10+subframes+1)%NPDCCH_period)==0)
		{
			LOG("H_SFN:%d,frame:%d,subframes:%d,Build next pp(%d~%d)struc for UL/DL at previous SF of each pp\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+NPDCCH_period));
			// system("pause");
			n_pp++;
			//	calloc initialize to 0 -> channel_t.NA
			UL_Channel_bitmap= (uint32_t **)calloc(num_UL_Subcarrier,sizeof(uint32_t *));
			// // UL_Channel_bitmap= (uint32_t **)malloc(num_UL_Subcarrier * sizeof(uint32_t *));
			element= (uint32_t *)calloc(num_UL_Subcarrier * NPDCCH_period,sizeof(uint32_t));
			// // element= (uint32_t *)malloc(num_UL_Subcarrier * NPDCCH_period * sizeof(uint32_t));
			int i;
			for (i=0; i<num_UL_Subcarrier; i++, element+=NPDCCH_period)
				UL_Channel_bitmap[i]=element;
			DL_Channel_bitmap=(uint32_t *)calloc(NPDCCH_period,sizeof(uint32_t));
			// DL_Channel_bitmap=(uint32_t *)malloc(NPDCCH_period * sizeof(uint32_t));
			// LOG("frame:%d,subframes:%d\n",frame,subframes);
			// Check if UL_data/Msg3/Msg5/Other UL RRC Msg3 arrival before schedule ulsch/dlsch
		}
		//Schedule UL/DL virtual channel structure at the start of previous SF of each pp.
		if(((H_SFN * 10240+frame * 10+subframes+1)%NPDCCH_period)==0)
		{
			LOG("H_SFN:%d,frame:%d,subframes:%d,Schedule next pp(%d~%d) for UL/DL at previous SF of each p\n",H_SFN,frame,subframes,(frame*10+subframes+1),(frame*10+subframes+1+NPDCCH_period));
			system("pause");
			NB_schedule_MIB(frame,subframes,NPDCCH_period,DL_Channel_bitmap,false);//NPBCH
			NB_schedule_SI(frame,subframes,NPDCCH_period,DL_Channel_bitmap,&MIB_NB_S, &SIB1_NB_S,false);
			// NB_schedule_RA(frame,subframes,DL_Channel_bitmap,UL_Channel_bitmap);
			// NB_schedule_ulsch(frame,subframes,NPDCCH_period,DL_Channel_bitmap,UL_Channel_bitmap,&SIB2_NB_S,UL_Indicaiton);
			// NB_schedule_dlsch(frame,subframes,DL_Channel_bitmap);
		}


		if(((H_SFN * 10240+frame * 10+subframes+2)%NPDCCH_period)==0)//The end of previous tow SF of each pp.
		{
			free(UL_Channel_bitmap[0]);
			free(UL_Channel_bitmap);
			free(DL_Channel_bitmap);
			// free(element); //Don't need free element....Bug
			LOG("Free This NPDCCH Period Memory space!\n");
			system("pause");
		}
		/*subframes_ind in FAPI*/
		++subframes;
		if(subframes==10)
		{
			subframes=0;
			++frame;
		}
		if(frame==1024)
		{
			frame=0;
			H_SFN++;
		}
		// system("pause");
	}
	system("pause");
	return 0;
}

