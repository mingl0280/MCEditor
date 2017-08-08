#include <cstdio>
#include "globals.h"
#include "MCACoder.h"
using namespace std;

MCACoder coder;

int main()
{
    int x, y, z, id;
    scanf("%d%d%d%d", &x, &y, &z, &id);
    //coder.setBlock(x, z, y, id);
    for (int i = -100; i <= 100; i++)
	for (int j = -100; j <= 100; j++)
	    for (int k = -5; k < 5; k++)
		coder.setBlock(x + i, z + j, y + k, BlockInfo());
    coder.Finalize();
    return 0;
}	
