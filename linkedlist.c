#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


void **list;
void **tail;

size_t build_list(size_t n) {
    size_t i;

    for (i = 0; i < n; i++) {
	void **p = malloc(sizeof(void *));
	if (!p) return i;
	if (!list) {
	    list = p;
	} else {
	    *tail = p;
	}
	tail = p;
    }
    return i;
}

void walk_list(void) {
    void **p = list;
    while (*p) p = *p;
}

void free_list(void) {
    void **p = list;
    while (p) {
	void **next = *p;
	free(p);
	p = next;
    }
    list = NULL;
}

double gettimestamp(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec*1E-6;
}

int main (int argc, char **argv) {
    size_t n;
    for (n = 1; ; n <<=1) {
	double t0, t1;
	size_t n1;


	printf("build list of size %d ... ", n); fflush(stdout);
	t0 = gettimestamp();
	n1 = build_list(n);
	t1 = gettimestamp();
	printf("%g seconds\n", t1-t0);

	printf("walking list of size %d ... ", n1); fflush(stdout);
	t0 = gettimestamp();
	walk_list();
	t1 = gettimestamp();
	printf("%g seconds\n", t1-t0);

	printf("freeing list of size %d ... ", n1); fflush(stdout);
	t0 = gettimestamp();
	free_list();
	t1 = gettimestamp();
	printf("%g seconds\n", t1-t0);

	if (n1 != n) break;

    }
    return 0;
}
