char diskbench_c_rcs_id [] =
	"$Id: diskbench.c,v 1.3 2000-06-12 21:56:17 hjp Exp $";
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
 * $Log: diskbench.c,v $
 * Revision 1.3  2000-06-12 21:56:17  hjp
 * *** empty log message ***
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

/*
	define some constants
*/

#if INT_MAX < 0x8000
/* what we really would like to test here is SIZE_T_MAX, but we can't
 * do that in the preprocessor :-(
 */
#define BUFSIZE		0x4000
#else
#define BUFSIZE		0x10000
#endif


long lrand (long limit) {
	double	fact = limit / ((double) RAND_MAX + 1);
	long	rv = rand () * fact;
	return rv;
}


#ifdef O_BINARY
#define OFLAGS (O_WRONLY|O_CREAT|O_BINARY)
#else
#define OFLAGS (O_WRONLY|O_CREAT)
#endif


void diskbench (char ***argvp)
{
	int	fd;
	static
	char	*testfile = "diskbench.tmp";
	static
	char	buf [BUFSIZE];
	long	len = 0;		/* length of the test file	*/
	int	rc;
	double	tr, tc;
	int	i;
	long	maxlen;
	int 	nr_seeks;

	(*argvp)++;

	if (**argvp && strcmp(**argvp, "filename") == 0) {
		(*argvp)++;
		testfile = (**argvp);
		(*argvp)++;
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

	while ((rc = write (fd, buf, BUFSIZE)) > 0) {
		len += rc;
		gettimer (&tr, &tc);
		if (tr > 5.0) break;
	}
	close (fd);
	remove(testfile);

	/* The real test should take about 1 minute */
	maxlen = len * 60.0 / tr > LONG_MAX
			? LONG_MAX
			: len * 60.0 / tr;


        /* Write test
         */
        if (verbose) printf ("Write test ...\n");
        resettimer ();
	if ((fd = open (testfile, OFLAGS, 0666)) == -1) {
		printf ("Oops ! I cannot create the test file\nOS says : \"%s\"\n", strerror (errno));
			return;
	}

	len = 0;
	while (len < maxlen && (rc = write (fd, buf, BUFSIZE)) > 0) {
	    if (verbose) putchar('.');
	    len += rc;
	}
	close (fd);
	if (verbose) printf("C\n");

	gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%ld bytes written in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%ld	| %g	| %g 	| %g	| ",
			len, tr, tc, 
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
	while ((rc = read (fd, buf, BUFSIZE)) > 0) len += rc;
	close (fd);

        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%ld bytes read in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%ld	| %g	| %g 	| %g	| ",
			len, tr, tc, 
			tr != 0 ? len/tr : 0.0 );
	}

        /* need to calibrate for seeks again since seek speed is
         * independent of read and write speed.
         */
        if (verbose) printf ("Calibrating ...\n");
        resettimer ();

	if ((fd = open (testfile, O_RDONLY)) == -1) {
		printf ("Oops ! I cannot open the test file\n"
			"DOS says : \"%s\"\n", strerror (errno));
		return;
	}

	for (i = 0, tr = 0; tr < 5.0; i ++) {
		long	pos = lrand (len);

		if (lseek (fd, pos, SEEK_SET) < 0) {
			printf ("cannot seek to position %ld\n"
			"DOS says : \"%s\"\n", (long)pos, strerror (errno));
		}
		if ((rc = read (fd, buf, 1)) != 1) {
			if (rc == 0) { 
				printf ("unexpected EOF at %ld\n",
					(long)pos);
			} else {
				printf ("read error at position %ld\n"
				"DOS says : \"%s\"\n", (long)pos, strerror (errno));
			}
		}
		gettimer (&tr, &tc);
	}

	close (fd);

	nr_seeks = i * 60.0 / tr > INT_MAX
			? INT_MAX
			: i * 60.0 / tr;



        if (verbose) printf ("Seek test ...\n");
        resettimer ();

	if ((fd = open (testfile, O_RDONLY)) == -1) {
		printf ("Oops ! I cannot open the test file\n"
			"DOS says : \"%s\"\n", strerror (errno));
		return;
	}
	for (i = 0; i < nr_seeks; i ++) {
		long	pos = lrand (len);

		if (lseek (fd, pos, SEEK_SET) < 0) {
			printf ("cannot seek to position %ld\n"
			"DOS says : \"%s\"\n", (long)pos, strerror (errno));
		}
		if ((rc = read (fd, buf, 1)) != 1) {
			if (rc == 0) { 
				printf ("unexpected EOF at %ld\n",
					(long)pos);
			} else {
				printf ("read error at position %ld\n"
				"DOS says : \"%s\"\n", (long)pos, strerror (errno));
			}
		}
	}

	close (fd);

        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%d seeks in %g seconds (%g seeks / second)\n",
			nr_seeks, tr,
			tr != 0 ? nr_seeks/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%d	| %g	| %g 	| %g	| ",
			nr_seeks, tr, tc, 
			tr != 0 ? nr_seeks/tr : 0.0 );
	}
	printf ("\n");

	unlink (testfile);
}
