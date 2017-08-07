#ifndef MCACoder_h
#define MCACoder_h

#include <string>
#include <cstdlib>
#include "NBTCoder.h"

class MCACoder
{    
public:
    MCACoder(): file_name("") { memset(Chunk, 0, sizeof(Chunk)); };
    
    void setBlock(int x, int z, int y, int id);

    void setRegion(int* B, int x_max, int z_max, int y_max,
                    int x0, int z0, int y0); 
    
private:
    int DX[6] = {0, 0, -1, 1, 0, 0}, 
        DZ[6] = {0, 0, 0, 0, -1, 1},
        DY[6] = {1, -1, 0, 0, 0, 0};

    NBTCoder nbt_coder;

    std::string file_name;
    node* Chunk[K1 + 7];
    uc buffer[M64], chunk_buffer[M1];

    int location[K1], timestamp[K1];
    
    int blocks[K1][K1][256];
    int blocklight[K1][K1][256];
    int skylight[K1][K1][256];

    int readBlock(int x, int z, int y);

    void loadMCA(const std::string &file_name_);
    void writeMCA();

    node* newSubSection(int y);  
};

#endif /* MCACoder_h */	
