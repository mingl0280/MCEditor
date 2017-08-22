#include "MCACoder.h"
#include <cstring>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include "globals.h"
using namespace std;

string MCAFileNameXZ(int x, int z)
{
    char tmp[256];
    int len = sprintf(tmp, "./region/r.%d.%d.mca", x, z);
    if (len < 0)
    {
        fprintf(stdout, "Buffer overflow when generating mca file name.\n");
        exit(0);
    }
    string res = tmp;
    return res;
}

void MCACoder::saveModification()
{
    if (!modification_saved)
        writeMCA();
    for (int i = 0; i < K1; i++)
    {
        nbt_coder.Clear(Chunk[i]);
        Chunk[i] = 0;
    }
    cur_file_name = "";
    modification_saved = true;
}

void MCACoder::setBlock(const Pos &position, const BlockInfo &info)
{
    //fprintf(stderr, "Setting (%d, %d, %d).\n", x, y, z);
    int x = position.x, z = position.z, y = position.y;

    int chunk_x = x >> 4, chunk_z = z >> 4;
    int region_x = chunk_x >> 5, region_z = chunk_z >> 5;
    string file_name = MCAFileNameXZ(region_x, region_z);

    if (loadMCA(file_name) == -1)
    {
        fprintf(stderr, "File %s that contains block (%d, %d, %d) does not exists.\n",
                file_name.c_str(), x, y, z);
        return;
    }

    int idx = (chunk_x & 31) + 32 * (chunk_z & 31);
    node *chunk_root = Chunk[idx];

    if (!chunk_root)
    {
        fprintf(stderr,
                "Chunk that contains (%d, %d, %d) not initialized\n",
                x, y, z);
        return;
    }

    modification_saved = false;

    int sec_no = y >> 4;
    node *level_root = chunk_root->childWithName("Level");
    node *sec_root = level_root->childWithName("Sections");
    node *T = sectionNodeWithY(sec_root, sec_no);
    if (!T)
    {
        /*fprintf(stderr,
            "Section at Y = %d not initialized.\n", sec_no);
            return;*/
        T = newSectionNodeWithY(sec_no);
        sec_root->addChild(T);
    }

    node *u;
    int block_x = (x & 15), block_z = (z & 15), block_y = (y & 15);
    int block_pos = block_y * 16 * 16 + block_z * 16 + block_x;

    //set Blocks
    u = T->childWithName("Blocks");
    nbt_coder.setByteInArrayContent(u, block_pos, info.id);

    //set Add
    u = T->childWithName("Add");
    if (u || info.add)
    {
        if (!u)
        {
            u = new node(TAG_BYTE_ARRAY, "Add");
            u->tag.va.resize(K2);
            T->addChild(u);
        }
        nbt_coder.setHalfByteInArrayContent(u, block_pos, info.add);
    }

    //set BlockData
    u = T->childWithName("Data");
    nbt_coder.setHalfByteInArrayContent(u, block_pos, info.data);

    //set BlockLight
    u = T->childWithName("BlockLight");
    nbt_coder.setHalfByteInArrayContent(u, block_pos, info.block_light);

    //set SkyLight
    u = T->childWithName("SkyLight");
    nbt_coder.setHalfByteInArrayContent(u, block_pos, info.sky_light);
}

BlockInfo MCACoder::getBlock(int x, int z, int y)
{
    //fprintf(stderr, "Reading (%d, %d, %d).\n", x, z, y);

    int chunk_x = x >> 4, chunk_z = z >> 4;
    int region_x = chunk_x >> 5, region_z = chunk_z >> 5;
    string file_name = MCAFileNameXZ(region_x, region_z);

    if (loadMCA(file_name) == -1)
    {
        fprintf(stderr, "File %s that contains block (%d, %d, %d) does not exists.\n",
                file_name.c_str(), x, y, z);
        return BlockInfo();
    }

    int idx = (chunk_x & 31) + 32 * (chunk_z & 31);
    node *T = Chunk[idx];

    if (!T)
    {
        fprintf(stderr,
                "Chunk that contains (%d, %d, %d) not initialized\n",
                x, z, y);
        return BlockInfo();
    }

    int sec_no = y >> 4;

    T = T->childWithName("Level");
    T = T->childWithName("Sections");
    T = sectionNodeWithY(T, sec_no);
    if (!T)
    {
        /*fprintf(stderr,
            "Section at Y = %d not initialized.\n", sec_no);*/
        return BlockInfo();
    }

    node *u;
    int block_x = (x & 15), block_z = (z & 15), block_y = (y & 15);
    int block_pos = block_y * 16 * 16 + block_z * 16 + block_x;

    u = T->childWithName("Blocks");
    int id = nbt_coder.getByteInArrayContent(u, block_pos);

    u = T->childWithName("Add");
    int add = u ? nbt_coder.getHalfByteInArrayContent(u, block_pos) : 0;

    u = T->childWithName("Data");
    int data = u ? nbt_coder.getHalfByteInArrayContent(u, block_pos) : 0;

    u = T->childWithName("BlockLight");
    int block_light = nbt_coder.getHalfByteInArrayContent(u, block_pos);

    u = T->childWithName("SkyLight");
    int sky_light = nbt_coder.getHalfByteInArrayContent(u, block_pos);

    return BlockInfo(id, add, data, block_light, sky_light);
}

void MCACoder::removeBlockEntity(const Pos &position)
{
    int x = position.x, z = position.z, y = position.y;

    int chunk_x = x >> 4, chunk_z = z >> 4;
    int region_x = chunk_x >> 5, region_z = chunk_z >> 5;
    string file_name = MCAFileNameXZ(region_x, region_z);

    if (loadMCA(file_name) == -1)
        return;

    int idx = (chunk_x & 31) + 32 * (chunk_z & 31);
    node *chunk_root = Chunk[idx];
    if (!chunk_root)
        return;

    node *level_root = chunk_root->childWithName("Level");
    node *T = level_root->childWithName("TileEntities");

    for (auto it = T->ch.begin(); it != T->ch.end(); it++)
    {
        node *u = *it;
        if (u->ch.empty())
            continue;

        node *v;
        v = u->childWithName("x");
        int x = v->tag.vi;
        v = u->childWithName("z");
        int z = v->tag.vi;
        v = u->childWithName("y");
        int y = v->tag.vi;
        if (!(position == Pos(x, z, y)))
            continue;

        delete u;
        it = T->ch.erase(it);
        modification_saved = false;
        break;
    }
}

void MCACoder::insertBlockEntity(const Pos &position, BlockEntity *entity)
{
    if (!entity)
        return;

    int x = position.x, z = position.z, y = position.y;

    node *chunk_root = chunkWithXZ(x, z);
    if (!chunk_root)
    {
        fprintf(stderr,
                "Chunk that contains (%d, ?, %d) is not initialized\n",
                x, z);
        return;
    }

    node *level_root = chunk_root->childWithName("Level");
    node *T = level_root->childWithName("TileEntities");
    if (!T)
    {
        fprintf(stderr,
                "TileEntities does not exits (%d, %d, %d)\n",
                x, y, z);
        return;
    }

    T->tag.ch_type = TAG_COMPOUND;
    T->addChild(newBlockEntityNode(entity));
    modification_saved = false;
}

void MCACoder::setHeightMap(int x, int z, int y)
{
    node *chunk_root = chunkWithXZ(x, z);
    if (!chunk_root)
    {
        fprintf(stderr,
                "Chunk that contains (%d, ?, %d) is unintialized",
                x, z);
        return;
    }

    node *level_root = chunk_root->childWithName("Level");
    node *T = level_root->childWithName("HeightMap");

    int block_x = (x & 15), block_z = (z & 15);
    int col_pos = block_z * 16 + block_x;
    int h = min((int)nbt_coder.getByteInArrayContent(T, col_pos), y);
    nbt_coder.setByteInArrayContent(T, col_pos, 0);
}

//////////////////////////////////PRIVATE/////////////////////////////////////

int MCACoder::loadMCA(const string &file_name)
{
    if (cur_file_name == file_name)
        return 0;

    fprintf(stderr, "Loading %s...\n", file_name.c_str());

    if (cur_file_name != "")
        saveModification();

    FILE *handle = fopen(file_name.c_str(), "r");
    if (!handle)
        return -1;

    cur_file_name = file_name;

    fread(buffer, 1, K4, handle);
    for (int i = 0; i < K4; i += 4)
        location[i >> 2] = K4 * byteToInt(buffer, i, 3);

    fread(buffer, 1, K4, handle);
    for (int i = 0; i < K4; i += 4)
        timestamp[i >> 2] = K4 * byteToInt(buffer, i, 4);

    for (int i = 0; i < K1; i++)
        if (location[i])
        {
            fseek(handle, location[i], SEEK_SET);

            fread(buffer, 1, 4, handle);
            int chunk_len = (int)byteToInt(buffer, 0, 4);
            chunk_len--;

            fread(buffer, 1, 1, handle);
            int cmpr_type = byteToInt(buffer, 0, 1);

            if (cmpr_type != 2)
            {
                fprintf(stderr, "Decompression Type Error: %d.\n", cmpr_type);
                exit(0);
            }

            chunk_len = fread(buffer, 1, chunk_len, handle);

            decompress(chunk_buffer, sizeof(chunk_buffer), buffer, chunk_len);

            Chunk[i] = nbt_coder.Decode(chunk_buffer);

            //debug::
            //string name = "chunk"; name += to_string(i);
            //freopen(name.c_str(), "w", stdout);
            //nbt_coder.Print(Chunk[i]);
        }
        else
            Chunk[i] = 0;

    fclose(handle);

    return 0;
}

void MCACoder::writeMCA()
{
    memset(buffer, 0, sizeof(buffer));

    ull offset = K4 << 1;
    for (int i = 0; i < K1; i++)
    {
        if (!Chunk[i])
        {
            intToByte(0, buffer, i << 2, 3);
            intToByte(0, buffer, (i << 2) + 3, 1);
            continue;
        }

        intToByte(offset / K4, buffer, i << 2, 3);

        ull chunk_len = nbt_coder.Encode(Chunk[i], chunk_buffer);
        ull cmpr_len = compress(buffer + offset + 5, sizeof(buffer) - (offset + 5),
                                chunk_buffer, chunk_len);

        cmpr_len++;
        intToByte(cmpr_len, buffer, offset, 4);
        cmpr_len += 4;
        intToByte(2, buffer, offset + 4, 1);
        //offset will be updated later

        ull sector_cnt = 0;
        if (cmpr_len % K4 == 0)
            sector_cnt = cmpr_len / K4;
        else
            sector_cnt = cmpr_len / K4 + 1;

        intToByte(sector_cnt, buffer, (i << 2) + 3, 1);
        cmpr_len = sector_cnt * K4; //Pad to a multiple of K4
        offset += cmpr_len;
    }

    for (int i = 0; i < K1; i++)
        intToByte(timestamp[i], buffer, K4 + (i << 2), 4);

    remove(cur_file_name.c_str());
    FILE *handle = fopen(cur_file_name.c_str(), "w");
    fwrite(buffer, 1, offset, handle);
    fclose(handle);
}

node *MCACoder::chunkWithXZ(int x, int z)
{
    int chunk_x = x >> 4, chunk_z = z >> 4;
    int region_x = chunk_x >> 5, region_z = chunk_z >> 5;
    string file_name = MCAFileNameXZ(region_x, region_z);

    if (loadMCA(file_name) == -1)
    {
        fprintf(stderr, "File %s that contains block (%d, ?, %d) does not exists.\n",
                file_name.c_str(), x, z);
        return 0;
    }

    int idx = (chunk_x & 31) + 32 * (chunk_z & 31);
    return Chunk[idx];
}

node *MCACoder::sectionNodeWithY(node *T, int y)
{
    for (node *u : T->ch)
    {
        node *v = u->childWithName("Y");
        if (v->tag.vi == y)
            return u;
    }
    return 0; //newSectionWithY(y);
}

node *MCACoder::newSectionNodeWithY(int y)
{
    node *u;
    node *T = new node(TAG_COMPOUND, "none");

    u = new node(TAG_BYTE, "Y");
    u->tag.vi = y;
    T->addChild(u);

    u = new node(TAG_BYTE_ARRAY, "Blocks");
    u->tag.va.resize(K4, 0);
    T->addChild(u);

    /*  u = new node(TAG_BYTE_ARRAY, "Add");
    u->tag.va.resize(K2, 0);
    T->addChild(u);
    */

    u = new node(TAG_BYTE_ARRAY, "Data");
    u->tag.va.resize(K2, 0);
    T->addChild(u);

    u = new node(TAG_BYTE_ARRAY, "BlockLight");
    u->tag.va.resize(K2, 0);
    T->addChild(u);

    u = new node(TAG_BYTE_ARRAY, "SkyLight");
    u->tag.va.resize(K2, 0xFF);
    T->addChild(u);

    return T;
}

node *MCACoder::newBlockEntityNode(BlockEntity *entity)
{
    node *u;

    //Tags common to all block entities
    node *T = new node(TAG_COMPOUND, "none");

    u = new node(TAG_STRING, "id");
    u->tag.vs = entity->entity_id;
    T->addChild(u);

    u = new node(TAG_INT, "x");
    u->tag.vi = entity->position.x;
    T->addChild(u);

    u = new node(TAG_INT, "y");
    u->tag.vi = entity->position.y;
    T->addChild(u);

    u = new node(TAG_INT, "z");
    u->tag.vi = entity->position.z;
    T->addChild(u);

    //Tags special to each block entity
    if (entity->entity_id == "minecraft:noteblock")
    {
        BlockEntityNote *noteblock = (BlockEntityNote *)entity;

        u = new node(TAG_BYTE, "note");
        u->tag.vi = noteblock->note;
        T->addChild(u);

        u = new node(TAG_BYTE, "powered");
        u->tag.vi = noteblock->powered;
        T->addChild(u);
    }

    //TODO

    return T;
}