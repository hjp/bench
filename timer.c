#include "timer.h"

#if __MSDOS__
#define USE_CLOCK 1
#else
#define USE_TIMES 1
#endif

#if USE_CLOCK

/* MSDOS doesn't do per process accounting, so real and cpu time are
 * the same.
 */

#include <time.h>

static clock_t tr;

void resettimer (void) {
	tr = clock ();
}

void gettimer (double *real, double *cpu) {
	clock_t now = clock ();

        *cpu = *real = (double) (now - tr) / CLOCKS_PER_SEC;
}

#endif

#if USE_TIMES

/* All Posix systems have the times system call. Unfortunately, it
 * is a lot coarser than the system clock on some systems.
 * (e.g, on Ultrix, the system clock has a frequency of 256 Hz,
 * but the cpu times have a granularity of 60 Hz and the real time
 * one of 1 Hz!).
 */

#include <assert.h>
#include <sys/times.h>
#include <unistd.h>

static clock_t tr, tc;
static double period;

void resettimer (void) {
	struct tms t;

	if (period == 0.0) {
		period = 1.0 / sysconf (_SC_CLK_TCK);
		assert (period > 0.0);
	}

	tr = times (&t);
	tc = t.tms_utime + t.tms_stime;
}

void gettimer (double *real, double *cpu) {
	clock_t now;
	struct tms nowcpu;

	now = times (&nowcpu);

        *real = (now - tr) * period;
        *cpu = (nowcpu.tms_utime + nowcpu.tms_stime - tc) * period;
}

#endif
