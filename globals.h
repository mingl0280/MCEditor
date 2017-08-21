#ifndef globals_h
#define globals_h

#include <string>
#include <cstring>
#include <algorithm>

//////////////////////////////MACROS & CONSTANTS///////////////////////

#define K1 0x400ULL
#define K2 0x800ULL
#define K4 0x1000ULL
#define M1 0x100000ULL
#define M2 0x200000ULL
#define M4 0x400000ULL
#define M8 0x800000ULL
#define M16 0x1000000ULL
#define M32 0x2000000ULL
#define M64 0x4000000ULL
#define M128 0x8000000ULL
#define M256 0x10000000ULL
#define M512 0x20000000ULL
#define M1024 0x40000000ULL
#define G1 0x40000000ULL
#define G2 0x80000000ULL
#define G4 0x100000000ULL
#define G8 0x200000000ULL
#define G16 0x400000000ULL
#define G32 0x800000000ULL
#define G64 0x1000000000ULL

typedef unsigned int ui;
typedef unsigned long long ull;
typedef unsigned short us;
typedef unsigned char uc;
typedef long long ll;

#define TAG_END 0
#define TAG_BYTE 1
#define TAG_SHORT 2
#define TAG_INT 3
#define TAG_LONG 4
#define TAG_FLOAT 5
#define TAG_DOUBLE 6
#define TAG_BYTE_ARRAY 7
#define TAG_STRING 8
#define TAG_LIST 9
#define TAG_COMPOUND 10
#define TAG_INT_ARRAY 11

extern std::string TypeName[18];

/////////////////////////////////FUNCTIONS//////////////////////////////

//reverse [offset, offset + len) in buffer
void reverse_(uc* buffer, ull offset, ull len);

//convert bytes to an integer value (big endian)
ll byteToInt(uc* buffer, ull offset, ull len);

//convert bytes to a float value (big endian)
float byteToFloat(uc* buffer, ull offset, ull len);

//convert bytes to a double value (big endian)
double byteToDouble(uc* buffer, ull offset, ull len);

//convert bytes to a string value ("little endian")
std::string byteToString(uc* buffer, ull offset, ull len);

//convert an integer value to bytes (big endian)
void intToByte(ll v, uc* buffer, ull offset, ull len);

//convert a float value to bytes (big endian)
void floatToByte(float v, uc* buffer, ull offset, ull len);

//convert a double value to bytes (big endian)
void doubleToByte(double v, uc* buffer, ull offset, ull len);

//convert a string to bytes ("little endian")
void stringToByte(const std::string &str, uc* buffer, ull offset, ull len);

//decompress src into dest
void decompress(uc *dest, ull dest_len, uc* src, ull src_len);

//compress src into dest and return the length of compressed data
ull compress(uc *dest, ull dest_len, uc* src, ull src_len);

//get the opacity of block id
int get_opacity(ui id);

//get the block light
int get_block_light(ui id);

/////////////////////////////////STRUCTs//////////////////////////////

struct Pos
{
    int x, z, y;

    Pos() {}
    Pos(int x_, int z_, int y_):
        x(x_), z(z_), y(y_) {};

    bool operator < (const Pos &B) const
    {
        int ax = x >> 9, bx = B.x >> 9;
        if (ax != bx)
            return ax < bx;
        else return (z >> 9) < (B.z >> 9);
    }

    bool operator == (const Pos &B) const
    {
        return x == B.x && z == B.z && y == B.y;
    }
};

struct BlockInfo
{
    ui id, add, data;
    ui block_light, sky_light;

    BlockInfo(): id(0), add(0), data(0),
                 block_light(0), sky_light(15) {};

    BlockInfo(ui id_, ui add_, ui data_,
                ui block_light_ = 0, ui sky_light_ = 0)
            : id(id_), add(add_), data(data_),
                block_light(block_light_), sky_light(sky_light_) {};
};

struct Block
{
    Pos position;
    BlockInfo info;

    Block() {};
    Block(const Pos &position_, const BlockInfo &info_)
        : position(position_), info(info_) {};

    bool operator < (const Block &B) const
    {
        return position < B.position;
    }
};

#endif /* globals_h */
