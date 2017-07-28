#ifndef NBTCoder_h
#define NBTCoder_h

#include <cstdio>
#include <list>
#include <string>
#include <algorithm>
#include <cstring>
#include "globals.h"

struct data
{
    int type; std::string name;
    ll vi; double vf; std::string vs; std::list<int> va;
    data(): type(-1), name("none") {}
};

struct node
{
    data tag;
    std::list<node*> ch;
};

class NBTCoder
{
public:
    node* Decode(char* A);
    void Print(node* T);
    void Clear(node* T);
    
private:
    char* buffer;
    
    node* newNode();

    node* decodePayload(int type, ull &offset);
    node* decode(ull &offset);
    node* decodeByte(ull &offset);
    node* decodeShort(ull &offset);
    node* decodeInt(ull &offset);
    node* decodeLong(ull &offset);
    node* decodeFloat(ull &offset);
    node* decodeDouble(ull &offset);
    node* decodeByteArray(ull &offset);
    node* decodeString(ull &offset);
    node* decodeList(ull &offset);
    node* decodeCompound(ull &offset);
    node* decodeIntArray(ull &offset);

    void print(node* T, int d);
};

#endif
