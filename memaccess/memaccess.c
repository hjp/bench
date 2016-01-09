#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>


char *cmnd;

void usage(void) {
        fprintf(stderr, "Usage: %s size [n]\n", cmnd);
        exit(1);
}

void bench(size_t size, int flags, long count, size_t stride) {
        char *p = mmap(NULL, size,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | flags,
                       -1, 0);
        if (p == MAP_FAILED) {
            fprintf(stderr, "%s: cannot mmap %lu bytes: %s\n",
                    cmnd, (unsigned long)size, strerror(errno));
            exit(1);
        }
        int s = 0;
        size_t off = 0;
        struct timeval tv0, tv1;
        gettimeofday(&tv0, NULL);

        for (int long i = 0; i < count; i++) {
            s += p[off];
            off += stride;
            if (off >= size) off -= size;
        }
        gettimeofday(&tv1, NULL);
        double dt = (tv1.tv_sec - tv0.tv_sec) * 1E6 + (tv1.tv_usec - tv0.tv_usec);
        double ct = dt / count;
        printf("%zd %d %zd %ld %g %g %d\n", size, flags, stride, count, dt, ct, s);
        munmap(p, size);
}

int main(int argc, char **argv) {
    bench(      4096,           0, 1000000000,      1);
    bench(      4096,           0, 1000000000,   2333);
    bench(   4194304,           0, 1000000000,      1);
    bench(   4194304,           0, 1000000000,   2333);
    bench(   4194304,           0, 1000000000, 999809);
    bench(   4194304, MAP_HUGETLB, 1000000000,      1);
    bench(   4194304, MAP_HUGETLB, 1000000000,   2333);
    bench(   4194304, MAP_HUGETLB, 1000000000, 999809);
    bench(4294967296,           0, 1000000000,      1);
    bench(4294967296,           0, 1000000000,   2333);
    bench(4294967296,           0, 1000000000, 999809);
    bench(4294967296, MAP_HUGETLB, 1000000000,      1);
    bench(4294967296, MAP_HUGETLB, 1000000000,   2333);
    bench(4294967296, MAP_HUGETLB, 1000000000, 999809);
}

