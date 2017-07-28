CC = g++
CFLAGS = -std=c++11 -Ofast -Wno-unused-result
LIBS = globals.cpp NBTCoder.cpp

default:
	$(CC) $(LIBS) test.cpp $(CFLAGS) -o test

NBTDecode: NBTDecode.cpp
	$(CC) NBTDecode.cpp $(CFLAGS) -c

clean:
	-rm *~ \#*\#
