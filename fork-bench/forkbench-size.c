#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

char *cmnd;

void usage(void) {
        fprintf(stderr, "Usage: %s size1 size2 ...\n", cmnd);
        exit(1);
}

int main(int argc, char **argv) {
        int i;
	int n = 1;
	int f = 0;
	int s = 0;
        void *p = NULL;
        size_t last_size = 0;
        char **arg;

	cmnd = argv[0];

	if (argc <= 1) {
		usage();
	}
        for (arg = argv+1; *arg; arg++) {
                struct timeval tv0, tv1, tv2;
                size_t size;
                pid_t pid;
                int pip[2];
                char buf[1];

		size = strtoul(*arg, NULL, 0);
                p = realloc(p, size);
                if (!p) {
                    fprintf(stderr, "%s: cannot malloc %lu bytes: %s\n",
                            cmnd, (unsigned long)size, strerror(errno));
                    exit(1);
                }
                if(size > last_size) {
                        memset(p + last_size, 1, size - last_size);
                }
                gettimeofday(&tv0, NULL);
                switch (pid = fork()) {
                case -1:
                        fprintf(stderr, "%s: cannot fork: %s\n",
                                cmnd, strerror(errno));
                        exit(1);
                        break;
                case 0:
                        write(pip[1], "f", 1);
                        exit(0);
                default:
                        read(pip[0], buf, 1);
                        gettimeofday(&tv1, NULL);
                        while (wait3(NULL, WNOHANG, NULL) > 0);
                        s++;
                }
                gettimeofday(&tv2, NULL);
                printf("%lu %g %g\n",
                                (unsigned long)size,
                                (tv1.tv_sec + tv1.tv_usec * 1E-6) -
                                (tv0.tv_sec + tv0.tv_usec * 1E-6),
                                (tv2.tv_sec + tv2.tv_usec * 1E-6) -
                                (tv0.tv_sec + tv0.tv_usec * 1E-6)
                      );
        }
	exit(0);
}
