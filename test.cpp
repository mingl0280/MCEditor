#include <cstdio>
#include "globals.h"
#include "NBTCoder.h"
using namespace std;

char buffer[G1];
NBTCoder coder;

int main()
{
    FILE* handle = fopen("bigtest.nbt", "r");
    fread(buffer, 1, G1, handle);
    coder.Print(coder.Decode(buffer));
    return 0;
}
