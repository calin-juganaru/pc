build: send recv

all: build

link_emulator/lib.o:
	$(MAKE) -C link_emulator

send: send.o ./link_emulator/lib.o
	g++ -O3 -std=c++11 -g send.o ./link_emulator/lib.o -o send

recv: recv.o ./link_emulator/lib.o
	g++ -O3 -std=c++11 -g recv.o ./link_emulator/lib.o -o recv

.cpp.o: packet.hpp
	g++ -O3 -std=c++11 -Wall -g -c $?

clean:
	$(MAKE) -C link_emulator clean
	-rm -f *.o send recv
