#include <ant/time.h>

extern void dummy(double d);

int main(void) {
    int max = 1;
    double t = 0, tc = 0;

    int i;

    do {
	double s;
	max = 2 * max;
	resettimer();
	s = 0;
	for (i = 0; i < max; i++) {
	    s++;
	}
	gettimer(&t, &tc);
	dummy(s);
    } while (t < 1 && 2 * max > max);
    printf("max = %d t = %g rate= %g\n", max, t, max/t);
    return 0;
}
