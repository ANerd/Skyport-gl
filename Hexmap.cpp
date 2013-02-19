
#include "Hexmap.h"
#include <cstdlib>

const real Hexmap::TileDistance = 0.05f;
const VectorF2 Hexmap::jOffset( (1.5+TileDistance),-(0.87+TileDistance));
const VectorF2 Hexmap::kOffset(-(1.5+TileDistance),-(0.87+TileDistance));

Hexmap::Hexmap(AssetRef<Program> program, AssetRef<Texture> baseTexture, 
        AssetRef<Texture> emblemTexture)
    : myHextiles(NULL), myProgramRef(program), myBaseTextureRef(baseTexture), 
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
            VectorF4 mjOffset(-jOffset[X],jOffset[Y],0);
            VectorF4 mkOffset(-kOffset[X],kOffset[Y],0);

            TileData &tile = myHextiles[Index(j,k)];
            tile.Mov.Transform.Set(MatrixF4::Translation(mjOffset*j+mkOffset*k)*
                    MatrixF4::RotationZ(rand()%6*Pi/3));
            tile.Mov.SetChild(&(tile.TileContainer));
            tile.EmblemMove.Transform.Set(MatrixF4::Translation(VectorF4(0,0,0.5)));
            tile.EmblemMove.SetChild(&(tile.Emblem));
            tile.TileContainer.AddChild(&(tile.Tile));
            tile.TileContainer.AddChild(&(tile.EmblemMove));
            tile.TileContainer.AddChild(&(tile.Border));
            tile.Tile.SetProgram(myProgram, myProgramStates[0]);
            tile.Emblem.SetTexture(myEmblemTexture);
            AddChild(&(tile.Mov));
            tile.Emblem.ProgramState().SetUniform("FrameCount", VectorI2(1,7));
            tile.Border.ProgramState().SetUniform("Color", ColorF(1,0,0,1));
        }
    }
}

int TypeToIndex(char type)
{
    switch(type)
    {
        case 'G':
            return 5;
        case 'V':
            return 0;
        case 'S':
            return 1;
        case 'E':
            return 6;
        case 'R':
            return 3;
        case 'C':
            return 2;
        case 'O':
            return 4;
        default:
            throw Error(Error::InvalidValue, "Tile type not known");
    }
}

void Hexmap::OnCreate()
{
    myProgram = myProgramRef;
    myBaseTexture = myBaseTextureRef;
    for(uint i = 0; i < TileTypeCount; i++)
    {
        myProgramStates[i] = myProgram->CreateState();
        Program::ProgramState &state = myProgram->GetState(myProgramStates[i]);
        state.SetUniform("FrameCount", VectorI2(1,TileTypeCount));
        state.SetUniform("Frame", VectorI2(0,i));
        state.SetUniform("Texture", myBaseTexture);
    }
    MultiContainer::OnCreate();
}

void Hexmap::OnDestroy()
{
    myProgram.Release();
    myBaseTexture.Release();
    MultiContainer::OnDestroy();
}

void Hexmap::SetTileType(uint j, uint k, char type)
{
    myHextiles[Index(j,k)].Tile.SetProgramState(
            myProgramStates[TypeToIndex(type)]);
    //myHextiles[Index(j,k)].Tile.ProgramState().SetUniform("Frame", VectorI2(0, TypeToIndex(type)));
    myHextiles[Index(j,k)].Emblem.ProgramState().SetUniform("Frame", VectorI2(0,TypeToIndex(type)));
}
