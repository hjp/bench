#include <stdio.h>
#include <ant/time.h>
#include <sys/time.h>

int main(void) {
    int max = 1;
    double t = 0, tc = 0;

    int i;

    do {
	max = 2 * max;
	resettimer();
	for (i = 0; i < max; i++) {
            struct timeval tv;
	    gettimeofday(&tv, NULL);
	}
	gettimer(&t, &tc);
    } while (t < 1 && 2 * max > max);
    printf("max = %d t = %g rate= %g\n", max, t, max/t);
    return 0;
}
