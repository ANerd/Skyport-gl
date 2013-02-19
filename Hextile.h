#ifndef HEXTILE_H_
#define HEXTILE_H_

#include "entity/FixedGeometry.h"
#include "assets/AssetManager.h"

using namespace anengine;

class Hextile : public FixedGeometry
{
    static StaticAsset<VertexBuffer> myVertexBuffer;
    protected:
    virtual AssetRef<VertexBuffer> GetGeometry(AssetManager *manager)
    {
        return myVertexBuffer.Get(manager);
    }
    virtual void OnDraw(FrameTime elapsed)
    {
        glDrawArrays(GL_TRIANGLES, 0, 48);
    }
    public:
    virtual ~Hextile() { }
};

#endif
