char diskbench_c_rcs_id [] =
	"$Id: diskbench.c,v 1.14 2016-01-09 00:43:00 hjp Exp $";
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
 * Revision 1.14  2016-01-09 00:43:00  hjp
 * Add incomplete seek_parallel bench
 *
 * I obviously started to add a parallel seek benchmark, but it doesn't look like
 * it does what it's supposed to do (it doesn't even fork).
 *
 * Revision 1.13  2016-01-09 00:34:29  hjp
 * Add parameter maxtime
 *
 * Revision 1.12  2015-07-25 11:10:48  hjp
 * diskbench: print filename and maxlen if verbose
 *
 * Revision 1.11  2010-05-12 13:08:49  hjp
 * no real change
 *
 * Revision 1.10  2004/04/05 19:41:29  hjp
 * include float.h for DBL_MAX (how often?)
 * shut up gcc.
 * be more friendly to other processes.
 *
 * Revision 1.9  2003/10/24 10:22:49  hjp
 * Added parameter maxlen to restrict length of test file.
 *
 * Revision 1.8  2003/10/06 16:08:44  hjp
 * 64 bit seeks.
 *
 * Revision 1.7  2003/01/31 12:07:19  hjp
 * Added largefile support (for real this time).
 *
 * Revision 1.6  2003/01/31 11:50:11  hjp
 * Changed verbose progress report from 1 dot per block to 1 line of
 * timing info per second.
 *
 * Revision 1.5  2002/10/01 18:13:23  hjp
 * Added large file support on systems which know O_LARGEFILE
 *
 * Fsync file after writing. This should make read performance somewhat
 * more reliable since flushing the write buffers and reading the file
 * won't overlap any more. It will also give "write to platter" instead
 * of "write to cache" performance figures for writing which may or may not
 * be what we want.
 *
 * Removed calibrating stage. With large disk caches this can be wildly
 * off. Better to check runtime after every write. The overhead shouldn't
 * be too bad.
 *
 * Revision 1.4  2001/03/07 15:42:55  hjp
 * Use GNUmake*s in Makefile
 * Use dirname of rundisk to determine path of bench
 * Removed <sys/stat.h> from diskbench.c for no apparent reason.
 *
 * Revision 1.3  2000/06/12 21:56:17  hjp
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

#define _LARGEFILE64_SOURCE 1

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <fcntl.h>
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


static double drand (double limit) {
	double	fact = limit / ((double) RAND_MAX + 1);
	double	rv = rand () * fact;
	return rv;
}


#ifndef O_BINARY
#	define O_BINARY 0
#endif

#ifndef O_LARGEFILE
#	define O_LARGEFILE 0
#endif

#define OWFLAGS (O_WRONLY|O_CREAT|O_BINARY|O_LARGEFILE)
#define ORFLAGS (O_RDONLY|O_LARGEFILE)

void seek_sequential(const char *testfile, int nr_seeks, double len) {
	int fd;
	double tr, tc;
	int i;
        resettimer ();
	if ((fd = open (testfile, ORFLAGS)) == -1) {
		printf ("Oops ! I cannot open the test file\n"
			"DOS says : \"%s\"\n", strerror (errno));
		return;
	}
	for (i = 0; i < nr_seeks; i ++) {
		off64_t	pos = drand (len);
		int     rc;
		char	buf [BUFSIZE];

		if (lseek64 (fd, pos, SEEK_SET) < 0) {
			printf ("cannot seek to position %ld\n"
			"OS says : \"%s\"\n", (long)pos, strerror (errno));
		}
		if ((rc = read (fd, buf, 1)) != 1) {
			if (rc == 0) { 
				printf ("unexpected EOF at %ld\n",
					(long)pos);
			} else {
				printf ("read error at position %ld\n"
				"OS says : \"%s\"\n", (long)pos, strerror (errno));
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

void seek_parallel(int parallel, const char *testfile, int nr_seeks, double len) {
	int fd;
	double tr, tc;
	int i;
        resettimer ();
	srand(getpid() << 17 ^ time(NULL));
	if ((fd = open (testfile, ORFLAGS)) == -1) {
		printf ("Oops ! I cannot open the test file\n"
			"DOS says : \"%s\"\n", strerror (errno));
		return;
	}
	for (i = 0; i < nr_seeks; i ++) {
		off64_t	pos = drand (len);
		int     rc;
		char	buf [BUFSIZE];

		if (lseek64 (fd, pos, SEEK_SET) < 0) {
			printf ("cannot seek to position %ld\n"
			"OS says : \"%s\"\n", (long)pos, strerror (errno));
		}
		if ((rc = read (fd, buf, 1)) != 1) {
			if (rc == 0) { 
				printf ("unexpected EOF at %ld\n",
					(long)pos);
			} else {
				printf ("read error at position %ld\n"
				"OS says : \"%s\"\n", (long)pos, strerror (errno));
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

void diskbench (char ***argvp)
{
	int	fd;
	static
	char	*testfile = "diskbench.tmp";
	static
	char	buf [BUFSIZE];
	double	maxlen = DBL_MAX;	/* max length of the test file	*/
	double	len = 0;		/* length of the test file	*/
	int	rc;
	double	maxtime = 60;	/* max time to write test file */
	double	tr, tc;
	int	ltr = 0;	/* last value of tr (rounded down to seconds */
	int	i;
	int 	nr_seeks;
	int     parallel;

	(*argvp)++;

	for (;;) {
		if (!**argvp) break;

		if (strcmp(**argvp, "filename") == 0) {
			(*argvp)++;
			testfile = (**argvp);
                        if (verbose) printf ("testfile: %s\n", testfile);
			(*argvp)++;
		} else if (strcmp(**argvp, "maxlen") == 0) {
			(*argvp)++;
			maxlen = strtod(**argvp, NULL);
                        if (verbose) printf ("maxlen: %g\n", maxlen);
			(*argvp)++;
		} else if (strcmp(**argvp, "maxtime") == 0) {
			(*argvp)++;
			maxtime = strtod(**argvp, NULL);
                        if (verbose) printf ("maxtime: %g\n", maxtime);
			(*argvp)++;
		} else if (strcmp(**argvp, "maxtime") == 0) {
			(*argvp)++;
			maxtime = strtod(**argvp, NULL);
			(*argvp)++;
		} else {
			break;
		}
	}

	#ifdef SIGXFSZ
	    signal(SIGXFSZ, SIG_IGN);
	#endif


        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("Disk Bench:\n");
        else printf ("disk	| ");
        /* Write test
         */
        if (verbose) printf ("Write test ...\n");
        resettimer ();
	if ((fd = open (testfile, OWFLAGS, 0666)) == -1) {
		printf ("Oops ! I cannot create the test file\nOS says : \"%s\"\n", strerror (errno));
			return;
	}

	len = 0;
	tr = 0;
	while (tr <= maxtime && len < maxlen && (rc = write (fd, buf, BUFSIZE)) > 0) {
	    len += rc;
	    gettimer (&tr, &tc);
	    if (verbose && (int)tr != ltr) {
		printf ("%g bytes written in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		ltr = tr;
		usleep(40000);
	    }
	}
	if (verbose) printf("F");
	fsync(fd);
	if (verbose) printf("C");
	close (fd);
	if (verbose) printf("\n");

	gettimer (&tr, &tc);

	if (verbose) {
		printf ("%g bytes written in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%g	| %g	| %g 	| %g	| ",
			len, tr, tc, 
			tr != 0 ? len/tr : 0.0 );
	}

        /* Read test
         */
        if (verbose) printf ("Read test ...\n");

        resettimer ();

	if ((fd = open (testfile, ORFLAGS)) == -1) {
		printf ("Oops ! I cannot open the test file\nOS says : \"%s\"\n", strerror (errno));
		return;
	}
	len = 0;
	while ((rc = read (fd, buf, BUFSIZE)) > 0) len += rc;
	close (fd);

        gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%g bytes read in %g seconds (%g bytes / second)\n",
			len, tr,
			tr != 0 ? len/tr : 0.0 );
		printf ("CPU time: %g\n", tc);
	} else {
		printf ("%g	| %g	| %g 	| %g	| ",
			len, tr, tc, 
			tr != 0 ? len/tr : 0.0 );
	}

        /* need to calibrate for seeks again since seek speed is
         * independent of read and write speed.
         */
        if (verbose) printf ("Calibrating ...\n");
        resettimer ();

	if ((fd = open (testfile, ORFLAGS)) == -1) {
		printf ("Oops ! I cannot open the test file\n"
			"DOS says : \"%s\"\n", strerror (errno));
		return;
	}

	for (i = 0, tr = 0; tr < 5.0; i ++) {
		off64_t	pos = drand (len);

		if (lseek64(fd, pos, SEEK_SET) < 0) {
			printf ("cannot seek to position %ld\n"
			"OS says : \"%s\"\n", (long)pos, strerror (errno));
		}
		if ((rc = read (fd, buf, 1)) != 1) {
			if (rc == 0) { 
				printf ("unexpected EOF at %ld\n",
					(long)pos);
			} else {
				printf ("read error at position %ld\n"
				"OS says : \"%s\"\n", (long)pos, strerror (errno));
			}
		}
		gettimer (&tr, &tc);
	}

	close (fd);

	nr_seeks = i * 60.0 / tr > INT_MAX
			? INT_MAX
			: i * 60.0 / tr;



        if (verbose) printf ("Seek test ...\n");

	if (parallel) {
		seek_parallel(parallel, testfile, nr_seeks, len);
	} else {
		seek_sequential(testfile, nr_seeks, len);
	}
}

