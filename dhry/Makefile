CC=clang
CXX=clang++

CFLAGS= -c -O0 -fPIC
LDFLAGS+= -L/usr/local/lib

DHRYSTONE=dhry_1.c dhry_2.c

DHRYSTONE_OBJECTS=$(DHRYSTONE:.c=.o)

all: dhrystone

dhrystone: $(DHRYSTONE_OBJECTS)

clean:
	rm *.o
