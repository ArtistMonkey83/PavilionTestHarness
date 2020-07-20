
OSHMEM_CC=shmemcc
OSHMEM_C_FLAGS=-O3 -g
LIBS=
CFLAGS=-std=c99

all: hotspotinc

hotspotinc: hotspotinc.c
	$(OSHMEM_CC) $(CFLAGS) $(OSHMEM_C_FLAGS) -o $@ $< $(LIBS)

clean:
	rm hotspotinc
