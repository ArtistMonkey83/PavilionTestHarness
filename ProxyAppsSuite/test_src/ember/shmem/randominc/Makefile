
OSHMEM_CC=shmemcc
OSHMEM_C_FLAGS=-O3 -g
LIBS=
CFLAG=-std=c99
all: randominc

randominc: randominc.c
	$(OSHMEM_CC) $(OSHMEM_C_FLAGS) $(CFLAG) -o $@ $< $(LIBS)

clean:
	rm randominc
