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
    struct timeval t0;
    gettimeofday(&t0, NULL);
    int s = 0;
    for (size_t i = 0; i < n; i++) {
	ip[i] = rand();
    }
    struct timeval t1;
    gettimeofday(&t1, NULL);
    double t = t1.tv_sec - t0.tv_sec + (t1.tv_usec - t0.tv_usec) * 1E-6;
    printf("%lu random numbers in %g seconds: %g seconds per random numbers\n", 
    	   (unsigned long)n, t, t / n);
    return 0;
}
