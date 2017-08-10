CC = g++
CFLAGS = -std=c++11 -Ofast -Wno-unused-result
LIBS = globals.cpp NBTCoder.cpp MCACoder.cpp MCEditor.cpp

default: 
	$(CC) $(LIBS) test.cpp $(CFLAGS) -lz

paint:
	$(CC) $(LIBS) paint.cpp $(CFLAGS) -lz

clean:
	rm *.out
