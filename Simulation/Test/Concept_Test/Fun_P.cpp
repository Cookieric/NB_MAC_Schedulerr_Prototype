#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LOG(...) printf(__VA_ARGS__)

int addInt(int n, int m);


int main(int argc, char const *argv[])
{
	// if(argc < 1 || *(argv+1)== NULL)
	// {
	// 	printf("No Argument Parameters");
	// 	return 0;
	// 	// printf("%s\n", *argv);
	// 	// printf("%s\n", *(argv+1));
 //    	// perror("perror");
 //    	// printf("strerror(errno)=%s\n", strerror(errno));
	// }
	//Set NPRACH_Occupy_Subcarrier=48, NPRACH_Config =1 ( CE level 0 )
	int NPRACH_Occupy_Time=0, NPRACH_Config=1, NPRACH_Period=0, NPRACH_Start_Time=0,NPRACH_End_Time=0;
	double NPRACH_Occupy_Subcarrier=48;
	double UL_Remaining_Freq=0;
	int (*functionPtr)(int,int);
	functionPtr = &addInt;
	int sum = (*functionPtr)(2, 3);

	LOG("%d",sum);

	system("pause");
	return 0;
}


int addInt(int n, int m) {
    return n+m;
}
