#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


char *cmnd;

static void usage(void) {
    fprintf(stderr, "Usage: %s [-b buflen] [-l lengthofile] [-s] filename\n", cmnd);
    exit(1);
}
    

int main(int argc, char **argv) {
    int fd;
    size_t buflen = 0x1000;
    double maxlen = 1E9;
    int sync = 0;
    char *buf;
    struct timeval tv;
    double t0, t1;
    int r = 0;
    double len = 0;
    int c;


    cmnd = argv[0];

    while ((c = getopt(argc, argv, "b:l:s")) != EOF) {
	switch (c) {
	    case 'b':
		buflen = strtoul(optarg, NULL, 0);
		break;
	    case 'l':
		maxlen = strtod(optarg, NULL);
		break;
	    case 's':
		sync = 1;
		break;
	    case '?':
		usage();
	    default:
		assert(0);
	}
    }
    buf = malloc(buflen);
    assert(buf);

    if (optind != argc -1) usage();

    if ((fd = open(argv[optind], O_WRONLY | O_CREAT, 0666)) == -1)  {
	fprintf(stderr, "%s: cannot open %s for writing: %s\n",
		argv[0], argv[optind], strerror(errno));
       	exit(1);
    }
    gettimeofday(&tv, NULL);
    t0 = tv.tv_sec + tv.tv_usec * 1E-6;
    while (len < maxlen && (r = write(fd, buf, buflen)) > 0) {
	len += r;
    }
    if (r == -1) {
	fprintf(stderr, "%s: cannot write to %s: %s\n",
		argv[0], argv[optind], strerror(errno));
    }
    if (sync) fsync(fd);
    close(fd);
    gettimeofday(&tv, NULL);
    t1 = tv.tv_sec + tv.tv_usec * 1E-6;



    printf("%g bytes in %g seconds: %g bytes/second\n",
	    len, (t1 - t0), len / (t1 - t0));
    return 0;
}


