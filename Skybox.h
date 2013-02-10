#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "entity/FixedGeometry.h"
#include "assets/AssetManager.h"

using namespace anengine;

class Skybox : public FixedGeometry
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
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
    Skybox(AssetRef<Texture> tex)
        : myTextureRef(tex) { }
    virtual ~Skybox() { }

    void SetTexture(AssetRef<Texture> texture)
    {
        myTextureRef = texture;
    }
};

#endif
