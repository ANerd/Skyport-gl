#ifndef HEXTILE_H_
#define HEXTILE_H_

#include "entity/FixedGeometry.h"
#include "assets/AssetManager.h"

using namespace anengine;

class Hextile : public FixedGeometry
{
    static StaticAsset<VertexBuffer> myVertexBuffer;
    AssetRef<Texture> myTextureRef;
    Asset<Texture> myTexture;
    protected:
    virtual AssetRef<VertexBuffer> GetGeometry(AssetManager *manager)
    {
        return myVertexBuffer.Get(manager);
    }
    virtual void OnDraw(FrameTime elapsed)
    {
        glDrawArrays(GL_TRIANGLES, 0, 48);
    }

    virtual void OnCreate()
    {
        myTexture = myTextureRef;
        FixedGeometry::OnCreate();
    }

    virtual void OnDestroy()
    {
        myTexture.Release();
        FixedGeometry::OnDestroy();
    }

    virtual void OnNewProgram()
    {
        ProgramState().SetUniform("Texture", myTexture);
        FixedGeometry::OnNewProgram();
    }
    public:
    virtual ~Hextile() { }

    void SetTexture(AssetRef<Texture> texture)
    {
        myTextureRef = texture;
    }
};

#endif
