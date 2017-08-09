#ifndef MCAEditor_h
#define MCAEditor_h

#include "globals.h"
#include "MCACoder.h"

class MCEditor
{
public:
    void setRegion(int A[512][512][256],
		   int B[512][512][256],
		   int xl, int zl, int yl,
		   int x0, int z0, int y0);
private:
    MCACoder mca_coder;
    
    const int DX[6] = {0, 0, -1, 1, 0, 0},
	DZ[6] = {0, 0, 0, 0, -1, 1},
	DY[6] = {1, -1, 0, 0, 0, 0};
    
    int x_len, z_len, y_len,
	x_ori, z_ori, y_ori;
    
    ui 	blocks[512][512][256],
	blockdata[512][512][256],
	blocklight[512][512][256],
	skylight[512][512][256];

    void initBlocks(int A[512][512][256], int B[512][512][256]);

    void computeBlockLight();

    void computeSkyLight();

    void updateMCA();

    void lightPropagate(ui light[512][512][256]);
};

#endif /* MCAEditor_h */
