#CC = gcc -Wall -fsoft -MD
CC = gcc -Wall -MD
#CC = acc
#CC = cc
#CC = c89
CFLAGS = -O6 -D_HPUX_SOURCE
bench: bench.o diskbench.o screenbench.o sieve.o floatbench.o getch.o filetree.o timer.o
	$(CC) -o bench bench.o diskbench.o screenbench.o sieve.o floatbench.o getch.o filetree.o timer.o

clean:
	rm -f bench *.o core

-include *.d
