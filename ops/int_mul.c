#include <stdio.h>
#include <ant/time.h>

extern void dummy(double d);

int main(void) {
    unsigned max = 1;
    double t = 0, tc = 0;

    do {
	int s;
	max = 2 * max;
	resettimer();
	s = 0;
	for (unsigned i = 0; i < max; i++) {
            for (unsigned j = 0; j < max; j++) {
                s += i * j;
            }
	}
	gettimer(&t, &tc);
	dummy(s);
    } while (t < 1 && 2 * max > max);
    printf("max = %d t = %g rate= %g\n", max, t, (double)max*max/t);
    return 0;
}
