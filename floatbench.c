#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "timer.h"
#include "bench.h"
#include "floatbench.h"


/*
	compute Mandelbrot set.
		Area = (-2, -2) .. (2, 2);
		Step = (0.01, 0.01);
		Iterations = 32;

	Typical times see floatbench.notes

	NOTE:	When I wrote this benchmark I had just bought my 386 computer
		and many people still had 8088s. FPUs were rare and the
		long time time the benchmark was running (over 3 minutes
		on my 25 MHz 386, over half an hour (or almost an hour?
		I don't remember) on a 4.77 MHz 8088) made it necessary
		to include some kind of progress indicator. Now the
		benchmark completes in well under one second on most
		machines and I fear it is mostly measuring the speed of
		printing FP numbers.
	NOTE:	I changed the step width. Originally it was 0.04, now
		it is 0.01. This should increase the run times by a 
		factor of about 16. I am including the step width in 
		the notes from now on since it is expected to change
		again.
*/

#define STEP 0.0025

void floatbench (char ***argvp)
{
	double	x, y;
	double	r, i, rr, ii;
	int	k;
	double	tr, tc;
	int	n = 0;

        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("Mandelbrot Bench:\n");
        else printf ("mandelbrot	| ");

	(*argvp)++;

	resettimer ();
	for (y = -2.0; y < 2.0; y += STEP) {
		for (x = -2.0; x < 2.0; x += STEP) {
			n ++;
			r = i = 0.0; k = 0;
			do {
				rr = r * r;
				ii = i * i;
				i = 2 * r * i;
				r = rr - ii;
				k ++;
			} while (rr + ii < 4.0 && k < 32);
		}
		fprintf (stderr, "%f\r", y);
	}

	gettimer (&tr, &tc);

	if (verbose) {
		printf( "\n%d points of Mandelbrot set computed in %g seconds\n", n, tr);
		printf ("CPU time = %g seconds\n", tc);
	} else {
		printf( "%d	| %g	| %g", n, tr, tc);
	}
	printf ("\n");
}
