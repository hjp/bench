#!/usr/bin/perl -w
use strict;

my $t0 = time;

$| = 1;
for(my $n = 4;; $n++) {
    my $ts = 0;
    for my $j (1 .. $n - 1) {
	if ($n % $j == 0) {
	    $ts += $j;
	}
    }
    if ($n % 100 == 0) {
        print "$n\r";
	my $t1 = time;
	if ($t1 - $t0 >= 60) {
	    printf "%d %d %.0f nps\n", $n, $t1 - $t0, $n/($t1 - $t0);
	    exit(0);
	}
    }
    print "$n $ts\n" if ($n == $ts);
}
