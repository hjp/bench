#include <stdio.h>
#include <time.h>

int main(void) {

    time_t t0 = time(NULL);
    long n;

    setbuf(stdout, NULL);
    for(n = 4;; n++) {
	long ts = 0;
	long j;
	for (j = 1; j < n; j++) {
	    if (n % j == 0) {
		ts += j;
	    }
	}
	if (n % 100 == 0) {
	    time_t t1;
	    printf("%ld\r", n);
	    t1 = time(NULL);
	    if (difftime(t1, t0) >= 60) {
		printf("%ld %.0f %.0f nps\n",
		       n, difftime(t1, t0), n/difftime(t1, t0));
		exit(0);
	    }
	}
	if (n == ts) printf("%ld %ld\n", n, ts);
    }
}
