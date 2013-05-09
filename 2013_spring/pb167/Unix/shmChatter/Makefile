FLAGS = -std=c99 -lpthread -lrt

all: clean shm
shm:
	gcc ${FLAGS} shm1.c common.c -o shm.out

clean:
	rm -f *.out
