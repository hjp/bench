CFLAGS = -std=gnu11 -Wall -O9

PROGS = \
    double_postinc \
    int_access \
    int_mod \
    int_mul \
    int_postinc \
    rand \
    strlen \

all: $(PROGS)

clean:
	rm -f *.o
	rm -f $(PROGS)

double_postinc: double_postinc.o dummy.o
	$(CC) $^ -o $@ 

int_access: int_access.o dummy.o
	$(CC) $^ -o $@ 

int_mod: int_mod.o dummy.o
	$(CC) $^ -o $@ 

int_mul: int_mul.o dummy.o
	$(CC) $^ -o $@ 

int_postinc: int_postinc.o dummy.o
	$(CC) $^ -o $@ 

rand: rand.o dummy.o
	$(CC) $^ -o $@ 

strlen: strlen.o my_strlen.o
