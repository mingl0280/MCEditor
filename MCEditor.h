#ifndef MCAEditor_h
#define MCAEditor_h

#include "MCACoder.h"

class MCEditor
{
public:
    void setRegion(int A[K1][K1][256],
		   int xl, int zl, int yl,
		   int x0, int z0, int y0);
private:
	const int DX[6] = {0, 0, -1, 1, 0, 0},
			DZ[6] = {0, 0, 0, 0, -1, 1},
        	DY[6] = {1, -1, 0, 0, 0, 0};

	int x_len, z_len, y_len,
		x_ori, z_ori, y_ori;

    int blocks[K1][K1][256],
		blocklight[K1][K1][256],
		skylight[K1][K1][256];

	MCACoder mca_coder;

	void initBlocks();

	void computeBlockLight();

	void computeSkyLight();

	void updateMCA();
};

#endif /* MCAEditor_h */
