#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    int fd;
    char buf[1024*1024];
    struct timeval tv0, tv1;
    double off;
    double dt;
    int rc;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
	fprintf(stderr, "%s: cannot open %s: %s\n",
	        argv[0], argv[1], strerror(errno));
	exit(1);
    }
    for (off = 0; ; off += rc) {
	gettimeofday(&tv0, NULL);
	rc = read(fd, buf, sizeof(buf));
	gettimeofday(&tv1, NULL);
	if (rc <= 0) break;

	dt = (tv1.tv_sec + tv1.tv_usec * 1E-6) -
	     (tv0.tv_sec + tv0.tv_usec * 1E-6);
	printf("%.0f %d %g %g\n", off, rc, dt, rc/dt);
    }
    return 0;
}
