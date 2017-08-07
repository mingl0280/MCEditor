CC = g++
CFLAGS = -std=c++11 -Ofast -Wno-unused-result
LIBS = globals.cpp NBTCoder.cpp MCACoder.cpp

default:
	$(CC) $(LIBS) test.cpp $(CFLAGS) -lz -o test

test2: MCACoder.cpp test2.cpp
	$(CC) $(LIBS) test2.cpp $(CFLAGS) -lz -o test2	

NBTDecode: NBTDecode.cpp
	$(CC) NBTDecode.cpp $(CFLAGS) -c

clean:
	-rm *~ \#*\#
	-rm chunk*
