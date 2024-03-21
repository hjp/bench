#include <stdio.h>
#include <time.h>

extern void dummy(double d);

int main(void) {
    unsigned max = 1;
    double t = 0;
    struct timespec t0, t1;

    do {
	int s;
	max = 2 * max;
	clock_gettime(CLOCK_MONOTONIC, &t0);
	s = 0;
	for (unsigned i = 0; i < max; i++) {
            for (unsigned j = 0; j < max; j++) {
                s += i * j;
            }
	}
	clock_gettime(CLOCK_MONOTONIC, &t1);
        t = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) * 1E-9;
	dummy(s);
    } while (t < 1 && 2 * max > max);
    printf("max = %d t = %g rate= %g\n", max, t, (double)max*max/t);
    return 0;
}
