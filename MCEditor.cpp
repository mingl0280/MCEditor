#include "MCEditor.h"
#include <vector>
#include <algorithm>
#include <queue>
#include <utility>
#include <set>
#include <cstring>
#include "globals.h"
using namespace std;

inline bool in_region(int x, int z, int y,
                      int x1, int z1, int y1,
                      int x2, int z2, int y2)
{
    return x1 <= x && x < x2 && z1 <= z && z < z2 && y1 <= y && y < y2;
}

/////////////////////////////////Region Member Functions////////////////////////

MCRegion::MCRegion(int x0, int z0, int y0,
                   int xl, int zl, int yl)
{
    A = new BlockInfo** [xl];
    for (int i = 0; i < xl; i++)
    {
        A[i] = new BlockInfo* [zl];
        for (int j = 0; j < zl; j++)
            A[i][j] = new BlockInfo [yl];
    }
        
    B = new BlockEntity*** [xl];
    for (int i = 0; i < xl; i++)
    {
        B[i] = new BlockEntity** [zl];
        for (int j = 0; j < zl; j++)
        {
            B[i][j] = new BlockEntity* [yl];
            memset(B[i][j], 0, yl * sizeof(BlockEntity*));
        }
    }

    x_len = xl, z_len = zl, y_len = yl;
    x_ori = x0, z_ori = z0, y_ori = y0;
}

MCRegion::~MCRegion()
{
    for (int i = 0; i < x_len; i++)
    {
        for (int j = 0; j < z_len; j++)
            delete [] A[i][j];
        delete [] A[i];
    }
    delete A;

    for (int i = 0; i < x_len; i++)
    {
        for (int j = 0; j < z_len; j++)
        {
            for (int k = 0; k < y_len; k++)
                delBlockEntity(B[i][j][k]);
            delete [] B[i][j];
        }
        delete [] B[i];
    }
    delete B;
}

void MCRegion::delBlockEntity(BlockEntity* entity)
{
    if (!entity) return;

    string id = entity->entity_id;
    if (id == "noteblock")
        delete ((BlockEntityNote*)entity);
    //TODO
}

////////////////////////////////MCEditor Member Functions///////////////////////

void MCEditor::setRegion(const MCRegion &R)
{
    x_len = R.x_len, z_len = R.z_len, y_len = R.y_len;
    x_ori = R.x_ori, z_ori = R.z_ori, y_ori = R.y_ori;
    block_entities = R.B;

    initArrays(x_len + 60, z_len + 60, 256);

    initBlocks(R);

    computeBlockLight();

    computeSkyLight();

    updateMCA();

    clearArrays(x_len + 60, z_len + 60, 256);
}

void MCEditor::initArrays(int l1, int l2, int l3)
{
    blocks = new3DUIArray(l1, l2, l3);
    blockdata = new3DUIArray(l1, l2, l3);
    blocklight = new3DUIArray(l1, l2, l3);
    skylight = new3DUIArray(l1, l2, l3);
}

void MCEditor::clearArrays(int l1, int l2, int l3)
{
    del3DUIArray(blocks, l1, l2, l3);
    del3DUIArray(blockdata, l1, l2, l3);
    del3DUIArray(blocklight, l1, l2, l3);
    del3DUIArray(skylight, l1, l2, l3);
}

ui*** MCEditor::new3DUIArray(int l1, int l2, int l3)
{
    ui*** A;
    A = new ui** [l1];
    for (int i = 0; i < l1; i++)
    {
        A[i] = new ui* [l2];
        for (int j = 0; j < l2; j++)
            A[i][j] = new ui [l3];
    }
    return A;
}

void MCEditor::del3DUIArray(ui*** &A, int l1, int l2, int l3)
{
    for (int i = 0; i < l1; i++)
    {
        for (int j = 0; j < l2; j++)
            delete [] A[i][j];
        delete [] A[i];
    }
    delete A; A = 0;
}

void MCEditor::initBlocks(const MCRegion &R)
{
    int d = 30;
    for (int i = 0; i < x_len; i++)
        for (int j = 0; j < z_len; j++)
            for (int k = 0; k < y_len; k++)
            {
                ui id = R.A[i][j][k].id | (R.A[i][j][k].add << 4);
                blocks[d + i][d + j][y_ori + k] = id;
                blockdata[d + i][d + j][y_ori + k] = R.A[i][j][k].data;
            }

    int x0 = x_ori - 30, x1 = x_ori + x_len + 30,
        z0 = z_ori - 30, z1 = z_ori + z_len + 30,
        y0 = 0, y1 = 256;
    //coordinates corresponding to the "lower" and "upper" corner of
    //the whole region stored in array;

    vector<Pos> V;
    for (int x = x0; x < x1; x++)
        for (int z = z0; z < z1; z++)
            for (int y = y0; y < y1; y++)
            {
                if (in_region(x, z, y,
                              x_ori, z_ori, y_ori,
                              x_ori + x_len, z_ori + z_len, y_ori + y_len))
                    continue;
                //leave edited blocks unchanged;
                V.push_back(Pos(x, z, y));
            }
    sort(V.begin(), V.end());

    for (Pos u : V)
    {
        int i = u.x - x0, j = u.z - z0, k = u.y - y0;
        BlockInfo res = mca_coder.getBlock(u.x, u.z, u.y);

        blocks[i][j][k] = res.id + (res.add << 8);
        blockdata[i][j][k] = res.data;
        blocklight[i][j][k] = res.block_light;
        skylight[i][j][k] = res.sky_light;
    }
}

void MCEditor::computeBlockLight()
{
    fprintf(stderr, "Computing Block Light...\n");
    for (int x = 15; x < x_len + 45; x++)
        for (int z = 15; z < z_len + 45; z++)
            for (int y = 0; y < 256; y++)
                blocklight[x][z][y] = get_block_light(blocks[x][z][y]);

    lightPropagate(blocklight);
}

void MCEditor::computeSkyLight()
{
    fprintf(stderr, "Computing Sky Light...\n");
    for (int x = 15; x < x_len + 45; x++)
        for (int z = 15; z < z_len + 45; z++)
            for (int y = 0; y < 256; y++)
                skylight[x][z][y] = 0;

    for (int x = 15; x < x_len + 45; x++)
        for (int z = 15; z < z_len + 45; z++)
            for (int y = 255; y >= 0; y--)
                if (get_opacity(blocks[x][z][y]) <= 1)
                    skylight[x][z][y] = 15;
                else 
                    break;

    lightPropagate(skylight);
}

void MCEditor::updateMCA()
{
    //remove block entities in the editing region;
    vector<Pos> VP;
    for (int x = 0; x < x_len; x++)
        for (int z = 0; z < z_len; z++)
            for (int y = 0; y < y_len; y++)
                VP.push_back(Pos(x + x_ori, z + z_ori, y + y_ori));

    sort(VP.begin(), VP.end());

    for (auto position : VP)
        mca_coder.removeBlockEntity(position);

    //update blocks
    vector<Block> VB;
    int x0 = x_ori - 30, z0 = z_ori - 30;
    for (int x = 15; x < x_len + 45; x++)
        for (int z = 15; z < z_len + 45; z++)
            for (int y = 0; y < 256; y++)
            {
                Pos position = Pos(x + x0, z + z0, y);
                ui id = blocks[x][z][y] & 0xFF;
                ui add = (blocks[x][z][y] >> 8) & 0xF;
                ui data = blockdata[x][z][y];
                BlockInfo info = BlockInfo(id, add, data,
                                           blocklight[x][z][y], skylight[x][z][y]);
                VB.push_back(Block(position, info));
            }
        
    sort(VB.begin(), VB.end());
        
    for (Block u : VB)
        mca_coder.setBlock(u.position, u.info);

    //insert associated block entities
    vector<pair<Pos, BlockEntity*> > VE;
    for (int x = 0; x < x_len; x++)
        for (int z = 0; z < z_len; z++)
            for (int y = 0; y < y_len; y++)
                VE.push_back(make_pair(Pos(x + x_ori, z + z_ori, y + y_ori),
                                       block_entities[x][z][y]));
    sort(VE.begin(), VE.end());

    for (auto u : VE)
        mca_coder.insertBlockEntity(u.first, u.second);

    //make sure that all modifications are saved
    mca_coder.saveModification();
}

void MCEditor::lightPropagate(ui*** light)
{
    fprintf(stderr, "Propagating Light...\n");
    queue<Pos> Q;

    for (int x = 0; x < x_len + 60; x++)
        for (int z = 0; z < z_len + 60; z++)
            for (int y = 0; y < 256; y++)
                if (light[x][z][y])
                {
                    Q.push(Pos(x, z, y));
                    while (!Q.empty())
                    {
                        Pos u = Q.front();
                        Q.pop();
                        for (int d = 0; d < 6; d++)
                        {
                            int vx = u.x + DX[d],
                                vz = u.z + DZ[d],
                                vy = u.y + DY[d];
                            if (!in_region(vx, vz, vy, 0, 0, 0,
                                           x_len + 60, z_len + 60, 256))
                                continue;

                            int dec = get_opacity(blocks[vx][vz][vy]);
                            int vlight = (int)light[u.x][u.z][u.y] - dec;
                            if (vlight <= 0)
                                continue;

                            if (light[vx][vz][vy] < vlight)
                            {
                                light[vx][vz][vy] = vlight;
                                Q.push(Pos(vx, vz, vy));
                            }
                        }
                    }
                }
    fprintf(stderr, "Finished Propagating Light.\n");
}
