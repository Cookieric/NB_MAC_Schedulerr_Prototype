for (it1=UL_Sche_UE_List.begin(); it1!=UL_Sche_UE_List.end();)
{
	UE_info *i = &*it1;
	int flag=preSubframeMapping(j,*i,NB_SIB2_Struc,UL_Channle_Struc);
	if(i->remaining_Buffer_Sizes<=0&&i->remainging_subframe==0&&flag!=-1)
	{
		// cout<<"UE_id: "<<i->UE_id<<" has finished packet transmission"<<endl;
		it1 = UL_Sche_UE_List.erase (it1);
		// --it1;
	}
	else if(i->remaining_Buffer_Sizes>0&&i->remainging_subframe==0&&flag!=-1)
	{
		// cout<<"DCI for this round done, let BSR as new arrival for BSR_UE struc after Bdelay..."<<endl;
		// cout<<"UE_id: "<<i->UE_id<<"; remaining_Buffer_Sizes: "<<i->remaining_Buffer_Sizes<<endl;
		// system("pause");
		it1 = UL_Sche_UE_List.erase (it1);
		// --it1;
		if(UL_Sche_UE_List.empty())
		{
			finalPacketPos=j;
			// cout<<"finalPacketPos= "<<j<<endl;
		}
	}
	else
		++it1;
	// it1 = mylist.erase (it1);
}
