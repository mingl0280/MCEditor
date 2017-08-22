#ifndef MCACoder_h
#define MCACoder_h

#include <string>
#include <cstdlib>
#include "NBTCoder.h"
#include "BlockEntity.h"

class MCACoder
{
public:
    MCACoder()
    : modification_saved(true), cur_file_name("") 
    {
        memset(Chunk, 0, sizeof(Chunk));
    }

    void saveModification();

    void setBlock(const Pos &position, const BlockInfo &info);

    BlockInfo getBlock(int x, int z, int y);

    void removeBlockEntity(const Pos &position);

    void insertBlockEntity(const Pos &position, BlockEntity* entity);

    void setHeightMap(int x, int z, int y);

private:
    NBTCoder nbt_coder;
    bool modification_saved;

    std::string cur_file_name;
    node* Chunk[K1];
    uc buffer[M64], chunk_buffer[M1 << 1];

    int location[K1], timestamp[K1];

    int loadMCA(const std::string &file_name);

    void writeMCA();

    node* chunkWithXZ(int x, int z);

    node* sectionNodeWithY(node* T, int y);

    node* newSectionNodeWithY(int y);

    node* newBlockEntityNode(BlockEntity* entity);
};

#endif /* MCACoder_h */
