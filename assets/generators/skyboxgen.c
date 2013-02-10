
#include <math.h>
#include <stdio.h>


void EmmitVertex(int i, double positions[], double texcoords[], FILE *file)
{
    fprintf(file, "%g %g %g %g %g %g\n", positions[i*3+0],positions[i*3+1],
        positions[i*3+2], 0.0, 
        texcoords[i*2+0], texcoords[i*2+1]);
    //fprintf(file, "%g %g %g %g 0.0 0.0\n", texcoords[i*2+0], texcoords[i*2+1],
    //    positions[i*4+2], positions[i*4+3]);
}

int wrap(int val, int max)
{
    if(val >= max)
        return val - max;
    if(val < 0)
        return val + max;
    return val;
}

static const SideOffset[2*6] = 
{
    0,4,
    0,5,
    0,0,
    0,1,
    0,2,
    0,3
};

int main(int argc, char *argv[])
{
    double angleInc = 2*M_PI/6;
    double angleFleng = M_PI/6;

    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "w");
    if(!file)
    {
        fprintf(stderr, "Failed to open %s:", argv[1]);
        perror("");
    }

    fprintf(file, "A2 Position float4 false 24 0 Texcoord float2 false 24 16 1 216\n");

    int side;
    for(side = 0; side < 6; side++)
    {
        int dir = side / 2;
        int sign = side % 2;
        int signdir = sign * 2 - 1;

        double pos[3*4] = {0};
        double tex[2*4] = {0};

        int j;
        for(j = 0; j < 2; j++)
        {
            int k;
            for(k = 0; k < 2; k++)
            {
                pos[(j*2+k)*3+dir] = (sign-0.5);
                pos[(j*2+k)*3+wrap(dir+signdir, 3)] = j - 0.5;
                pos[(j*2+k)*3+wrap(dir+signdir*2, 3)] = k - 0.5;
                double texX = dir == 0 ? -pos[(j*2+k)*3 + 2] : pos[(j*2+k)*3 + 0];
                double texY = dir == 1 ? pos[(j*2+k)*3 + 2] : pos[(j*2+k)*3 + 1];
                tex[(j*2+k)*2+0] = (SideOffset[side*2+0] + -signdir * texX + 0.5);
                tex[(j*2+k)*2+1] = (SideOffset[side*2+1] + texY + 0.5)/6.0;
            }
        }

        EmmitVertex(0, pos, tex, file);
        EmmitVertex(1, pos, tex, file);
        EmmitVertex(2, pos, tex, file);
        EmmitVertex(1, pos, tex, file);
        EmmitVertex(3, pos, tex, file);
        EmmitVertex(2, pos, tex, file);
    }

    fclose(file);
    return 0;
}
