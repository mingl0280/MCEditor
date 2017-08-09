#include <iostream>
#include <cmath>
#include <string>
#include "globals.h"
#include "MCEditor.h"
using namespace std;

struct RGB
{
    int r, g, b;    
    RGB() {}
    RGB(int r_, int g_, int b_)
        :r(r_), g(g_), b(b_) {}
    
    double operator - (const RGB &C) const
    {
        double r_ = (r + C.r) / 2.0;
        double dr = r - C.r, dg = g - C.g, db = b - C.b;
        double d1 = 2 + r_ / 256.0, d2 = 2 + (255 - r_) / 256.0;
        return sqrt(d1 * dr * dr + 4 * dg * dg + d2 * db * db);
    }
};

const RGB Preset[16] = {
    RGB(209, 214, 215), RGB(211, 105, 41), RGB(157, 58, 155), RGB(65, 136, 194),
    RGB(232, 178, 63), RGB(114, 168, 54), RGB(201, 109, 144), RGB(53, 56, 60),
    RGB(125, 125, 116), RGB(53, 118, 134), RGB(92, 36, 152), RGB(41, 48, 139),
    RGB(92, 60, 33), RGB(76, 90, 40), RGB(133, 40, 35), RGB(1, 1, 1)
};

int n, m;
int Color[512][512];
int A[512][512][256] = {0}, B[512][512][256] = {0};
MCEditor editor;

int main()
{
    int x0, y0, z0;
    fscanf(stdin, "%d %d %d", &x0, &y0, &z0);
    
    FILE *in = fopen("bmp.txt", "r");
    
    int r, g, b;
    fscanf(in, "%d%d", &n, &m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            fscanf(in, "%d%d%d", &r, &g, &b);
            RGB pixel_color = RGB(r, g, b);
            
            double idx = 0, res = 1e9;
            for (int k = 0; k < 16; k++)
            {
                double dis = pixel_color - Preset[k];
                if (dis < res)
                {
                    res = dis;
                    idx = k;
                }
            }
            Color[i][j] = idx;
        }
    fclose(in);
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            A[j][i][0] = 251;
            B[j][i][0] = Color[i][j];
        }
    
    editor.setRegion(A, B, m, n, 40, x0, z0, y0);
    
    return 0;
}
