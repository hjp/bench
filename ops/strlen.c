#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "my_strlen.h"

static void bench_strlen(size_t len) {
    char *str = malloc(len+1);
    assert(str);
    memset(str, 'a', len);
    str[len] = '\0';
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    printf("%lld %ld\n", (long long)t0.tv_sec, t0.tv_nsec);
    size_t n = 1000000000 / len;
    size_t s = 0; 
    for (size_t i = 0; i < n; i++) {
        s += my_strlen(str);
    }
    assert(s >= 1000000000 - len && s <= 1000000000);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("%lld %ld\n", (long long)t1.tv_sec, t1.tv_nsec);
    printf("%zu %g %g\n",
           len,
           ((t1.tv_sec - t0.tv_sec) * 1E9 + (t1.tv_nsec - t0.tv_nsec)) / n,
           ((t1.tv_sec - t0.tv_sec) * 1E9 + (t1.tv_nsec - t0.tv_nsec)) / s
    );

}

int main(void) {
    for (size_t len = 1; len < 1000000; len = len * 3 / 2 + 1) {
        bench_strlen(len);
    }
}
