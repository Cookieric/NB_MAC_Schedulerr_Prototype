#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <list>

#include <time.h>
#include <stdlib.h>

using namespace std;


// string *Split2(string ,char);
double exp_arrival_time(double);
void define_system_arrival();


int timeslot;
double gamma ;
double lambda ;

double temparrival;
double packetArrivalPos=0;

// #include <boost/random/normal_distribution.hpp>
// #include <boost/random/uniform_real_distribution.hpp>
// #include <boost/random/mersenne_twister.hpp>
// #include <boost/random/variate_generator.hpp>
// #include <boost/random/poisson_distribution.hpp>
// #include <boost/math/distributions/pareto.hpp>


// int main()
// {
// 	srand(time(NULL));
// 	define_system_arrival();
// 	system("pause");
// 	return 0;

	// boost::mt19937 randGen(15); //held constant for repeatability
	// // boost::math::pareto_distribution<> dist(1, 1);
	// boost::poisson_distribution<> dist(1);
	// boost::variate_generator<boost::mt19937&,boost::poisson_distribution<>> generator(randGen, dist);

	// for(int i = 0; i < 10; i++)
 //    	std::cout << generator() << '\n';

 //    system("pause");
	// return 0;
	// // open the input file
 //    fstream fin;
 //    //fin.open("12.in", fstream::in);
 //    fin.open("NPRACH_Configuration.in", fstream::in);
 //    if(!fin.is_open())
 //    {
 //        cout << "Error: the input file is not opened!!" << endl;
 //        exit(1);
 //    }
 //    char title[1000];
 //    fin.getline(title,sizeof(title),'\n');
 //    cout << title << endl;
 //    for (int i = 0; i < 5; ++i)
 //    {
 //    	fin.getline(title,sizeof(title),',');
 //    	int a=stoi(title);
 //    	cout << a << endl;
 //    }
 //    fin.getline(title,sizeof(title),'\n');
 //    int b=stoi(title);
 //    cout << b << endl;
 //    for (int i = 0; i < 5; ++i)
 //    {
 //    	fin.getline(title,sizeof(title),',');
 //    	cout << title << endl;
 //    }
 //    fin.getline(title,sizeof(title),'\n');
 //    cout << title << endl;
 //    for (int i = 0; i < 5; ++i)
 //    {
 //    	fin.getline(title,sizeof(title),',');
 //    	cout << title << endl;
 //    }
 //    fin.getline(title,sizeof(title),'\n');
 //    cout << title << endl;
	// system("pause");
	// return 0;
// }

double exp_arrival_time(double lambda){
	double t = (-1 / lambda)*log(1 - ((rand() / (double)RAND_MAX)));
	// printf("%lf\n",t);
	// system("pause");
	return t;
}

bool compareMyType (const int &a, const int &b)
{
	return a < b;
	// if(a2->round!=b2->round)	return b2->round - a2->round;
	// else	return b2->DV - a2->DV;
}
int factorial(int n)
{
    int i;
    int result = 1;
    if(n==0) return result;
    for (i = 1; i <= n; i++)
        result *= i;
    return result;
}

void define_system_arrival(list<int> & arrivalPerdSubframe,const int user,int * arrivalTime)
{
	double packet_delay_time[user];
	double packetArrivalTime[user];
	ofstream filePtrSimArrMode1, filePtrAnalArrMode1;//check arrival model 1
	double lambda=0.007;
	int countUserPerSubframe=0;
	arrivalPerdSubframe.assign (user*2,0);
	// int *arrivalPerdSubframe = new int[user*2];
	filePtrSimArrMode1.open("Matlab_Result\\SimArrivalMode1_M_5000.csv", ios::out);
	filePtrAnalArrMode1.open("Matlab_Result\\AnalArrivalModel_M_5000.csv", ios::out);
	for (int i = 0; i < user; i++)
	{
		packetArrivalTime[i] = 0;
	}
	// list<int>::iterator it2=arrivalPerdSubframe.begin();
	int userCnt=0;
	for (int j = 0; j < user; j++)
	{
		int u = rand() % user ; // randomly choose 0 ~ user-1
		if (packetArrivalTime[u] == 0)
		{
			packetArrivalTime[u] = exp_arrival_time(lambda);
			int tempPos=ceil(packetArrivalPos);//last subframe bundary
			if(packetArrivalPos+packetArrivalTime[u]>tempPos&&packetArrivalPos!=0)
			{
				list<int>::iterator it2=arrivalPerdSubframe.begin();
				// cout<<"tempPos:"<<tempPos<<endl;
				for (int i = 1; i < tempPos; ++i)	++it2;
				arrivalPerdSubframe.insert (it2,countUserPerSubframe);
				// for (list<int>::iterator it=arrivalPerdSubframe.begin(); it!=arrivalPerdSubframe.end(); ++it)
					// cout << *it <<endl;
				// system("pause");
				// arrivalPerdSubframe.push_back(countUserPerSubframe);
				// cout<<countUserPerSubframe<<endl;
				// system("pause");
				countUserPerSubframe=0;
			}
			++countUserPerSubframe;
			packetArrivalPos= packetArrivalPos + packetArrivalTime[u];
			// cout <<"user " << u + 1<<" inter arrival time = " << packetArrivalTime[u] << ", packetArrivalPos=" << packetArrivalPos <<endl;
			arrivalTime[userCnt]=floor(packetArrivalPos);
			++userCnt;
			// system("pause");
		}
		else
		{
			j = j - 1;
		}
		if(j==user-1)
		{
			list<int>::iterator it2=arrivalPerdSubframe.begin();
			for (int i = 1; i < ceil(packetArrivalPos); ++i)	++it2;
				arrivalPerdSubframe.insert (it2,countUserPerSubframe);
			arrivalPerdSubframe.resize(ceil(packetArrivalPos));
		}
	}
	// for (list<int>::iterator it=arrivalPerdSubframe.begin(); it!=arrivalPerdSubframe.end(); ++it)	cout << *it <<endl;
	// system("pause");
	arrivalPerdSubframe.sort(compareMyType);
	list<int>::iterator it1=arrivalPerdSubframe.begin();
	double cntCDF=0;
	double cntPDF=0;
	list<int> Xarray;
	list<double> YarrayCDF;//PoissionProb CDF
	list<double> YarrayPDF;//PoissionProb PDF,PoissionProb
	// for (list<int>::iterator it=arrivalPerdSubframe.begin(); it!=arrivalPerdSubframe.end(); ++it)	cout << *it <<endl;
	// system("pause");
	for (list<int>::iterator it=arrivalPerdSubframe.begin(); it!=arrivalPerdSubframe.end(); ++it)
	{
		++it1;
		cntCDF++;
		cntPDF++;
		if(*it1!=*it)
		{
			Xarray.push_back(*it);
			YarrayCDF.push_back(cntCDF/arrivalPerdSubframe.size());
			YarrayPDF.push_back(cntPDF/arrivalPerdSubframe.size());
			cntPDF=0;
		}
	}
	// for (list<int>::iterator it=Xarray.begin(); it!=Xarray.end(); ++it)	cout<<*it<<' '<<endl;
	// for (list<double>::iterator it=YarrayCDF.begin(); it!=YarrayCDF.end(); ++it)	cout<<*it<<' '<<endl;
	// for (list<double>::iterator it=YarrayPDF.begin(); it!=YarrayPDF.end(); ++it)	cout<<*it<<' '<<endl;
	list<int>::iterator it3=Xarray.begin();
	for (list<double>::iterator it=YarrayPDF.begin(); it!=YarrayPDF.end(); ++it)
	{
		filePtrSimArrMode1 << *it3 <<","<<*it<<endl;
		++it3;
	}
	// double tempAns=exp(-lambda);
	// double PoissionAnalysis[21];
	// for (int i = 0; i < 21; ++i)
	// {
	// 	// int tempPow=pow(lambda,i);
	// 	// int Fac=factorial(i);
	// 	PoissionAnalysis[i]=(tempAns * pow(lambda,i))/factorial(i);
	// 	if(i>lambda+10)	PoissionAnalysis[i]=0;
	// 	filePtrAnalArrMode1 << i <<","<<PoissionAnalysis[i]<<endl;
	// 	// cout<<"P: "<<PoissionAnalysis[i]<<"pow(lambda,i)"<<tempPow<<"Fac: "<<Fac<<endl;
	// 	// system("pause");
	// }

	timeslot = floor(packetArrivalPos);
	cout<<"Final_Packet_arrival at " << timeslot << " subframe"<<endl;
	filePtrSimArrMode1.close();
	filePtrAnalArrMode1.close();

	// system("pause");
}



