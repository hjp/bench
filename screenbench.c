#include <stdio.h>

#include "timer.h"
#include "bench.h"
#include "screenbench.h"


/*
	define some constants
*/

#define MAXLINES	500
#define MAXCHAR		50

/*
	screenbench

	write characters to screen via putc

	Possibly unportable assumptions:

	1) stderr refers to a terminal.
	2) all characters between 'a' and 'z' are printable
	3) buffering strategies don't neglible performance-wise.
	   This is almost certainly wrong for memory-mapped
	   terminals where the system-call overhead does matter
	   and it may also be wrong for network connections, where
	   it makes some difference whether each character gets 
	   sent in a packet of its own.

	typical throughputs:

	Computer
	------------------------------------
	Schneider PC 1512 (putch)	 740
	Escom 386-25			1100
	Escom 386-25 (ansi.sys)		 720
	Escom 386-25 (nnansi.sys)	3300
	Escom 386-25 (ix/386)		5000
	Escom 386-25 (Minix 1.3)       17000
	Escom 386-25 (Minix 1.5)       32000
	Escom 386-25 (Minix-386)       30000
*/


void screenbench (char ***argvp)
{
	int	i, j;
	int	c;
        double	tr, tc;

        (*argvp)++;

        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("Screen Bench:\n");
        else printf ("screen	| ");

	resettimer ();
	c = 'a';
	for (i = 0; i < MAXLINES; i ++) {
		for (j = 0; j < MAXCHAR; j ++) {
			putc (c, stderr);
			c ++; if (c > 'z') c = 'a';
		}
		putc ('\n', stderr);
	}

	gettimer (&tr, &tc);
	if (verbose) {
		printf ("\n%ld bytes written in %g seconds (%g bytes / second)\n",
			(long)MAXCHAR * MAXLINES, tr,
			tr ? (long)MAXCHAR * MAXLINES / tr : 0.0);
		printf ("CPU time: %g seconds\n", tc);
	} else {
		printf ("%ld	| %g	| %g	| %g",
			(long)MAXCHAR * MAXLINES, tr, tc,
			tr ? (long)MAXCHAR * MAXLINES / tr : 0.0);
	}
	printf("\n");
}
