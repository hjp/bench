/*
	bench.c

	contains a number of benchmark routines

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bench.h"
#include "diskbench.h"
#include "screenbench.h"
#include "floatbench.h"
#include "sieve.h"
#include "filetree.h"

int verbose = 0;
char *id = NULL;

void setverbose(char ***argvp) {
	verbose = 1;
	(*argvp)++;
}

void setid(char ***argvp) {
	(*argvp)++;
	id = **argvp;
	(*argvp)++;
}


int main (int argc, char **argv)
{
	static struct {
		char *name;
		void (*func)(char ***);
	} benchtab[] = {
		{ "disk", diskbench },
		{ "screen", screenbench },
		{ "sieve", sieve },
		{ "mandelbrot", floatbench },
		{ "tree", filetree },

		/* The following are not benchmarks but global parameters */
		{ "verbose", setverbose },
		{ "id", setid },
	};
	const int n =sizeof(benchtab)/sizeof(benchtab[0]);


	setvbuf(stdout, NULL, _IONBF, 0);

	argv++;
	while (*argv) {
		int i;

		for (i = 0; i < n; i++) {
			if (strcmp(*argv, benchtab[i].name) == 0) {
				benchtab[i].func(&argv);
				break;
			}
		}
		if (i == n) {
			fprintf(stderr, "ignoring unknown benchmark %s\n", *argv);
			argv++;
		}
	}
	return 0;
}
