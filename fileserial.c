char diskbench_c_rcs_id [] =
	"$Id: fileserial.c,v 1.1 2001-10-31 10:46:33 hjp Exp $";
/*
 *	diskbench
 *
 *	measure disk i/o
 *
 *	create a file of length MAXLEN (or available disk space).
 *	print time needed to write and read this file sequentially
 *	and for 10000 1 byte reads in random positions in this file.
 *
 *	see diskbench.notes for typical throughputs [kB/s]:
 *
 * $Log: fileserial.c,v $
 * Revision 1.1  2001-10-31 10:46:33  hjp
 * Committing fileserial.c
 *
 * Revision 1.2  1998/02/21 18:58:38  hjp
 * fixed typo in diskbench.c
 * Added shell script to run disk benches.
 *
 * Revision 1.1.1.1  1996/01/04  11:20:31  hjp
 *
 * Revision 1.2  1996/01/03  23:13:25  hjp
 * Changed usage from menu- to commandline-driven.
 * Added terse output format.
 * Cleanup.
 *
 * Revision 1.1.1.1  1996/01/03  20:18:55  hjp
 *
 * Revision 1.6  1995/04/30  15:57:16  hjp
 * Isolated UNIXisms. Should now work without changes on all Posix and
 * MSDOS systems (except for terminal handling, which is still hard coded).
 *
 * Revision 1.5  1995/04/30  14:52:03  hjp
 * Ported to HP-UX
 *
 * Revision 1.4  1994/11/14  11:29:20  hjp
 * Fixed overflow in diskbench.c (well, circumvented it and guarded it
 * by a comment).
 * screenbench.notes now has full name.
 *
 * Revision 1.3  1994/03/13  16:51:21  hjp
 * prototype headers, made more portable
 *
 * Revision 1.1  1994/03/04  13:10:38  hp
 * Initial revision
 *
 * Revision 1.2  1993/12/10  12:40:35  hjp
 * Added seek benchmark
 * some cleanup
 *
 */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "unixio.h"
#include "timer.h"

#include "bench.h"
#include "diskbench.h"


#ifdef O_BINARY
#define OFLAGS (O_WRONLY|O_CREAT|O_BINARY)
#else
#define OFLAGS (O_WRONLY|O_CREAT)
#endif

char *cmnd;

char *id = NULL;
int verbose = 0;

void usage(void) {
    fprintf(stderr, "Usage: %s filename, bufsize, filesize\n", cmnd);
    exit(1);
}


void fileserial (char *testfile, size_t bufsize, unsigned long filesize)
{
	int	fd;
	static char	*buf;
	unsigned long	len = 0;		/* length of the test file	*/
	int	rc;
	double	tr, tc;
	unsigned long	maxlen;


	buf = malloc(bufsize);
	if (!buf) {
	    fprintf(stderr, "%s: cannot malloc %lu bytes\n", 
	    	    cmnd, (unsigned long)bufsize);
	    exit(1);
	}

        /* First get a rough estimate on the speed of the
         * device by writing a file for 5 seconds.
         */

        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("Disk Bench:\n");
        else printf ("disk	| ");
        if (verbose) printf ("Calibrating ...\n");
        resettimer ();
	if ((fd = open (testfile, OFLAGS, 0666)) == -1) {
		printf ("Oops ! I cannot create the test file\nOS says : \"%s\"\n", strerror (errno));
			return;
	}

	while ((rc = write (fd, buf, bufsize)) > 0) {
		len += rc;
		gettimer (&tr, &tc);
		if (tr > 5.0) break;
	}
	close (fd);
	remove(testfile);

	/* The real test should take about 1 minute */
	maxlen = len * 60.0 / tr > filesize
			? filesize
			: len * 60.0 / tr;

        fprintf(stderr, "len: %lu tr: %g filesize: %lu maxlen: %lu\n",
		len, tr, filesize, maxlen);

        /* Write test
         */
        if (verbose) printf ("Write test ...\n");
        resettimer ();
	if ((fd = open (testfile, OFLAGS, 0666)) == -1) {
		printf ("Oops ! I cannot create the test file\nOS says : \"%s\"\n", strerror (errno));
			return;
	}

	len = 0;
	while (len < maxlen && (rc = write (fd, buf, bufsize)) > 0) len += rc;
	close (fd);

	gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%ld bytes written in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%lu	| %lu	| %g	| %g 	| %g	| ",
			len, (unsigned long) bufsize, tr, tc, 
			tr != 0 ? len/tr : 0.0 );
	}

        /* Read test
         */
        if (verbose) printf ("Read test ...\n");

        resettimer ();

	if ((fd = open (testfile, O_RDONLY)) == -1) {
		printf ("Oops ! I cannot open the test file\nOS says : \"%s\"\n", strerror (errno));
		return;
	}
	len = 0;
	while ((rc = read (fd, buf, bufsize)) > 0) len += rc;
	close (fd);

        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%ld bytes read in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%ld	| %lu	| %g	| %g 	| %g	| ",
			len, (unsigned long)bufsize, tr, tc, 
			tr != 0 ? len/tr : 0.0 );
	}

	printf("\n");

	unlink (testfile);
}


int main(int argc, char **argv) {
    cmnd = argv[0];

    if (argc != 4) usage();
    fileserial(argv[1], strtoul(argv[2], 0, 0), strtoul(argv[3], 0, 0));
    return 0;
}

