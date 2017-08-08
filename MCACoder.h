#ifndef MCACoder_h
#define MCACoder_h

#include <string>
#include <cstdlib>
#include "NBTCoder.h"

class MCACoder
{    
public:
    MCACoder(): cur_file_name("") { 
        memset(Chunk, 0, sizeof(Chunk)); 
    }
    
    void Finalize();

    void setBlock(int x, int z, int y, 
                    const BlockInfo &info);

    BlockInfo getBlock(int x, int z, int y);
    
private:
    NBTCoder nbt_coder;

    std::string cur_file_name;
    node* Chunk[K1 + 7];
    uc buffer[M64], chunk_buffer[M1];

    int location[K1], timestamp[K1];

    void loadMCA(const std::string &file_name_);

    void writeMCA();

    node* newSubSection(int y);
};

#endif /* MCACoder_h */	
