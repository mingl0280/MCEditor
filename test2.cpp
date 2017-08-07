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
    for (int i = -10; i <= 10; i++)
	for (int j = -10; j <= 10; j++)
	{
	    coder.setBlock(x + i, z + j, y - 1, id);
	    for (int k = 0; k < 10; k++)
		coder.setBlock(x + i, z + j, y + k, 0);
	}
    return 0;
}	
