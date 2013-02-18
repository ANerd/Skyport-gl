#ifndef HEXBORDER_H_
#define HEXBORDER_H_

#include "entity/FixedGeometry.h"
#include "assets/AssetManager.h"

using namespace anengine;

class Hexborder : public FixedGeometry
{
    static StaticAsset<VertexBuffer> myVertexBuffer;
    static StaticAsset<Program> myProgram;
    protected:
    virtual AssetRef<VertexBuffer> GetGeometry(AssetManager *manager)
    {
        return myVertexBuffer.Get(manager);
    }
    virtual void OnDraw(FrameTime elapsed)
    {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    virtual void OnCreate()
    {
        SetProgram(myProgram.Get(Scene.Get()->GetAssetManager()));
        FixedGeometry::OnCreate();
    }

    public:
    virtual ~Hexborder() { }
};

#endif
