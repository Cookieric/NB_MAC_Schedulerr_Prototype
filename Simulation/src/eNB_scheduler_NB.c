#include <stdio.h>
#include <stdlib.h>

#include "rrc_NB.h"
#include "sche_pro_NB.h"
#include "type_NB.h"

extern uint32_t NPDCCH_period;
uint32_t **UL_Channle_bitmap;
uint32_t * element;
// channel_t * DL_Channle_bitmap;

uint32_t *DL_Channle_bitmap;
// channel_t **UL_Channle_bitmap;
// int num_DL_Subcarrier=12;
int num_UL_Subcarrier=48;




uint32_t Cell_id=0;//Fixed base on NPSS/NSSS Calculation

//Argument of NB_eNB_dlsch_ulsch_scheduler should be:
//module_id_t module_idP,uint8_t cooperation_flag, frame_t frameP, sub_frame_t subframeP
int main(void)
{
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


	uint32_t i_pp;//loop for NPDCCH_period
	while(1)
	{
		for (i_pp = 0; i_pp < NPDCCH_period; ++i_pp)
		{
			if(i_pp==0)//Build UL/DL Structure base on NPDCCH Period
			{
				LOG("frame:%d,subframes:%d,Start NPDCCH period(pp)/Build pp struc for DL/UL \n",frame,subframes);
				system("pause");
				//	calloc initialize to 0 -> channel_t.NA
				UL_Channle_bitmap= (uint32_t **)calloc(num_UL_Subcarrier,sizeof(uint32_t *));
				// // UL_Channle_bitmap= (uint32_t **)malloc(num_UL_Subcarrier * sizeof(uint32_t *));
				element= (uint32_t *)calloc(num_UL_Subcarrier * NPDCCH_period,sizeof(uint32_t));
				// // element= (uint32_t *)malloc(num_UL_Subcarrier * NPDCCH_period * sizeof(uint32_t));
				int i;
				for (i=0; i<num_UL_Subcarrier; i++, element+=NPDCCH_period)
					UL_Channle_bitmap[i]=element;
				DL_Channle_bitmap=(uint32_t *)calloc(NPDCCH_period,sizeof(uint32_t));
				// DL_Channle_bitmap=(uint32_t *)malloc(NPDCCH_period * sizeof(uint32_t));
			}
			// LOG("frame:%d,subframes:%d\n",frame,subframes);
			// UEs_arrival(frame,subframes);



			NB_shceudle_fixed(frame,subframes,i_pp,DL_Channle_bitmap);//NPSS/NSSS/NPBCH
			NB_shceudle_SI(frame,subframes,i_pp,DL_Channle_bitmap,&MIB_NB_S, &SIB1_NB_S);
			// NB_shceudle_RA(frame,subframes,DL_Channle_bitmap,UL_Channle_bitmap);
			// NB_shceudle_ulsch(frame,subframes,DL_Channle_bitmap,UL_Channle_bitmap);
			// NB_shceudle_dlsch(frame,subframes,DL_Channle_bitmap);



			if(i_pp==255)//The end of this NPDCCH period, Free UL/DL Structure
			{
				free(UL_Channle_bitmap[0]);
				free(UL_Channle_bitmap);
				free(DL_Channle_bitmap);
				// free(element); //Don't need free element....Bug
				LOG("Free This NPDCCH Period Memory space!\n");
				system("pause");
			}
			++subframes;
			if(subframes==10)
			{
				subframes=0;
				++frame;
			}
		}
		// system("pause");
	}
	system("pause");
	return 0;
}
