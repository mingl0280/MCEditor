#include "NBTCoder.h"
using namespace std;

node* NBTCoder::Decode(char* A)
{
    buffer = A;
    ull offset = 0;
    return decode(offset);
}

void NBTCoder::Print(node* T)
{
    print(T, 0);
}

node* NBTCoder::newNode()
{
    return (new node);
}

node* NBTCoder::decodePayload(int type, ull &offset)
{
    node* root = NULL;
    switch (type)
    {
        case 0: root = NULL; break;
        case 1: root = decodeByte(offset); break;
        case 2: root = decodeShort(offset); break;
        case 3: root = decodeInt(offset); break;
        case 4: root = decodeLong(offset); break;
        case 5: root = decodeFloat(offset); break;
        case 6: root = decodeDouble(offset); break;
        case 7: root = decodeByteArray(offset); break;
        case 8: root = decodeString(offset); break;
        case 9: root = decodeList(offset); break;
        case 10: root = decodeCompound(offset); break;
        case 11: root = decodeIntArray(offset); break;
        defualt: root = NULL;
    }
    return root;
}

node* NBTCoder::decode(ull &offset)
{
    int type = buffer[offset]; offset++;
    int name_len = (int)byteToInt(buffer, offset, 2); offset += 2;
    string name = byteToString(buffer, offset, name_len); offset += name_len;
    
    node *root = decodePayload(type, offset);
    
    if (!root) return root;
    root->tag.type = type; root->tag.name = name;
    return root;
}

node* NBTCoder::decodeByte(ull &offset)
{
    node* root = newNode();
    root->tag.vi = (char)byteToInt(buffer, offset, 1);
    offset += 1;
    return root;
}

node* NBTCoder::decodeShort(ull &offset)
{
    node* root = newNode();
    root->tag.vi = (short)byteToInt(buffer, offset, 2);
    offset += 2;
    return root;
}

node* NBTCoder::decodeInt(ull &offset)
{
    node* root = newNode();
    root->tag.vi = (int)byteToInt(buffer, offset, 4);
    offset += 4;
    return root;
}

node* NBTCoder::decodeLong(ull &offset)
{
    node* root = newNode();
    root->tag.vi = byteToInt(buffer, offset, 8);
    offset += 8;
    return root;
}

node* NBTCoder::decodeFloat(ull &offset)
{
    node* root = newNode();
    root->tag.vf = byteToFloat(buffer, offset, 4);
    offset += 4;
    return root;
}

node* NBTCoder::decodeDouble(ull &offset)
{
    node* root = newNode();
    root->tag.vf = byteToDouble(buffer, offset, 8);
    offset += 8;
    return root;
}

node* NBTCoder::decodeByteArray(ull &offset)
{
    node* root = newNode();
    int len = (int)byteToInt(buffer, offset, 4);
    offset += 4;

    root->tag.va.clear();
    for (int i = 0; i < len; i++)
    {
        root->tag.va.push_back((char)byteToInt(buffer, offset, 1));
        offset += 1;
    }
    
    return root;
}

node* NBTCoder::decodeString(ull &offset)
{
    node* root = newNode();

    int len = (short)byteToInt(buffer, offset, 2);
    offset += 2;
    
    root->tag.vs = byteToString(buffer, offset, len);
    offset += len;
    
    return root;
}

node* NBTCoder::decodeList(ull &offset)
{
    node* root = newNode();
    
    int type = (char)byteToInt(buffer, offset, 1);
    offset++;
    
    int len = (int)byteToInt(buffer, offset, 4);
    offset += 4;
    
    root->ch.clear();
    for (int i = 0; i < len; i++)
    {
        node* u = decodePayload(type, offset);
        u->tag.type = type;
        root->ch.push_back(u);
    }
    
    return root;
}

node* NBTCoder::decodeCompound(ull &offset)
{
    node* root = newNode();
    
    int type;
    while ((type = (char)byteToInt(buffer, offset, 1)))
        root->ch.push_back(decode(offset));
    offset++;
    
    return root;
}

node* NBTCoder::decodeIntArray(ull &offset)
{
    node* root = newNode();
    
    root->tag.va.clear();
    int len = (int)byteToInt(buffer, offset, 4);
    offset += 4;
    
    for (int i = 0; i < len; i++)
    {
        root->tag.va.push_back((int)byteToInt(buffer, offset, 4));
        offset += 4;
    }
    
    return root;
}

void NBTCoder::print(node* T, int d)
{
    for (int i = 0; i < d; i++)
	    printf("    ");
    printf("TYPE: %s,  NAME: %s\n",
	   TypeName[T->tag.type].c_str(), T->tag.name.c_str());
    for (int i = 0; i < d; i++)
	    printf("    ");
    printf("CONTENT: ");
    switch (T->tag.type)
    {
    case 1:
    case 2:
    case 3:
    case 4: printf("%lld", T->tag.vi); break;
    case 5:
    case 6: printf("%.4lf", T->tag.vf); break;
    case 8: printf("%s", T->tag.vs.c_str()); break;
    case 7:
    case 11: for (auto x : T->tag.va) printf("%d ", x); break;
    default: printf("No Content"); break;
    }
    printf("\n");

    for (node* v : T->ch)
	print(v, d + 1);
}
