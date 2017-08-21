#include <cstdio>
#include "MCEditor.h"
#include "BlockEntity.h"
#include <vector>
#include <cstdlib>
using namespace std;

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

struct event
{
    int offset, pitch;
    event() {}
    event(int offset_, int pitch_)
        : offset(offset_), pitch(pitch_) {}

    bool operator < (const event &E) const
    {
        return offset < E.offset;
    }
};

vector<event> E;
vector<vector<event> > Track;
vector<int> TrackOffset;

void setNoteBlockWithPitch(MCRegion &R, int x, int z, int y, int pitch);
void setRepeaterWithDelay(MCRegion &R, int x, int z, int y, int delay);
void setRedStone(MCRegion &R, int x, int z, int y);
void setConcrete(MCRegion &R, int x, int z, int y);

void setNoteBlockWithPitch(MCRegion &R, int x, int z, int y, int pitch)
{
    if (pitch < 30 || pitch > 102)
    {
        setRedStone(R, x, z, y);
        setConcrete(R, x, z, y - 1);
        return;
    } //out of range

    int note = 0;

    if (30 <= pitch && pitch <= 41)
    {
        note = pitch - 30;
        R.A[x][z][y - 1] = BlockInfo(5, 0, 2); //Bass: Birch Wood Planks
    }
    else if (42 <= pitch && pitch <= 53)
    {
        note = pitch - 42;
        R.A[x][z][y - 1] = BlockInfo(35, 0, 0); //Guitar: White Wool
    }
    else if (54 <= pitch && pitch <= 78)
    {
        note = pitch - 54;
        R.A[x][z][y - 1] = BlockInfo(42, 0, 0); //Piano: Iron Block
    }
    else if (79 <= pitch && pitch <= 102)
    {
        note = pitch - 78;
        R.A[x][z][y - 1] = BlockInfo(41, 0, 0); //Bell: Gold Block
    }
    R.B[x][z][y - 1] = 0;

    R.A[x][z][y] = BlockInfo(25, 0, 0); //noteblock
    Pos position = Pos(R.x_ori + x, R.z_ori + z, R.y_ori + y);
    R.B[x][z][y] = new BlockEntityNote(position, note, 0);
}

void setRepeaterWithDelay(MCRegion &R, int x, int z, int y, int delay)
{
    ui data = ((delay - 1) << 2) | EAST;
    R.A[x][z][y] = BlockInfo(93, 0, data);
    R.B[x][z][y] = 0;
}

void setRedStone(MCRegion &R, int x, int z, int y)
{
    R.A[x][z][y] = BlockInfo(55, 0, 0);
    R.B[x][z][y] = 0;
}

void setConcrete(MCRegion &R, int x, int z, int y)
{
    R.A[x][z][y] = BlockInfo(251, 0, 0);
    R.B[x][z][y] = 0;
}

int max_len = 0;
MCEditor Editor;

void buildTrack(MCRegion &R, int k)
{
    int offset = 0;
    int x_offset = 0;
    for (auto e : Track[k])
    {
        int delta = e.offset - offset;
        int q = delta / 4, r = delta % 4;

        for (int x = x_offset; x < x_offset + q; x++)
        {
            setConcrete(R, x, k << 1, 0);
            setRepeaterWithDelay(R, x, k << 1, 1, 4);
        }
        x_offset += q;

        if (r > 0)
        {
            setConcrete(R, x_offset, k << 1, 0);
            setRepeaterWithDelay(R, x_offset, k << 1, 1, r);
            x_offset++;
        }

        setNoteBlockWithPitch(R, x_offset, k << 1, 1, e.pitch);
        x_offset++;

        offset = e.offset;
    }
    max_len = max(max_len, x_offset);
}

int main()
{
    int x0, z0, y0;
    scanf("%d%d%d", &x0, &y0, &z0);

    FILE* handle = fopen("notes.txt", "r");
    if (!handle)
    {
        fprintf(stderr, "File not found.\n");
        exit(0);
    }

    int n;
    fscanf(handle, "%d", &n);
    for (int i = 0; i < n; i++)
    {
        int offset, pitch;
        fscanf(handle, "%d%d", &offset, &pitch);
        E.push_back(event(offset + 5, pitch));
    }
    fclose(handle);
    sort(E.begin(), E.end());
    
    int track_cnt = 0;
    for (int l = 0; l < n;)
    {
        int r = l;
        while (E[r].offset == E[l].offset && r < n)
            r++;
        track_cnt = max(track_cnt, r - l);
        l = r;
    }

    for (int i = 0; i < track_cnt; i++)
    {
        TrackOffset.push_back(0);
        Track.push_back(vector<event>());
    }

    int max_offset = 0;
    for (auto it = E.begin(); it != E.end();)
        for (int i = 0; i < track_cnt && it != E.end(); i++)
        {
            Track[i].push_back(*it);
            TrackOffset[i] = it->offset;
            max_offset = max(max_offset, it->offset);
            it++;
        }
    
    MCRegion Region(x0, z0, y0,
                    max_offset, track_cnt << 1, 5);

    for (int i = 0; i < track_cnt; i++)
        buildTrack(Region, i);
    Region.x_len = max_len + 10;

    Editor.setRegion(Region);

    return 0;
}
