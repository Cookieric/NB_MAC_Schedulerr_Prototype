#include <iostream>
//All the stuff on the following should be modify
// void NB_schedule_MIB(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channel_bitmap, bool initialSche)
// {
	// uint32_t scheSubframe=0,scheFrame=0;
	// int i;
	// scheFrame=frame;
	// if(initialSche)	scheSubframe=subframes;
	// else scheSubframe=subframes+1;
	// if(scheSubframe==10)
	// {
	// 	scheSubframe=0;
	// 	++scheFrame;
	// }
	// for (int i = 0; i < NPDCCH_period; ++i)
	// {
	// 	if(scheSubframe==5)	DL_Channel_bitmap[i]=NPSS; //Reserve for NPSS, not send MAC_PDU/DCI
	// 	else if(((scheFrame & 0x01)==0)&&(scheSubframe==9))
	// 	{
	// 		// LOG("(frame & 0x01):%d\n",(frame & 0x01));
	// 		DL_Channel_bitmap[i]=NSSS;//Reserve for NSSS, not send MAC_PDU/DCI
	// 	}
	// 	else if(scheSubframe==0)	DL_Channel_bitmap[i]=NPBCH;//mac_rrc_data_req(get MIB content from RRC)

	// LOG("frame:%d,subframe:%d,Channel:%s\n",frame,subframes,channel_N[DL_Channel_bitmap[frame*10+subframes]]);
		// LOG("frame:%d,subframe:%d,Channel:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
		// system("pause");

// 		++scheSubframe;
// 		if(scheSubframe==10)
// 		{
// 			scheSubframe=0;
// 			++scheFrame;
// 		}
// 		if(scheFrame==1024)	scheFrame=0;
// 	}
// }

// uint32_t t_si_Period,t_si_windowStart,si_windowEnd;
// void NB_schedule_SI(frame_t frame,sub_frame_t subframes,uint32_t NPDCCH_period,uint32_t *DL_Channel_bitmap,MIB_NB * MIB_NB_S, SIB1_NB * SIB1_NB_S,bool initialSche)//schedule SIB1/SIB2/3
// {
	/*SIB1-NB in subframe #4 of every other frame in 16 continuous frames. Period = 256RF*/
	// uint32_t repetitionNum_SIB1=repetiitonSIB1(MIB_NB_S->schedulingInfoSIB1);
	// uint32_t startRF_SIB1=RFstartSIB1(repetitionNum_SIB1);
	// uint32_t TBS_Index=MIB_NB_S->schedulingInfoSIB1;
	// uint32_t TBS_SIB1=getTBS_SIB1(TBS_Index);
	// uint32_t repetitionOffset=sib1_Period/repetitionNum_SIB1;//if offset=64-->0,64,128,192,256....

	// uint32_t scheSubframe=0,scheFrame=0;
	// int i;
	// scheFrame=frame;
	// if(initialSche)	scheSubframe=subframes;
	// else scheSubframe=subframes+1;
	// if(scheSubframe==10)
	// {
	// 	scheSubframe=0;
	// 	++scheFrame;
	// }
	// for (int i = 0; i < NPDCCH_period; ++i)
	// {
		//Schedule SIB1 and disable shcedule SIB1 from every 16 frames to next repetiiton.
		// if(scheFrame%repetitionOffset==0)	shcedSIB1=true;
		// else if(scheFrame%repetitionOffset==16)	shcedSIB1=false;
		//Bug fixed(frame & 0x01==0)-->((frame & 0x01)==0)
		//starting schedule SIB1 from first RF or second RF base on sib1_startingRF
		// if (shcedSIB1&&((scheFrame & 0x01)==sib1_startingRF)&&(scheSubframe==4))
		// {
		// 	DL_Channel_bitmap[i]=SIB1;
		// }
		// LOG("repetitionNum_SIB1:%d,startRF_SIB1:%d,TBS_SIB1:%d,repetitionOffset:%d\n",repetitionNum_SIB1,startRF_SIB1,TBS_SIB1,repetitionOffset);
		// system("pause");

		/*SIB2-NB,SIB3-NB use same scheudlingInfo in subframe # within si-window. Period = 64RF*/
		// uint32_t si_windowStart=get_si_windowStart(SIB1_NB_S,scheFrame);//get start frame or -1
		/*Bug fixed, After free memory, the value will change even no assignment
		uint32_t t_si_windowStart,si_windowEnd,SIscheSubframe;
		uint32_t t_si_Period;*/
		// uint32_t SIscheSubframe;
		// LOG("si_windowStart:%d,si_windowEnd:%d\n",si_windowStart,si_windowEnd);
		// if((si_windowStart!=-1)&&(!lock))
		// {
		// 	// LOG("Start Sche SIB2-NB\n");
		// 	lock=true;
		// 	t_si_windowStart=si_windowStart;
		// 	si_windowEnd=si_windowStart+(SIB1_NB_S->si_WindowLength/10)-1;
		// 	t_si_Period=si_windowStart+(SIB1_NB_S->si_Periodicity/10)-1;
			// LOG("si_windowStart:%d,si_windowEnd:%d,next_si_Period:%d\n",t_si_windowStart,si_windowEnd,t_si_Period);
			// system("pause");
		// }
		//Schedule SIB2/3 within SI-window base on si-Repetiiton
		// if((scheFrame>=t_si_windowStart)&&(scheFrame<=si_windowEnd))
		// {
		// 	if(((scheFrame-SIB1_NB_S->si_RadioFrameOffset) & SIB1_NB_S->si_RepetitionPattern)==0)
		// 	{
		// 		schedSIB2=true;
		// 		if(scheSubframe==0)	SIscheSubframe=get_si_scheSubframe(SIB1_NB_S->si_TB);// for SIB2/3-NB
		// 	}
		// 	else	schedSIB2=false;
		// }

		// if((DL_Channel_bitmap[i]==NA)&&(schedSIB2)&&(SIscheSubframe>0))
		// {
			// LOG("si_windowStart:%d,si_windowEnd:%d,SIscheSubframe:%d\n",t_si_windowStart,si_windowEnd,SIscheSubframe);
			// system("pause");
			// DL_Channel_bitmap[i]=SIB2_3;//call mac_rrc_data_req();
			// --SIscheSubframe;
			// if((scheFrame==si_windowEnd)&&(SIscheSubframe>0))
			// {
			// 	LOG("SIB2-NB not finish transmission within si-window\n");
			// 	exit(1);
			// }
		// }
		// if((scheFrame==t_si_Period)&&(scheSubframe==9))	lock=false;//Wake up scheSIB2 at the end of this si-period.
		// LOG("frame:%d,subframe:%d,Channel:%s\n",scheFrame,scheSubframe,channel_N[DL_Channel_bitmap[i]]);
		// system("pause");
		// ++scheSubframe;
		// if(scheSubframe==10)
		// {
		// 	scheSubframe=0;
		// 	++scheFrame;
		// }
		// if(scheFrame==1024)	scheFrame=0;
	// }
// }
