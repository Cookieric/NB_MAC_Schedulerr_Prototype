// list::push_back
#include <iostream>
#include <list>
#include "NB_Default_Channel_Config.h"

int main ()
{
	std::list<UL_UE_list> UL_Sche_UE_List;
	std::list<UL_UE_list>::iterator it1,it2;//it1:(UL_UE_list *)
	// std::list<int> mylist;
	UL_UE_list UL_Sche_UE;
	int num_UEs=10, UE_id=0;
	for (int i = 0; i < num_UEs; ++i)
	{
		UL_Sche_UE_List.push_back (UL_Sche_UE);
	}
	std::cout << "UL_Sche_UE_List stores " << UL_Sche_UE_List.size() << " UEs.\n";
	for (it1=UL_Sche_UE_List.begin(); it1!=UL_Sche_UE_List.end(); ++it1)
	{
		UL_UE_list *i = &*it1;//need to convert from iterator to (UL_UE_list *)
		UE_Init(*i);
		i->UE_id=UE_id;
		++UE_id;
		i->CE_Level=rand() % 3; //decided by UE's RSRP
		i->DV=1+rand()%9;	//decided by UE's UL buffer
		i->multi_tone=rand() % 2; //decided by Msg3's content
		i->UL_Buffer_Sizes=get_UL_Data_Bytes(i->DV);
		//http://www.sharetechnote.com/html/Handbook_LTE_CodeRate.html#CodeRate_vs_MCS
		//MCS:2-10 <->Channnel quality/Coding rate
		i->mcs=2+rand()%9;//Qm=2,MCS:2-10 36213 Table 16.5.1.2-1
		std::cout << ' ' << i->UE_id<<','<<i->UL_Buffer_Sizes<<std::endl;
	}
	it2 = UL_Sche_UE_List.begin();
	++it2;
	UL_Sche_UE_List.erase (it2);
	std::cout << "UL_Sche_UE_List stores " << UL_Sche_UE_List.size() << " UEs.\n";
	UL_Sche_UE_List.push_front (UL_Sche_UE);
	std::cout << "UL_Sche_UE_List stores " << UL_Sche_UE_List.size() << " UEs.\n";

	system("pause");
 	return 0;
}
int get_UL_Data_Bytes(int DV)
{
	int UL_Data_Bytes=0;
	switch(DV)
	{
		case 0:
			UL_Data_Bytes=0;
			break;
		case 1:
			UL_Data_Bytes=rand()%10+1;//0 < DV <= 10
			break;
		case 2:
			UL_Data_Bytes=11+rand()%4;//10 < DV <= 14
			break;
		case 3:
			UL_Data_Bytes=15+rand()%5;//14 < DV <= 19
			break;
		case 4:
			UL_Data_Bytes=20+rand()%7;//19 < DV <= 26
			break;
		case 5:
			UL_Data_Bytes=27+rand()%10;//26 < DV <= 36
			break;
		case 6:
			UL_Data_Bytes=37+rand()%13;//36 < DV <= 49
			break;
		case 7:
			UL_Data_Bytes=50+rand()%18;//49 < DV <= 67
			break;
		case 8:
			UL_Data_Bytes=68+rand()%24;//67 < DV <= 91
			break;
		case 9:
			UL_Data_Bytes=92+rand()%34;//91 < DV <= 125
			break;
		case 10:
			UL_Data_Bytes=126+rand()%46;//125 < DV <= 171
			break;
		case 11:
			UL_Data_Bytes=172+rand()%63;//171 < DV <= 234
			break;
		case 12:
			UL_Data_Bytes=235+rand()%87;//234 < DV <= 321
			break;
		case 13:
			UL_Data_Bytes=322+rand()%447;//321 < DV <= 768
			break;
		case 14:
			UL_Data_Bytes=268+rand()%733;//768 < DV <= 1500-->fixed to no more 1000
			break;
		case 15:
			UL_Data_Bytes=1501;//DV > 1500
			break;
		default:
			UL_Data_Bytes=-1;
			break;
	}
	return UL_Data_Bytes;
}
void UE_Init(UL_UE_list &UL_Sche_UE)//Call by reference is the same as Call by pointer
{
	//UL
	UL_Sche_UE.UE_id=-1;
	UL_Sche_UE.Priority=-1;
	UL_Sche_UE.CE_Level=-1;
	UL_Sche_UE.BSR=-1;
	UL_Sche_UE.DV=-1;
	UL_Sche_UE.PHR=-1;
	UL_Sche_UE.multi_tone=-1;
	UL_Sche_UE.num_subcarrier_one_RU=-1;
	UL_Sche_UE.num_UL_Slot=-1;
	UL_Sche_UE.freq_Space=0;
	UL_Sche_UE.mcs=-1;
	UL_Sche_UE.remaining_Buffer_Sizes=0;
	UL_Sche_UE.round=0;
	//DL
	// DL_Sche_UE.UE_id=-1;
	// DL_Sche_UE.Priority=-1;
	// DL_Sche_UE.CE_Level=-1;
	// DL_Sche_UE.Buffer_Size=-1;
}
