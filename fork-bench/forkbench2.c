#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/time.h>

char *cmnd;

void usage(void) {
        fprintf(stderr, "Usage: %s n\n", cmnd);
        exit(1);
}

int main(int argc, char **argv) {
        int i;
	int n;
	int f = 0;
	int s = 0;
	struct timeval tv0, tv1;
	cmnd = argv[0];

	if (argc == 2) {
		n = strtoul(argv[1], NULL, 0);
	} else {
		usage();
	}
	gettimeofday(&tv0, NULL);
	for (i = 0; i < n; i++) {
		pid_t pid;
                switch (pid = fork()) {
		case -1:
			f++;
			break;
		case 0:
			exit(0);
		default:
			while (wait3(NULL, WNOHANG, NULL) > 0);
			s++;
		}
	}
	gettimeofday(&tv1, NULL);
	printf("%d successful forks, %d failures: %g seconds\n",
			s, f,
			(tv1.tv_sec + tv1.tv_usec * 1E-6) -
			(tv0.tv_sec + tv0.tv_usec * 1E-6)
	      );
	exit(0);
}
