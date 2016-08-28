#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <sys/time.h>

extern int my_atoi(const char *s);

int main(void) {
    char tests[65536][7];

    assert(INT_MAX > 32767);
    for (int i = -32768; i <= 32767; i++) {
        sprintf(tests[i+32768], "%d", i);
    }

    assert(INT_MIN <= (-32768 - 1) * 16384);

    struct timeval t0;
    gettimeofday(&t0, NULL);
    for (int j = 0; j < 1000; j++) {
        int s = 0;
        for (int i = -32768; i <= 32767; i++) {
            s += my_atoi(tests[i+32768]);
        }
        assert(s == -32768);
    }
    struct timeval t1;
    gettimeofday(&t1, NULL);
    double dt = t1.tv_sec - t0.tv_sec + (t1.tv_usec - t0.tv_usec) * 1E-6;
    printf("%g seconds, %g seconds/op\n", dt, dt/65536000);

}

