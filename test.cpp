#include <cstdio>
#include <cstdlib>
#include "zlib.h"
#include "globals.h"
#include "NBTCoder.h"
using namespace std;

uc buffer[M64], chunk_file[M128];
NBTCoder coder;

int file_no;
int location[K1], timestamp[K1];

void inf(uc* dest, int dest_len, uc* src, int src_len)
{
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    
    infstream.avail_in = (ui)src_len;
    infstream.next_in = (uc*)src;
    infstream.avail_out = (ui)dest_len;
    infstream.next_out = (uc*)dest;

    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
}

int main()
{
    FILE* handle = fopen("r.0.1.mca", "r");
    
    fread(buffer, 1, K4, handle);
    for (int i = 0; i < K4; i += 4)
	location[i >> 2] = K4 * byteToInt(buffer, i, 3);
    
    fread(buffer, 1, K4, handle);
    for (int i = 0; i < K4; i += 4)
	timestamp[i >> 2] = K4 * byteToInt(buffer, i, 4);

    file_no = 0;
    for (int i = 0; i < K1; i++)
	if (location[i])
	{
	    //printf("Chunk #%d:\n", file_no);
	    fseek(handle, location[i], SEEK_SET);

	    fread(buffer, 1, 4, handle);
	    int chunk_len = byteToInt(buffer, 0, 4);
	    chunk_len--;
	    
	    fread(buffer, 1, 1, handle);
	    int cmpr_type = byteToInt(buffer, 0, 1);
	    //printf("Compression Type: %d\n", cmpr_type);

	    //printf("%d bytes expected, ", chunk_len);
	    chunk_len = fread(buffer, 1, chunk_len, handle);
	    //printf("%d bytes read\n", chunk_len);

	    inf(chunk_file, M128, buffer, chunk_len);

	    string file_name = "chunk"; file_name += to_string(file_no);
	    freopen(file_name.c_str(), "w", stdout);
	    coder.Encode(coder.Decode(chunk_file), chunk_file);

	    //printf("\n============================================\n");
	    file_no++;
	}
    
    return 0;
}
