#include "MCACoder.h"
#include <cstring>
#include <cstdlib>
#include <queue>
#include "globals.h"
using namespace std;

string MCAFileNameXZ(int x, int z)
{
    char tmp[256];
    int len = sprintf(tmp, "r.%d.%d.mca", x, z);
    if (len < 0)
    {
	fprintf(stdout, "Buffer overflow when generating mca file name.\n");
	exit(0);
    }
    string res = tmp;
    return res;
}

node* childWithName(node* T, const string &name)
{
    for (auto v : T->ch)
	if (v->tag.name == name)
	    return v;
    return 0;
}

node* childWithName(node* T, const char* name)
{
    for (auto v : T->ch)
	if (v->tag.name == name)
	    return v;
    return 0;
}

node* sectionWithY(node* T, int y)
{
    for (node* u : T->ch)
    {
	node* v = childWithName(u, "Y");
	if (v->tag.vi == y) return u;
    }
    return 0;
}

void MCACoder::Finalize()
{
    writeMCA();
}

void MCACoder::setBlock(int x, int z, int y, const BlockInfo &info)
{
    fprintf(stderr, "Setting (%d, %d, %d).\n", x, y, z);

    int chunk_x = x >> 4, chunk_z = z >> 4;
    int region_x = chunk_x >> 5, region_z = chunk_z >> 5;
    string file_name = MCAFileNameXZ(region_x, region_z);
    
    loadMCA(file_name);
    
    int idx = (chunk_x & 31) + 32 * (chunk_z & 31);
    node* T = Chunk[idx];
    
    if (!T)
    {
	fprintf(stderr,
		"Chunk that contains (%d, %d, %d) not initialized\n",
		x, z, y);
	return;
    }
    
    int subsec_no = y >> 4;
    int block_x = (x & 15), block_z = (z & 15), block_y = (y & 15);
    int block_pos = block_y * 16 * 16 + block_z * 16 + block_x;

    T = childWithName(T, "Level");
    T = childWithName(T, "Sections");
    T = sectionWithY(T, subsec_no);
    if (!T)
    {
	fprintf(stderr,
		"Subsection at Y = %d not initialized.\n", subsec_no);
	return;
    }
    T = childWithName(T, "Blocks");
    
    T->tag.va[block_pos] = info.id;
}

BlockInfo MCACoder::getBlock(int x, int z, int y)
{
    int chunk_x = x >> 4, chunk_z = z >> 4;
    int region_x = chunk_x >> 5, region_z = chunk_z >> 5;
    string file_name = MCAFileNameXZ(region_x, region_z);
    
    loadMCA(file_name);
    
    int idx = (chunk_x & 31) + 32 * (chunk_z & 31);
    node* T = Chunk[idx];
    
    if (!T)
    {
	fprintf(stderr,
		"Chunk that contains (%d, %d, %d) not initialized\n",
		x, z, y);
	return BlockInfo();
    }
    
    int subsec_no = y >> 4;
    int block_x = (x & 15), block_z = (z & 15), block_y = (y & 15);
    int block_pos = block_y * 16 * 16 + block_z * 16 + block_x;

    T = childWithName(T, "Level");
    T = childWithName(T, "Sections");
    T = sectionWithY(T, subsec_no);
    if (!T)
    {
	fprintf(stderr,
		"Subsection at Y = %d not initialized.\n", subsec_no);
	return BlockInfo();
    }

    node* u;
    u = childWithName(T, "Blocks");
    int id = u->tag.va[block_pos];

    u = childWithName(T, "BlockLight");
    int block_light = u->tag.va[block_pos >> 1];
    if (block_pos & 1)
	block_light = (block_light >> 4) & 0xF;
    else block_light = block_light & 0xF;

    u = childWithName(T, "SkyLight");
    int sky_light = u->tag.va[block_pos >> 1];
    if (block_pos & 1)
	sky_light = (sky_light >> 4) & 0xF;
    else sky_light = sky_light & 0xF;

    return BlockInfo(id, 0, 0, block_light, sky_light);
}

void MCACoder::loadMCA(const string &file_name)
{
    if (cur_file_name == file_name)
	return;

    if (cur_file_name != "")
    {
	writeMCA();
        for (int i = 0; i < K1; i++)
	    nbt_coder.Clear(Chunk[i]);
    }
    
    memset(Chunk, 0, sizeof(Chunk));
    
    cur_file_name = file_name;
    
    FILE* handle = fopen(cur_file_name.c_str(), "r");
    
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
	}
	else Chunk[i] = 0;
    
    fclose(handle);
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
	else sector_cnt = cmpr_len / K4 + 1;

	intToByte(sector_cnt, buffer, (i << 2) + 3, 1);
	cmpr_len = sector_cnt * K4; //Pad to a multiple of K4
	offset += cmpr_len;
    }
    
    for (int i = 0; i < K1; i++)
	intToByte(timestamp[i], buffer, K4 + (i << 2), 4);

    remove(cur_file_name.c_str());
    FILE* handle = fopen(cur_file_name.c_str(), "w");
    fwrite(buffer, 1, offset, handle);
    fclose(handle);

    cur_file_name = "";
    
}

node* MCACoder::newSubSection(int y)
{
    node* u;
    node* T = nbt_coder.nodeWithTypeName(TAG_COMPOUND, "none");

    u = nbt_coder.nodeWithTypeName(TAG_BYTE, "Y");
    nbt_coder.setIntContent(u, y);
    T->addChild(u);

    uc tmp[K4]; memset(tmp, 0, sizeof(tmp));

    u = nbt_coder.nodeWithTypeName(TAG_BYTE_ARRAY, "Blocks");
    nbt_coder.setByteArrayContent(u, tmp, K4);
    T->addChild(u);

    u = nbt_coder.nodeWithTypeName(TAG_BYTE_ARRAY, "Add");
    nbt_coder.setByteArrayContent(u, tmp, K2);
    T->addChild(u);

    u = nbt_coder.nodeWithTypeName(TAG_BYTE_ARRAY, "Data");
    nbt_coder.setByteArrayContent(u, tmp, K2);
    T->addChild(u);

    return T;
	//TODO??
}
