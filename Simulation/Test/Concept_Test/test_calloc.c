#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define LOG(...) printf(__VA_ARGS__)


typedef enum channel_E{
	NA=0,
	//DL
	NPSS,
	NSSS,
	NPBCH,
	SIB1,
	SIB2,
	NPDCCH,
	NPDSCH,
	//UL
	NPRACH,
	NPUSCH,

	L_channels
}channel_t;

char channel_N[L_channels][10] = {
	"NA",
	//DL
	"NPSS",
	"NSSS",
	"NPBCH",
	"SIB1_NB",
	"SIB2_NB",
	"NPDCCH",
	"NPDSCH",
	//UL
	"NPRACH",
	"NPUSCH"
};

channel_t *DL_Channle_bitmap;

int main()
{
	bool flag=false;
	for (int i = 0; i < 1000; ++i)
	{
		uint32_t t_si_Period;
		if((i%100==0)&&(!flag))
		{
			flag=true;
			t_si_Period=100;
		}
		LOG("t_si_Period:%d\n",t_si_Period);
		system("pause");
	}



	int subframes=0,frame=0;
	uint32_t NPDCCH_period=256;
	DL_Channle_bitmap=(uint32_t *)calloc(NPDCCH_period,sizeof(uint32_t));
	// DL_Channle_bitmap[subframes]=NPBCH;

	LOG("subframes:%d,Channel:%s",subframes,DL_Channle_bitmap[subframes]);
	system("pause");


	while(1)
	{
		if(((frame & 0x01)==0)&&(subframes==9))
			DL_Channle_bitmap[subframes]=NSSS;
		LOG("frame:%d,subframes:%d,Channel:%s\n",frame,subframes,channel_N[DL_Channle_bitmap[subframes]]);
		system("pause");
		++subframes;
		if(subframes==10)
		{
			subframes=0;
			++frame;
		}
	}
	free(DL_Channle_bitmap);
	system("pause");
	return 0;
}
