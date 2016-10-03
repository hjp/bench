CFLAGS = -std=gnu11 -Wall -O9

all: \
    double_postinc \
    int_access \
    int_mod \
    int_mul \
    int_postinc \
    rand \
    strlen \


double_postinc: double_postinc.o dummy.o
	$(CC) $^ -lant -o $@ 

int_access: int_access.o dummy.o
	$(CC) $^ -lant -o $@ 

int_mod: int_mod.o dummy.o
	$(CC) $^ -lant -o $@ 

int_mul: int_mul.o dummy.o
	$(CC) $^ -lant -o $@ 

int_postinc: int_postinc.o dummy.o
	$(CC) $^ -lant -o $@ 

rand: rand.o dummy.o
	$(CC) $^ -lant -o $@ 

strlen: strlen.o my_strlen.o
