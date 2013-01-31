
#include <math.h>
#include <stdio.h>


double TexcoordOffset(double texcoord)
{
    double texcoordMargin =0;// 10.0/128;
    double texcoordWidth = 1-2*texcoordMargin;
    return texcoordMargin + texcoord * texcoordWidth;
}

void EmmitVertex(int i, double positions[], double texcoords[], FILE *file)
{
    fprintf(file, "%g %g %g %g %g %g\n", positions[i*4+0],positions[i*4+1],
        positions[i*4+2], positions[i*4+3], 
        TexcoordOffset(texcoords[i*2+0]), TexcoordOffset(texcoords[i*2+1]));
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

    fprintf(file, "A2 Position float4 false 24 0 Texcoord float2 false 24 16 1 288\n");
    //fprintf(file, "A1 Position float4 false 24 0 1 288\n");
    double positions[18*4];
    double texcoords[18*2];
    double flengHeight = 0.4;
    double totalHeight = (2+cos(angleFleng)*flengHeight*2)/2;

    int corner;
    for(corner = 0; corner < 6; corner++)
    {
        double cornerAngle = angleInc * corner;
        int fleng;
        int positionOffset = 3*corner*4;
        int texcoordOffset = 3*corner*2;
        positions[positionOffset + 0] = cos(cornerAngle);
        positions[positionOffset + 1] = sin(cornerAngle);
        positions[positionOffset + 2] = 0;
        positions[positionOffset + 3] = 1;

        texcoords[texcoordOffset + 0] = cos(cornerAngle) / (totalHeight*2)+0.5;
        texcoords[texcoordOffset + 1] = sin(cornerAngle) / (totalHeight*2)+0.5;
        for(fleng = 0; fleng < 2; fleng++)
        {
            double flengAngle = cornerAngle + (fleng*2-1) * angleFleng;
            positions[positionOffset + 4*(fleng+1) + 0] = cos(cornerAngle);
            positions[positionOffset + 4*(fleng+1) + 1] = sin(cornerAngle);
            positions[positionOffset + 4*(fleng+1) + 2] = -flengHeight;
            positions[positionOffset + 4*(fleng+1) + 3] = 1;

            texcoords[texcoordOffset + 2*(fleng+1) + 0] = 
                (cos(cornerAngle) + cos(flengAngle)*flengHeight)/(totalHeight*2)+0.5;
            texcoords[texcoordOffset + 2*(fleng+1) + 1] = 
                (sin(cornerAngle) + sin(flengAngle)*flengHeight)/(totalHeight*2)+0.5;
        }
    }

    // Emmit top
    int i;
    for(i = 1; i < 5; i++)
    {
        EmmitVertex(0*3     , positions, texcoords, file);
        EmmitVertex(i*3     , positions, texcoords, file);
        EmmitVertex((i+1)*3 , positions, texcoords, file);
    }
    for(i = 0; i < 6; i++)
    {
        EmmitVertex(i*3             , positions, texcoords, file);
        EmmitVertex(i*3+2           , positions, texcoords, file);
        EmmitVertex(wrap(i+1,6)*3+1 , positions, texcoords, file);
        EmmitVertex(i*3             , positions, texcoords, file);
        EmmitVertex(wrap(i+1,6)*3+1 , positions, texcoords, file);
        EmmitVertex(wrap(i+1,6)*3   , positions, texcoords, file);
    }

    fclose(file);
    return 0;
}
