
#include "Hexmap.h"
#include <cstdlib>

Hexmap::Hexmap(AssetRef<Program> program, AssetRef<Texture> baseTexture, 
        AssetRef<Texture> emblemTexture)
    : myHextiles(NULL), myProgram(program), myBaseTexture(baseTexture), 
    myEmblemTexture(emblemTexture) 
{
}

Hexmap::~Hexmap()
{
    if(myHextiles != NULL)
        delete [] myHextiles;
}

void Hexmap::Create(uint jSize, uint kSize)
{
    if(Scene.Get() == NULL)
        Debug("Hexmap %p has no scene!",this);
    if(Parent.Get() == NULL)
        Debug("Hexmap has no parent!");
    else
        if(Parent.Get()->Scene.Get() == NULL)
            Debug("Hexmap parent has no scene!");
    if(myHextiles != NULL)
        delete [] myHextiles;

    myJLength = jSize;
    myKLength = kSize;
    myHextiles = new TileData[jSize*kSize];

    for(uint j = 0; j < jSize; j++)
    {
        for(uint k = 0; k < kSize; k++)
        {
            real dist = 0.1;
            const VectorF4 jOffset(-(1.5+dist),-(0.87+dist),0);
            const VectorF4 kOffset(-(1.5+dist),0.87+dist,0);

            TileData &tile = myHextiles[Index(j,k)];
            tile.Mov.Transform.Set(MatrixF4::Translation(jOffset*j+kOffset*k) *
                    MatrixF4::RotationZ(rand()%6*Pi/3));
            tile.Mov.SetChild(&(tile.TileContainer));
            tile.EmblemMove.Transform.Set(MatrixF4::Translation(VectorF4(0,0,0.5)));
            tile.EmblemMove.SetChild(&(tile.Emblem));
            tile.TileContainer.AddChild(&(tile.Tile));
            tile.TileContainer.AddChild(&(tile.EmblemMove));
            tile.Tile.SetTexture(myBaseTexture);
            tile.Tile.SetProgram(myProgram);
            tile.Emblem.SetTexture(myEmblemTexture);
            AddChild(&(tile.Mov));
        }
    }
}

int TypeToIndex(char type)
{
    switch(type)
    {
        case 'G':
            return 0;
        case 'V':
            return 0;
        case 'S':
            return 0;
        case 'E':
            return 0;
        case 'R':
            return 1;
        case 'C':
            return 2;
        case 'O':
            return 0;
        default:
            throw Error(Error::InvalidValue, "Tile type not known");
    }
}

void Hexmap::SetTileType(uint j, uint k, char type)
{
    myHextiles[Index(j,k)].Tile.ProgramState().SetUniform("Frame", VectorI2(0,TypeToIndex(type)));
    myHextiles[Index(j,k)].Emblem.ProgramState().SetUniform("Frame", VectorI2(0,TypeToIndex(type)));
}
