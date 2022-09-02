#include <stdio.h>
#include <intrin.h>
// --------------------------------------------------- 
// Early binding vs late binding
// Performance test
// --------------------------------------------------- 
#define CC __stdcall

typedef void (CC *fn_ptr_t)();

volatile void CC function1()
{
}

volatile fn_ptr_t func_ptr = function1;

#define CALLS_NUMBER 10000

#define ABS(x) ((x < 0) ? -x : x)

int main()
{
	long long diff;
	unsigned long long time, eloss, lloss;

	printf("Early binding: ");
	time = __rdtsc();
	for (size_t i = 0; i < CALLS_NUMBER; i++)
		function1();
	
	eloss = __rdtsc() - time;
	printf("\t%lld tsc loss\n", eloss);

	// ----- 
	printf("Late binding: ");
	time = __rdtsc();
	for (size_t i = 0; i < CALLS_NUMBER; i++)
		func_ptr();

	lloss = __rdtsc() - time;
	printf("\t%lld tsc loss\n", lloss);

	// -----
	diff = lloss - eloss;
	printf("%s is win!\n", (diff < 0) ? "Late binding" : "Early binding");
	printf("%lld tsc diff\n", ABS(diff));
	return 0;
}