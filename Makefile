include GNUmakevars

all: bench fileserial largedir

bench: bench.o diskbench.o screenbench.o sieve.o floatbench.o getch.o \
    filetree.o timer.o fadd.o
	$(CC) -o $@ $^

fileserial: fileserial.o timer.o
	$(CC) -o $@ $^

clean:
	rm -f bench *.o core *.i

distclean: clean
	rm -f *.d


-include *.d

include GNUmakerules
