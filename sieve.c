#include <stdio.h>
#include <limits.h>

#include "timer.h"
#include "bench.h"
#include "sieve.h"


/*
	define some constants
*/

#if INT_MAX <= 32767
#define	NMAX 		32000
#else
#define	NMAX 		1000000
#endif
#if DEBUG
#define SIEVE_REP	1
#else
#define SIEVE_REP	20
#endif



/*
	compute the primes < 64003 (or <= 2000003) 20 times.

	typical times: see sieve.notes
*/

void	sieve (char ***argvp)
{
	static char  	prim [NMAX];
	unsigned   	n, n1, i, c = 0, l;
	double		tr, tc;

	(*argvp)++;

        if (id) {
		if (verbose) printf ("System: %s\n", id);
		else printf ("%s	| ", id);
	}
        if (verbose) printf ("Sieve of Erathosthenes Bench:\n");
        else printf ("sieve	| ");

	resettimer ();

	for( l = SIEVE_REP; l--;){

		c = 0;
	    	for( i = 0; i < NMAX; prim[ i++] = 1);
		for( n = 0; n < NMAX; n++){
			if( prim[n] )
			{
				c ++;
				n1 = 2 * n + 3;
				#if DEBUG
				printf("%8u", n1 );
				#endif
				for(i = n + n1; i < NMAX; i += n1)
				{
					prim[i] = 0;
				}
			}
		}
	}
	gettimer (&tr, &tc);

	if (verbose) {
		printf ("\n%d * %d primes found in %g seconds\n",
			SIEVE_REP, c, tr);
		printf ("CPU time: %g seconds\n", tc);
	} else {
		printf ("%d	| %d	| %g	| %g",
			SIEVE_REP, c, tr, tc);
	}
	printf("\n");
}
