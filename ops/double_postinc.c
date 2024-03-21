#include <stdio.h>
#include <time.h>

extern void dummy(double d);

int main(void) {
    int max = 1;
    double t = 0;
    struct timespec t0, t1;

    int i;

    do {
	double s;
	max = 2 * max;
	clock_gettime(CLOCK_MONOTONIC, &t0);
	s = 0;
	for (i = 0; i < max; i++) {
	    s++;
	}
	clock_gettime(CLOCK_MONOTONIC, &t1);
        t = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) * 1E-9;
	dummy(s);
    } while (t < 1 && 2 * max > max);
    printf("max = %d t = %g rate= %g\n", max, t, max/t);
    return 0;
}
