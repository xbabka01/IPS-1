
PRJ=pgrep
CFLAGS=-Wall -std=c++11 -pedantic -lm

all: pgrep.cpp
	g++ -Wall -Wextra -std=c++11 -pedantic pgrep.cpp -o pgrep -pthread


clean:
	rm -f *.o *.out $(PROGS)
#
