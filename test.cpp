#include <cstdio>
#include "globals.h"
#include "MCEditor.h"
#include "BlockEntity.h"
#include "MCACoder.h"
using namespace std;

MCEditor editor;
MCACoder coder;

int main()
{
    int x0, y0, z0;
    int xl = 10, yl = 1, zl = 10;
    
    scanf("%d%d%d", &x0, &y0, &z0);
    MCRegion region(x0, z0, y0, xl, zl, yl);
    
    for (int x = 0; x < xl; x++)
        for (int z = 0; z < zl; z++)
            for (int y = 0; y < yl; y++)
            {
                region.A[x][z][y] = BlockInfo(25, 0, 0, 0, 0);
                Pos position(x + x0, z + z0, y + y0);
                region.B[x][z][y] = new BlockEntityNote(position, 22, 0);
            }
    
    editor.setRegion(region);
    //coder.getBlock(x0, z0, y0);
    
    return 0;
}
