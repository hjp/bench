#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N (10*1000*1000)

#define BENCH(func) \
    do {	\
	gettimeofday(&tv0, NULL);	\
	for (i = 0; i < N; i++) {	\
	    func;	\
	}	\
	gettimeofday(&tv1, NULL);	\
	t = tv1.tv_sec - tv0.tv_sec + 	\
	    (tv1.tv_usec - tv0.tv_usec) * 1E-6;	\
	printf("%s: %d calls, %g seconds, %g seconds/call\n",	\
	      #func, N, t, t/N);	\
    } while (0)

int main(void) {
    struct timeval tv0, tv1;
    int i;
    double t;

    BENCH(rand());
    BENCH(getpid());
    BENCH(time(NULL));
    BENCH(clock());
    BENCH(srand(0));
    BENCH(srand(1));


    return 0;
}
