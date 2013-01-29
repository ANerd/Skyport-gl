
#include "Hexmap.h"

Hexmap::Hexmap(uint m, uint n, AssetRef<Program> program)
    : myHextiles(new TileData[m*n]), myM(m), myN(n)
{
    for(uint j = 0; j < m; j++)
    {
        for(uint k = 0; k < m; k++)
        {
            const VectorF4 jOffset(0.87,1.25,0);
            const VectorF4 kOffset(0.87,-1.25,0);
            TileData &tile = myHextiles[Index(j,k)];
            tile.Mov.Transform.Set(MatrixF4::Translation(jOffset*j+kOffset*k));
            tile.Mov.SetChild(&(tile.Tile));
            tile.Tile.SetProgram(program);
            AddChild(&(tile.Mov));
        }
    }
}

Hexmap::~Hexmap()
{
    delete [] myHextiles;
}
