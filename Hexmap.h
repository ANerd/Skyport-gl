#ifndef HEXMAP_H_
#define HEXMAP_H_

#include "entity/Container.h"
#include "Hextile.h"
#include "Hexborder.h"
#include "entity/Billboard.h"

class Hexmap : public MultiContainer
{
    static const uint TileTypeCount = 7;
    struct TileData
    {
        Hextile Tile;
        Hexborder Border;
        Billboard Emblem;
        Movable Mov;
        Movable EmblemMove;
        MultiContainer TileContainer;
    };
    TileData *myHextiles;
    uint myJLength;
    uint myKLength;
    AssetRef<Program> myProgramRef;
    Asset<Program> myProgram;
    AssetRef<Texture> myBaseTextureRef;
    Asset<Texture> myBaseTexture;
    AssetRef<Texture> myEmblemTexture;

    uint Index(uint j, uint k)
    {
        return k * myJLength + j;
    }

    /*virtual void OnCreate()
    {
        Debug("Hexmap created: %p",this);
        MultiContainer::OnCreate();
    }*/

    ProgramStateId myProgramStates[TileTypeCount];

    public:
    static const real TileDistance;
    static const VectorF2 jOffset;
    static const VectorF2 kOffset;
    Hexmap(AssetRef<Program> program, AssetRef<Texture> baseTexture, 
            AssetRef<Texture> emblemTexture);
    virtual ~Hexmap();

    virtual void OnCreate();
    virtual void OnDestroy();

    void Create(uint jSize, uint kSize);
    void SetTileType(uint j, uint k, char type);
};

#endif
