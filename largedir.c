#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


static char *cmnd;

static void usage(void) {
    fprintf(stderr, "Usage: %s number_of_files ...\n", cmnd);
    exit(1);
}


int main(int argc, char **argv) {
    int i;


    cmnd = argv[0];

    if (argc == 1) {
	usage();
    }

    for (i = 1; i < argc; i++) {
	char dirname[1024];
	struct timeval tv;
	double t0, t1;
	int j, n;
	int *p;

	snprintf(dirname, sizeof(dirname), "largedir.%lu.%d",
		 (unsigned long)time(NULL), (int)getpid());
	if (mkdir(dirname, 0777) == -1) {
	    fprintf(stderr, "%s: mkdir(%s) failed: %s\n",
		    argv[0], dirname, strerror(errno));
	    exit(1);
	}
	if (chdir(dirname) == -1) {
	    fprintf(stderr, "%s: chdir(%s) failed: %s\n",
		    argv[0], dirname, strerror(errno));
	    exit(1);
	}
	n = strtoul(argv[i], NULL, 0);

	/*
	 * create $n$ files
	 */
	gettimeofday(&tv, NULL);
	t0 = tv.tv_sec + 1E-6 * tv.tv_usec;

	for (j = 0; j < n; j++) {
	    char filename[1024];
	    int fd;

	    snprintf(filename, sizeof(filename), "%d", j);
	    if ((fd = open(filename, O_CREAT | O_WRONLY, 0666)) == -1) {
		fprintf(stderr, "%s: open(%s, O_CREAT | O_WRONLY) failed: %s\n",
			argv[0], filename, strerror(errno));
		exit(1);
	    }
	    close(fd);
	}
		
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + 1E-6 * tv.tv_usec;

	printf ("create	%d	%g	%g\n", n, (t1 - t0), n / (t1 - t0));

	/*
	 * open/close them in random order
	 */
	p = malloc(n * sizeof(*p));
	assert(p);
	for (j = 0; j < n; j++) {
	    p[j] = j;
	}
	for (j = 0; j < n; j++) {
	    int k = (double)rand()*(n-1)/RAND_MAX;
	    int x;

	    assert(0 <= k && k < n);

	    x = p[j]; p[j] = p[k]; p[k] = x;
	}

	gettimeofday(&tv, NULL);
	t0 = tv.tv_sec + 1E-6 * tv.tv_usec;

	for (j = 0; j < n; j++) {
	    char filename[1024];
	    int fd;

	    snprintf(filename, sizeof(filename), "%d", p[j]);
	    /* printf("filename = %s\n", filename); */
	    fd = open(filename, O_RDONLY);
	    close(fd);
	}
		
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + 1E-6 * tv.tv_usec;

	printf ("open	%d	%g	%g\n", n, (t1 - t0), n / (t1 - t0));


	/*
	 * delete them.
	 */

	gettimeofday(&tv, NULL);
	t0 = tv.tv_sec + 1E-6 * tv.tv_usec;

	for (j = 0; j < n; j++) {
	    char filename[1024];
	    snprintf(filename, sizeof(filename), "%d", j);
	    if (unlink(filename) == -1) {
		fprintf(stderr, "%s: unlink(%s) failed: %s\n",
			argv[0], filename, strerror(errno));
		exit(1);
	    }
	}
		
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + 1E-6 * tv.tv_usec;

	printf ("delete	%d	%g	%g\n", n, (t1 - t0), n / (t1 - t0));
	if (chdir("..") == -1) {
	    fprintf(stderr, "%s: chdir(%s) failed: %s\n",
		    argv[0], "..", strerror(errno));
	    exit(1);
	}
	if (rmdir(dirname) == -1) {
	    fprintf(stderr, "%s: rmdir(%s) failed: %s\n",
		    argv[0], dirname, strerror(errno));
	    exit(1);
	}
    }

    return 0;

}
