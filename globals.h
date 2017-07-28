#ifndef globals_h
#define globals_h

#include <string>
#include <cstring>
#include <algorithm>

//////////////////////////////MACROS & CONSTANTS///////////////////////

#define K1 0x400ULL
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

extern std::string TypeName[18];

/////////////////////////////////FUNCTIONS//////////////////////////////

//convert bytes to a integer value (big endian)
ll byteToInt(char* buffer, ull offset, ull len);

//convert bytes to a float value (big endian)
float byteToFloat(char* buffer, ull offset, ull len);

//convert bytes to a double value (big endian)
double byteToDouble(char* buffer, ull offset, ull len);

//convert bytes to a string value ("little endian")
std::string byteToString(char* buffer, ull offset, ull len);

#endif /* globals_h */
