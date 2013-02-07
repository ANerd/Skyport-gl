
#include "Hexmap.h"
#include <cstdlib>

Hexmap::Hexmap(uint m, uint n, AssetRef<Program> program, AssetRef<Texture> tex)
    : myHextiles(new TileData[m*n]), myM(m), myN(n)
{
    for(uint j = 0; j < m; j++)
    {
        for(uint k = 0; k < m; k++)
        {
            real dist = 0.1;
            const VectorF4 jOffset(-(1.5+dist),-(0.87+dist),0);
            const VectorF4 kOffset(-(1.5+dist),0.87+dist,0);
            TileData &tile = myHextiles[Index(j,k)];
            tile.Mov.Transform.Set(MatrixF4::Translation(jOffset*j+kOffset*k) *
                    MatrixF4::RotationZ(rand()%6*Pi/3));
            tile.Mov.SetChild(&(tile.Tile));
            tile.Tile.SetTexture(tex);
            tile.Tile.SetProgram(program);
            AddChild(&(tile.Mov));
        }
    }
}

Hexmap::~Hexmap()
{
    delete [] myHextiles;
}