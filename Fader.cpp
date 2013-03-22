
#include "Fader.h"

StaticAsset<Program> Fader::FaderProgram(AssetManager::CreateStaticFromMemory<Program>(
"##s\n"
"attribute vec2 position; \n"
"attribute vec2 texcoord; \n"
"varying vec2 vtexcoord; \n"
"void main(void) \n"
"{ \n"
"  vtexcoord = texcoord; \n"
"  gl_Position = vec4((position * 2.0).xy, -0.999, 1.0); \n"
"} \n"
"##s\n"
"uniform float Fading; \n"
"varying vec2 vtexcoord; \n"
"void main(void) \n"
"{ \n"
"  gl_FragColor = vec4(0.0,0.0,0.0,Fading); \n"
"} \n"
"##\n"
"Default:Fading=0.0;"));

PropertyInfo Fader::FadingProperty("Fading", typeid(Fader));

void Fader::OnPropertyChanged(const PropertyInfo *info, bool implicit)
{
    if(info == &FadingProperty)
    {
        if(HasProgram())
        {
            ProgramState().SetUniform("Fading", Fading.Get());
        }
    }
    FixedGeometry::OnPropertyChanged(info, implicit);
}

void Fader::OnCreate()
{
    if(!HasProgram())
        SetProgram(FaderProgram.Get(Scene.Get()->GetAssetManager()));
    Sprite::OnCreate();
}

void Fader::OnNewProgram()
{
    ProgramState().SetUniform("Fading", Fading.Get());
    Sprite::OnNewProgram();
}

void Fader::OnDraw(FrameTime elapsed)
{
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FaderAnimationData::Update(real t)
{
    if(Reverse)
        Animated->Fading.Set(1-t);
    else
        Animated->Fading.Set(t);
}
