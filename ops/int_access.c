#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    if (argc != 2) {
	fprintf(stderr, "Usage: %s bytes\n", argv[0]);
	exit(1);
    }
    size_t n = strtoul(argv[1], NULL, 0) / sizeof(int);

    int *ip = malloc(n * sizeof(*ip));
    if (!ip) {
	fprintf(stderr, "%s: cannot malloc %lu ints: %s\n",
		argv[0], (unsigned long)n, strerror(errno));
	exit(1);
    }
    int **pp = malloc(n * sizeof(*pp));
    if (!pp) {
	fprintf(stderr, "%s: cannot malloc %lu pointers: %s\n",
		argv[0], (unsigned long)n, strerror(errno));
	exit(1);
    }
    for (size_t i = 0; i < n; i++) {
	ip[i] = i;
	pp[i] = &ip[i];
    }
    assert(RAND_MAX >= n);
    for (size_t i = 0; i < n; i++) {
	int *ap = pp[i];
	size_t j = (double)rand() * n / (RAND_MAX+1.0);
	pp[i] = pp[j];
	pp[j] = ap;
    }
    struct timeval t0;
    gettimeofday(&t0, NULL);
    int s = 0;
    for (size_t i = 0; i < n; i++) {
	//printf("%p\n", pp[i]);
	s += *pp[i];
    }
    struct timeval t1;
    gettimeofday(&t1, NULL);
    double t = t1.tv_sec - t0.tv_sec + (t1.tv_usec - t0.tv_usec) * 1E-6;
    printf("sum is = %d, %lu accesses in %g seconds: %g seconds per access\n", 
    	   s, (unsigned long)n, t, t / n);
    return 0;
}
