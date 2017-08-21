#ifndef MCAEditor_h
#define MCAEditor_h

#include "globals.h"
#include "MCACoder.h"
#include "BlockEntity.h"

class MCRegion
{
public:
    BlockInfo*** A;
    BlockEntity**** B;

    int x_len, z_len, y_len,
        x_ori, z_ori, y_ori;

    MCRegion(int x0, int z0, int y0,
             int xl, int zl, int yl);
    
    ~MCRegion();

private:
    void delBlockEntity(BlockEntity* entity);
};

class MCEditor
{
public:
    void setRegion(const MCRegion &A);

private:
    MCACoder mca_coder;
    
    const int DX[6] = {0, 0, -1, 1, 0, 0},
              DZ[6] = {0, 0, 0, 0, -1, 1},
              DY[6] = {1, -1, 0, 0, 0, 0};
    
    int x_len, z_len, y_len,
        x_ori, z_ori, y_ori;
    
    ui ***blocks, ***blockdata,
          ***blocklight, ***skylight;
    BlockEntity**** block_entities;

    void initArrays(int l1, int l2, int l3);

    void clearArrays(int l1, int l2, int l3);

    ui*** new3DUIArray(int l1, int l2, int l3);

    void del3DUIArray(ui*** &A, int l1, int l2, int l3);

    void initBlocks(const MCRegion &A);

    void computeBlockLight();

    void computeSkyLight();
    
    void lightPropagate(ui*** light);

    void updateMCA();
};

#endif /* MCAEditor_h */
