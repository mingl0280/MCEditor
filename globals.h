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

//compress src into dest
ull compress(uc *dest, ull dest_len, uc* src, ull src_len);

#endif /* globals_h */
