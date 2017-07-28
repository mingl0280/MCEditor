CC = g++
CFLAGS = -std=c++11 -Ofast -Wno-unused-result
LIBS = globals.cpp NBTCoder.cpp

default:
	$(CC) $(LIBS) test.cpp $(CFLAGS) -lz -o test

NBTDecode: NBTDecode.cpp
	$(CC) NBTDecode.cpp $(CFLAGS) -c

clean:
	-rm *~ \#*\#
	-rm chunk*
