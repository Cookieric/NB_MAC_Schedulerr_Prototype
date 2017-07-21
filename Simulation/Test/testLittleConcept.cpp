#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

double uniform_rng()
{
  double random;
  random = (double)(rand()%RAND_MAX)/((double)RAND_MAX);
  //LOG_D(OTG,"Uniform taus random number= %lf\n", random);
  return random;
}

// Uniform Distribution using the Uniform_Random_Number_Generator

double uniform_dist(int min, int max)
{
  double uniform_rn;
  uniform_rn = (max - min) * uniform_rng() + min;
  //  LOG_T(OTG,"Uniform Random Nb = %lf, (min %d, max %d)\n", uniform_rn, min, max);
  //printf("Uniform Random Nb = %lf, (min %d, max %d)\n", uniform_rn, min, max);
  return uniform_rn;
}


double pareto_dist(double scale, double shape)
{
  double pareto_rn;

  if ((scale<=0)||(shape<=0)) {
    printf("Pareto :: scale=%.2f or shape%.2f <0 , adjust new values: sale=3, shape=4 \n", scale,shape);
    scale=3;
    shape=4;
  }

  pareto_rn=scale * pow(1/(1-uniform_rng()), 1/shape);
  // printf("Pareto Random Nb = %lf (scale=%.2f, shape=%.2f)\n", pareto_rn,scale,shape);
  return pareto_rn;
}

uint32_t DV_table[15]={0,10,14,19,26,36,49,67,91,125,171,234,321,768,1500};//Map to highest
uint32_t DV1_table[15]={0,1,11,15,20,27,37,50,68,92,126,172,235,322,769};//Map to lowest

uint32_t BSR_table[63]={0,10,12,14,17,19,22,26,31,36,42,49,57,67,78,91,
107,125,146,171,200,234,274,321,376,440,515,603,706,826,967,
1132,1326,1552,1817,2127,2490,2915,3413,3995,4677,5476,6411,
7505,8787,10287,12043,14099,16507,19325,22624,26487,31009,
36304,42502,49759,58255,68201,79846,93479,109439,128125,150000};//>150000, BS assume 150000, Map to highest
uint32_t BSR1_table[63]={0,1,11,13,15,18,20,23,27,32,37,43,50,58,68,79,
92,108,126,147,172,201,235,275,322,377,441,516,604,707,827,
968,1133,1327,1553,1818,2128,2491,2916,3414,3996,4678,5477,
6412,7506,8788,10288,12044,14100,16508,19326,22625,26488,
31010,36305,42503,49760,58256,68202,79847,93480,109440,128126};//Map to lowest

int main(int argc, char const *argv[])
{


for (int i = 0; i < 63; ++i)
{
	printf("%d,",(BSR_table[i]+BSR1_table[i])/2);
}
system("pause");
return 0;

	srand(time(NULL));
	double PC[3]={0.6,0.2,0};//CE leve(0,1,2)-->(40%,40%,20%), ratio of devices in period_UL report.
	int CE_Level,Num_CE_Level[3]={0};
	for (int i = 0; i < 100000; ++i)
	{
		double pc=uniform_rng();
		if(pc>PC[0])	Num_CE_Level[0]++;
		else if((pc<PC[0])&&(pc>PC[1]))	Num_CE_Level[1]++;
		else if((pc>=PC[2])&&(pc<PC[1]))	Num_CE_Level[2]++;
	}
	int Total=Num_CE_Level[0]+Num_CE_Level[1]+Num_CE_Level[2];
	double p_CE0,p_CE1,p_CE2;
	p_CE0=(double)Num_CE_Level[0]/Total;
	p_CE1=(double)Num_CE_Level[1]/Total;
	p_CE2=(double)Num_CE_Level[2]/Total;
	printf("CE0:%lf,CE1:%lf,CE2:%lf\n",p_CE0,p_CE1,p_CE2);
	system("pause");

	printf("RAND_MAX:%d\n", RAND_MAX);
	for (int i = 0; i < 1000; ++i)
	{
		double payloadSize=pareto_dist(20,2.5);
		if(payloadSize>200)
		{
			payloadSize=200;
			// printf("pareto_dist[%d]:%lf\n", i,payloadSize);
		}
		else
			payloadSize=floor(payloadSize+0.5);
		printf("pareto_dist[%d]:%lf\n", i,payloadSize);
		// printf("uniform_rng:%lf\n",uniform_rng());
		// printf("uniform_dist(0,10):%lf\n", uniform_dist(0,10));
		// printf("%d\n",rand());
		// printf("%lf\n",(double)rand()/RAND_MAX );
	}
	// if(rand()/RAND_MAX)
	return 0;
}
