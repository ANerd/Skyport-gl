
#include "Laser.h"

StaticAsset<VertexBuffer> Laser::myVertexBuffer(AssetManager::CreateStaticFromMemory<VertexBuffer>(
"A2 position float2 false 16 0 texcoord float2 false 16 8 1 24" 
"0.0 -0.5 0.0 0.0 "
"1.0 -0.5 1.0 0.0 "
"1.0  0.5 1.0 1.0 "
"0.0 -0.5 0.0 0.0 " 
"1.0  0.5 1.0 1.0 "
"0.0  0.5 0.0 1.0 ")); 

StaticAsset<Program> Laser::LaserProgram(AssetManager::CreateStaticFromMemory<Program>(
"##s\n"
"uniform ivec2 FrameCount; \n"
"uniform ivec2 Frame; \n"
"uniform mat4 ViewProjection; \n"
"uniform mat4 World; \n"
"uniform float Length; \n"
"attribute vec2 position; \n"
"attribute vec2 texcoord; \n"
"varying vec2 vtexcoord; \n"
"void main(void) \n"
"{ \n"
"  vec2 pos = position; \n"
"  vec2 tex = texcoord; \n"
"  if(pos.x < 0.1) \n"
"  { \n"
"    tex.x = 1.0 - (Length / float(FrameCount.x)); \n"
"  } \n"
"  else if(pos.x > 0.9) \n"
"  { \n"
"    pos.x = Length; \n"
"  } \n"
"  vec2 frameSize = vec2(1.0,1.0/float(FrameCount.y));\n"
"  vtexcoord = vec2((tex.x+float(Frame.x)) * frameSize.x,(tex.y+float(Frame.y)) * frameSize.y); \n"
"  gl_Position = ViewProjection * (World * vec4(pos.x, pos.y, 0.0, 1.0)); \n"
"} \n"
"##s\n"
"uniform sampler2D Texture; \n"
"varying vec2 vtexcoord; \n"
"void main(void) \n"
"{ \n"
"  gl_FragColor = texture2D(Texture, vtexcoord); \n"
"} \n"
"##\n"
"Global:ViewProjection;"
"World:World;"
"Default:FrameCount=1 1;"
"Default:Frame=0 0;"
"Default:Length=0;"));

PropertyInfo Laser::LengthProperty("Length", typeid(Laser));

void Laser::OnPropertyChanged(const PropertyInfo *info, bool implicit)
{
    if(info == &LengthProperty)
    {
        if(HasProgram())
        {
            ProgramState().SetUniform("Length", Length.Get());
        }
    }
    FixedGeometry::OnPropertyChanged(info, implicit);
}

void Laser::OnCreate()
{
    if(!myTextureRef.Empty())
        myTexture = myTextureRef;

    //Debug("Set my program");
    FixedGeometry::OnCreate();
    if(!HasProgram())
        SetProgram(LaserProgram.Get(Scene.Get()->GetAssetManager()));
}

void Laser::OnDestroy()
{
    FixedGeometry::OnDestroy();
    myTexture.Release();
}

void Laser::OnNewProgram()
{
    if(!myTexture.Empty())
        ProgramState().SetUniform("Texture", myTexture);
    ProgramState().SetUniform("FrameCount", VectorI2(8,16));
    ProgramState().SetUniform("Length", Length.Get());
    FixedGeometry::OnNewProgram();
}

void Laser::OnDraw(FrameTime elapsed)
{
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void LaserAnimationData::Update(real t)
{
    Animated->Length.Set(t*Length);
}
