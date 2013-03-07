
#include "Hexmap.h"
#include <cstdlib>

const real Hexmap::TileDistance = 0.05f;
StaticAsset<Program> Hexmap::myHexborderProgramRef(AssetManager
            ::CreateStaticFromFile<Program>("assets/shaders/hexborder.sp"));
StaticAsset<Program> Hexmap::myHextileProgramRef(AssetManager
            ::CreateStaticFromFile<Program>("assets/shaders/Ground.sp"));
const VectorF2 Hexmap::jOffset( (1.5+TileDistance),-(0.87+TileDistance));
const VectorF2 Hexmap::kOffset(-(1.5+TileDistance),-(0.87+TileDistance));

Hexmap::Hexmap(AssetRef<Texture> baseTexture, AssetRef<Texture> emblemTexture)
    : myHextiles(NULL), myBaseTextureRef(baseTexture), 
    myEmblemTextureRef(emblemTexture) { }

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
            //tile.EmblemMove.Transform.Set(MatrixF4::Translation(VectorF4(0,0,0.5)));
            //tile.EmblemMove.SetChild(&(tile.Emblem));
            tile.TileContainer.AddChild(&(tile.Tile));
            tile.TileContainer.AddChild(&(tile.Emblem));
            tile.TileContainer.AddChild(&(tile.Border));
            tile.Tile.SetProgram(myHextileProgram, myHextileProgramStates[0]);
            tile.Border.SetProgram(myHexborderProgram, myHexborderProgramStates[0]);
            tile.Emblem.SetProgram(myEmblemProgram, myEmblemProgramStates[0]);
            AddChild(&(tile.Mov));
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

static const ColorF HexborderColors[Hexmap::TileTypeCount] = {
    ColorF(0,0,0,0),
    ColorF(0.48, 0.45, 0.04, 1),
    ColorF(0.24, 0.20, 0.16, 1),
    ColorF(0.29, 0.05, 0.04, 1),
    ColorF(0.37, 0.37, 0.37, 1),
    ColorF(0.20+0.1, 0.28+0.3, 0.00+0.3, 1),
    ColorF(0.44, 0.40, 0.16, 1)
};

void Hexmap::OnCreate()
{
    myHextileProgram = myHextileProgramRef.Get(Scene.Get()->GetAssetManager());
    myHexborderProgram = myHexborderProgramRef.Get(Scene.Get()->GetAssetManager());
    myBaseTexture = myBaseTextureRef;
    myEmblemTexture = myEmblemTextureRef;
    myEmblemProgram = Billboard::BillboardProgram.Get(Scene.Get()->GetAssetManager());
    for(uint i = 0; i < TileTypeCount; i++)
    {
        myHextileProgramStates[i] = myHextileProgram->CreateState();
        Program::ProgramState &hstate = myHextileProgram->GetState(myHextileProgramStates[i]);
        hstate.SetUniform("FrameCount", VectorI2(1,TileTypeCount));
        hstate.SetUniform("Frame", VectorI2(0,i));
        hstate.SetUniform("Texture", myBaseTexture);

        myHexborderProgramStates[i] = myHexborderProgram->CreateState();
        Program::ProgramState &bstate = myHexborderProgram->GetState(myHexborderProgramStates[i]);
        bstate.SetUniform("Color", HexborderColors[i]);

        myEmblemProgramStates[i] = myEmblemProgram->CreateState();
        Program::ProgramState &estate = myEmblemProgram->GetState(myEmblemProgramStates[i]);
        estate.SetUniform("FrameCount", VectorI2(1,TileTypeCount));
        estate.SetUniform("Frame", VectorI2(0,i));
        estate.SetUniform("Texture", myEmblemTexture);
        estate.SetUniform("Offset", VectorF2(0,0.5f));
    }
    MultiContainer::OnCreate();
}

void Hexmap::OnDestroy()
{
    myHextileProgram.Release();
    myHexborderProgram.Release();
    myEmblemProgram.Release();
    myBaseTexture.Release();
    myEmblemTexture.Release();

    MultiContainer::OnDestroy();
}

void Hexmap::SetTileType(uint j, uint k, char type)
{
    myHextiles[Index(j,k)].Tile.SetProgramState(
            myHextileProgramStates[TypeToIndex(type)]);
    myHextiles[Index(j,k)].Border.SetProgramState(
            myHexborderProgramStates[TypeToIndex(type)]);
    myHextiles[Index(j,k)].Emblem.SetProgramState(
            myEmblemProgramStates[TypeToIndex(type)]);
}
