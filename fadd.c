#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "timer.h"
#include "bench.h"
#include "fadd.h"



void faddbench (char ***argvp)
{
	double	r = 0;
	int	i;
	double	tr, tc;
	int	n = 100000000;

        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("Fadd Bench:\n");
        else printf ("fadd	| ");

	(*argvp)++;

	resettimer ();
	for (i = 0; i < n; i++) {
	    r ++;
	}

	gettimer (&tr, &tc);

	if (verbose) {
		printf( "\n%d double additions (result = %20g) in %g seconds\n",
			n, r, tr);
		printf ("CPU time = %g seconds\n", tc);
	} else {
		printf( "%d	| %g	| %g", n, tr, tc);
	}
	printf ("\n");
}
