#include <time.h>
#include <stdio.h>

int main()
{
	struct timespec res;
	struct timespec start, finish;
	int i,j;

	if (!clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res))
		printf("Resolution of realtime timer is: %ldns\n", res.tv_nsec);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (i=0; i<1000000; i++) {
		j = i%33;	
	}

	if (!clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finish))
		printf("loop time: %.2fms\n",(float)(finish.tv_nsec-start.tv_nsec)/1000000);

	return 0;
}

