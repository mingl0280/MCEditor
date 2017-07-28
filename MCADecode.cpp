#include "globals.h"
#include "NBTDecode.h"
#include <cstdio>
#include <cstring>
using namespace std;

char buffer[G1];
ull location[K1], timestamp[K1];

int main()
{
    FILE* handle = fopen(".mca");

    fread(buffer, 1, K4, handle);
    for (int i = 0; i < K4; i += 4)
	location[i >> 2] = K4 * byteToInt(buffer, i, 3);
    
    fread(buffer, 1, K4, handle);
    for (int i = 0; i < K4; i += 4)
	timestamp[i >> 2] = K4 * byteToInt(buffer, i, 4);

    
}
