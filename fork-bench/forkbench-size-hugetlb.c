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

int main(int argc, char **argv) {
        int i;
        size_t size;
	int n = 1;
	int f = 0;
	int s = 0;
	struct timeval tv0, tv1;
        void *p;
	cmnd = argv[0];

	if (argc >= 2) {
		size = strtoul(argv[1], NULL, 0);
	} else {
		usage();
	}
	if (argc >= 3) {
		n = strtoul(argv[2], NULL, 0);
	}
        p = mmap(NULL, size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                 -1, 0);
        if (!p) {
            fprintf(stderr, "%s: cannot mmap %lu bytes: %s\n",
                    cmnd, (unsigned long)size, strerror(errno));
            exit(1);
        }
        setlinebuf(stdout);
        memset(p, 1, size);
	for (i = 0; i < n; i++) {
                gettimeofday(&tv0, NULL);
		pid_t pid;
                switch (pid = fork()) {
		case -1:
			f++;
			break;
		case 0:
                        gettimeofday(&tv1, NULL);
                        printf("size %lu run %d after fork successful %d failed %d time %g\n",
                                        (unsigned long)size, i, s, f,
                                        (tv1.tv_sec + tv1.tv_usec * 1E-6) -
                                        (tv0.tv_sec + tv0.tv_usec * 1E-6)
                              );
			exit(0);
		default: ;
                        int rc;
                        int status;
			while ((rc = wait(&status)) > 0);
                        // printf("rc = %d, status = %x\n", rc, status);
			s++;
		}
                gettimeofday(&tv1, NULL);
                printf("size %lu run %d after wait successful %d failed %d time %g\n",
                                (unsigned long)size, i, s, f,
                                (tv1.tv_sec + tv1.tv_usec * 1E-6) -
                                (tv0.tv_sec + tv0.tv_usec * 1E-6)
                      );
	}
	exit(0);
}
