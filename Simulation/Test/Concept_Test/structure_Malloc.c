#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
struct person {
   int age;
   float weight;
   char name[30];
};


typedef enum channel_e{
	NA=0,
	//DL
	NPSS,
	NSSS,
	NPBCH,
	NPDCCH,
	NPDSCH,
	//UL
	NPRACH,
	NPUSCH,

	channels_length
}channel_t;//unsigned int

channel_t *DL_Channle_bitmap;


int main()
{

	int subframes=0,NPDCCH_period=256;
	DL_Channle_bitmap=(channel_t *)calloc(NPDCCH_period,sizeof(channel_t));
	DL_Channle_bitmap[subframes]=NPBCH;

	LOG("subframes:%d,Channel:%s",subframes,DL_Channle_bitmap[subframes]);
	system("pause");
	free(DL_Channle_bitmap);

   struct person *ptr;
   int i, num;

   printf("Enter number of persons: ");
   scanf("%d", &num);

   ptr = (struct person*) malloc(num * sizeof(struct person));
   // Above statement allocates the memory for n structures with pointer personPtr pointing to base address */

   for(i = 0; i < num; ++i)
   {
       printf("Enter name, age and weight of the person respectively:\n");
       scanf("%s%d%f", &(ptr+i)->name, &(ptr+i)->age, &(ptr+i)->weight);
   }

   printf("Displaying Infromation:\n");
   for(i = 0; i < num; ++i)
       printf("%s\t%d\t%.2f\n", (ptr+i)->name, (ptr+i)->age, (ptr+i)->weight);

   return 0;
}
