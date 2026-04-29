CC = gcc
CFLAGS = -Wall -pthread

all: orion eternal

orion:
	$(CC) $(CFLAGS) orion.c -o orion

eternal:
	$(CC) $(CFLAGS) eternal.c -o eternal

clean:
	rm -f orion eternal

clean_ipc:
	ipcs -m | awk 'NR>3 {print $$2}' | xargs -r ipcrm -m
