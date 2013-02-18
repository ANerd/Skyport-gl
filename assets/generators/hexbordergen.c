
#include <math.h>
#include <stdio.h>

#define X 0
#define Y 1
#define Z 2
#define ElementCount 3

void EmmitVertex(int i, double positions[], FILE *file)
{
    fprintf(file, "%g %g %g 1.0\n", positions[i*3+0],positions[i*3+1],
        positions[i*3+2]);
        
    //fprintf(file, "%g %g %g %g 0.0 0.0\n", texcoords[i*2+0], texcoords[i*2+1],
    //    positions[i*4+2], positions[i*4+3]);
}

int wrap(int val, int max)
{
    if(val >= max)
        return val - max;
    return val;
}

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

    fprintf(file, "A1 Position float4 false 16 0 1 144\n");
    //fprintf(file, "A1 Position float4 false 24 0 1 288\n");
    double positions[12*3];

    double innerRadius = 0.9;

    int corner;
    for(corner = 0; corner < 6; corner++)
    {
        double cornerAngle = angleInc * corner;
        int positionOffset = 2*corner*ElementCount;
        positions[positionOffset + X] = cos(cornerAngle);
        positions[positionOffset + Y] = sin(cornerAngle);
        positions[positionOffset + Z] = 0.001;

        int i;
        for(i = 0; i < 3; i++)
            positions[positionOffset + ElementCount + i] = positions[positionOffset + i] * innerRadius;
    }

    // Emmit top
    int i;
    for(i = 0; i < 6; i++)
    {
        EmmitVertex(wrap(i*2+0,12), positions, file);
        EmmitVertex(wrap(i*2+2,12), positions, file);
        EmmitVertex(wrap(i*2+1,12), positions, file);
        EmmitVertex(wrap(i*2+1,12), positions, file);
        EmmitVertex(wrap(i*2+2,12), positions, file);
        EmmitVertex(wrap(i*2+3,12), positions, file);
    }

    fclose(file);
    return 0;
}
