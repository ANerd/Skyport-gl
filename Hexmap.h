#ifndef HEXMAP_H_
#define HEXMAP_H_

#include "entity/Container.h"
#include "Hextile.h"
#include "Hexborder.h"
#include "entity/Billboard.h"

class Hexmap : public MultiContainer
{
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
    AssetRef<Program> myProgram;
    AssetRef<Texture> myBaseTexture;
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

    public:
    static const real TileDistance;
    static const VectorF2 jOffset;
    static const VectorF2 kOffset;
    Hexmap(AssetRef<Program> program, AssetRef<Texture> baseTexture, 
            AssetRef<Texture> emblemTexture);
    virtual ~Hexmap();

    void Create(uint jSize, uint kSize);
    void SetTileType(uint j, uint k, char type);
};

#endif
