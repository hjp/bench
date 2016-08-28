CC = gcc -std=gnu11
CFLAGS = -Wall -g -O9

ALL_BIN = schellong_goto schellong_misra reuther weikusat_a2n weikusat_a2n_unrolled

all: $(ALL_BIN)

schellong_goto: schellong_goto.o main.o
	$(CC) -o $@ $^

schellong_misra: schellong_misra.o main.o
	$(CC) -o $@ $^

reuther: reuther.o main.o
	$(CC) -o $@ $^

weikusat_a2n: weikusat_a2n.o main.o
	$(CC) -o $@ $^

weikusat_a2n_unrolled: weikusat_a2n_unrolled.o main.o
	$(CC) -o $@ $^

clean:
	rm *.o $(ALL_BIN)
