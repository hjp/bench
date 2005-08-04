#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

char *cmnd;

void usage(void) {
    fprintf(stderr, "Usage: %s [-w] [-r] [-b blocksize] device\n", cmnd);
    exit(1);
}

int main(int argc, char **argv) {
    int fd;
    size_t blocksize = 1024*1024;
    int do_write = 0;
    int random = 0;
    char *buf;
    struct timeval tv0, tv1;
    double off;
    double dt;
    int rc;
    int c;
    int i;

    cmnd = argv[0];

    while ((c = getopt(argc, argv, "wrb:")) != EOF) {
	switch(c) {
	    case 'w':
		do_write = 1;
		break;
	    case 'r':
		random = 1;
		break;
	    case 'b':
		blocksize = strtoul(optarg, NULL, 0);
		buf = malloc(blocksize);
		if (!buf) {
		    fprintf(stderr, "%s: cannot malloc %ul bytes for buffer\n", argv[0], (unsigned long)blocksize);
		    exit(1);
		}
	    case '?':
		usage();
	    default:
		assert(0);
	}
    }
    if (random) {
	for (i = 0; i < blocksize; i++) {
	    buf[i] = rand();
	}
    } else {
	memset(buf, 0, blocksize);
    }
    if (optind != argc - 1) usage();

    if (do_write) {
	if ((fd = open(argv[optind], O_WRONLY)) == -1) {
	    fprintf(stderr, "%s: cannot open %s: %s\n",
		    argv[0], argv[1], strerror(errno));
	    exit(1);
	}
	for (off = 0; ; off += rc) {
	    gettimeofday(&tv0, NULL);
	    rc = write(fd, buf, sizeof(buf));
	    gettimeofday(&tv1, NULL);
	    if (rc <= 0) break;

	    dt = (tv1.tv_sec + tv1.tv_usec * 1E-6) -
		 (tv0.tv_sec + tv0.tv_usec * 1E-6);
	    printf("%.0f %d %g %g\n", off, rc, dt, rc/dt);
	}
    } else {
	if ((fd = open(argv[optind], O_RDONLY)) == -1) {
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
    }
    return 0;
}
