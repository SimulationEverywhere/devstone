CC=clang
CXX=clang++

CFLAGS= -c -O0
CXXFLAGS= -c -Wall -pedantic -O2 --std=c++14 -DNDEBUG
LDFLAGS=

#Boost Library location
CXXFLAGS+= -I/usr/local/include
LDFLAGS+= -L/usr/local/lib

#CDPP headers
CXXFLAGS+= -I$(abspath ./simulators/cdboost/include)

MAINS=cdboost-devstone.cpp

OBJECTS=$(SOURCES:.cpp=.o) $(MODELS:.cpp=.o)

all: $(MAINS) cdboost-devstone

cdboost-devstone: cdboost-devstone.o
	cd dhry; make dhrystone
	$(CXX) $(LDFLAGS) -lboost_program_options cdboost-devstone.o dhry/dhry_1.o dhry/dhry_2.o -o cdboost-devstone

clean:
	rm cdboost-devstone *.o
	cd dhry; make clean
