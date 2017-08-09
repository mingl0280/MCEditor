#include <cstdio>
#include "globals.h"
#include "MCEditor.h"
#include "MCACoder.h"
using namespace std;

MCEditor editor;
MCACoder coder;
int A[512][512][256] = {0}, B[512][512][256] = {0};

int main()
{
    int x, y, z, id;
    scanf("%d%d%d%d", &x, &y, &z, &id);

    int x0 = x - 5, z0 = z - 5, y0 = y;
    int x_len = 200, z_len = 200, y_len = 30;

    for (int i = 0; i < x_len; i++)
	for (int j = 0; j < z_len; j++)
	    for (int k = 0; k < y_len; k++)
	    {
		    A[i][j][k] = id;
	    }
    
    editor.setRegion(A, B, x_len, z_len, y_len, x0, z0, y0);
    
    return 0;
}	
