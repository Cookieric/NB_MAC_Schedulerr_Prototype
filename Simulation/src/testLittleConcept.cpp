#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	for (int i = 0; i < 10; ++i)
	{
		printf("%lf\n",(double)rand()/RAND_MAX );
	}
	// if(rand()/RAND_MAX)
	return 0;
}
