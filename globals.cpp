#include "globals.h"
using namespace std;

std::string TypeName[18] = {"", "Byte", "Short", "Int", "Long Long",
			   "Float", "Double", "Byte Array", "String",
			   "List", "Compound", "Int Array"};

//convert bytes to a integer value (big endian)
ll byteToInt(char* buffer, ull offset, ull len)
{
    ll res = 0;
    for (int i = 0; i < len - 1 - i; i++)
        swap(buffer[offset + i], buffer[offset + len - 1 - i]);
    memcpy(&res, buffer + offset, len);
    return res;
}

//convert bytes to a float value (big endian)
float byteToFloat(char* buffer, ull offset, ull len)
{
    float res = 0;
    for (int i = 0; i < len - 1 - i; i++)
        swap(buffer[offset + i], buffer[offset + len - 1 - i]);
    memcpy(&res, buffer + offset, len);
    return res;
}

//convert bytes to a double value (big endian)
double byteToDouble(char* buffer, ull offset, ull len)
{
    double res = 0;
    for (int i = 0; i < len - 1 - i; i++)
        swap(buffer[offset + i], buffer[offset + len - 1 - i]);
    memcpy(&res, buffer + offset, len);
    return res;
}

//convert bytes to a string value ("little endian")
string byteToString(char* buffer, ull offset, ull len)
{
    string res = "";
    for (int i = (int)offset; i < offset + len; i++)
        res += buffer[i];
    return res;
}
