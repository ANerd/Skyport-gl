#include "Meteor.h"
#include <cstdlib>

StaticAsset<Program> Meteor::MeteorProgram = AssetManager
    ::CreateStaticFromMemory<Program>(
        "##s\n"
        "uniform ivec2 FrameCount; \n"
        "uniform ivec2 Frame; \n"
        "uniform mat4 View; \n"
        "uniform mat4 World; \n"
        "uniform float Z; \n"
        "uniform vec2 Size; \n"
        "attribute vec2 position; \n"
        "attribute vec2 texcoord; \n"
        "varying vec2 vtexcoord; \n"
        "void main(void) \n"
        "{ \n"
        "  mat4 wv = View * World; \n"
        "  vec2 frameSize = vec2(1.0/float(FrameCount.x),1.0/float(FrameCount.y));\n"
        "  vec2 ftexcoord = texcoord; \n"
        "  vtexcoord = vec2((ftexcoord.x+float(Frame.x)) * frameSize.x,(ftexcoord.y+float(Frame.y)) * frameSize.y); \n"
        "  vec4 vpos = World * vec4(position*Size,0.0,0.0); \n"
        "  vec2 pos = vpos.xy+(View * vec4((World * vec4(0.0,0.0,-1.0,1.0)).xyz, 0.0)).xy; \n"
        "  gl_Position = vec4(pos.xy,Z,1.0); \n"
        "} \n"
        "##s\n"
        "uniform sampler2D Texture; \n"
        "varying vec2 vtexcoord; \n"
        "void main(void) \n"
        "{ \n"
        "  gl_FragColor = texture2D(Texture, vtexcoord); \n"
        "} \n"
        "##\n"
        "Global:View;"
        "World:World;"
        "Default:FrameCount=1 1;"
        "Default:Size=0.1 0.40;"
        "Default:Z=0.98;");
StaticAsset<Texture> Meteor::MeteorTexture = AssetManager
    ::CreateStaticFromFile<Texture>("assets/textures/meteor.gen.png");


void Meteor::OnCreate()
{
    SetTexture(MeteorTexture.Get(Scene.Get()->GetAssetManager()));
    if(!HasProgram())
        SetProgram(MeteorProgram.Get(Scene.Get()->GetAssetManager()));
    Sprite::OnCreate();
}

void Meteor::OnNewProgram()
{
    ProgramState().SetUniform("FrameCount", VectorI2(16,1));
    Sprite::OnNewProgram();
}

void MeteorAnimationData::Update(real t)
{
    VectorF4 pos;
    MatrixF4 trans = Animated->Transform.Get();
    trans.GetTranslation(pos);
    pos[Y] = StartHeight + t*(EndHeight-StartHeight);
    pos[X] = StartX+XSpeed*t;
    trans.SetTranslation(pos);
    Animated->Transform.Set(trans);
}

void MeteorAnimationData::Finalize()
{
    StartX = 5 * (double(std::rand()) / double(RAND_MAX) - 0.5)+1;
    VectorF4 pos;
    MatrixF4 trans = Animated->Transform.Get();
    trans.GetTranslation(pos);
    pos[Y] = StartHeight;
    pos[X] = StartX;
    trans.SetTranslation(pos);
    Animated->Transform.Set(trans);
}
