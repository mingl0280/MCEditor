#ifndef MCACoder_h
#define MCACoder_h

#include <string>
#include <cstdlib>
#include "NBTCoder.h"

class MCACoder
{
public:
    MCACoder(): modification_saved(true), cur_file_name("") {
        memset(Chunk, 0, sizeof(Chunk));
    }

    void saveModification();

    void setBlock(int x, int z, int y,
                  const BlockInfo &info);

    BlockInfo getBlock(int x, int z, int y);

private:
    NBTCoder nbt_coder;
    bool modification_saved;

    std::string cur_file_name;
    node* Chunk[K1 + 7];
    uc buffer[M64], chunk_buffer[M1 << 1];

    int location[K1], timestamp[K1];

    int loadMCA(const std::string &file_name);

    void writeMCA();

    node* sectionWithY(node* T, int y);

    node* newSectionWithY(int y);

    void setHalfByte(node* T, int v);
};

#endif /* MCACoder_h */
