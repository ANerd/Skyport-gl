#ifndef HEXMAP_H_
#define HEXMAP_H_

#include "entity/Container.h"
#include "Hextile.h"

class Hexmap : public MultiContainer
{
    struct TileData
    {
        Hextile Tile;
        Movable Mov;
    };
    TileData *myHextiles;
    uint myM;
    uint myN;

    uint Index(uint j, uint k)
    {
        return j * myM + k;
    }

    public:
    Hexmap(uint m, uint n, AssetRef<Program> program);
    virtual ~Hexmap();
};

#endif
