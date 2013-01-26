
#include "Textbox.h"
#include "textlib/textlib.h"

namespace anengine
{
    StaticAsset<Program> Textbox::TextboxProgram = AssetManager
        ::CreateStaticFromMemory<Program>(
            "##s\n"
            "uniform mat4 ViewProjection; \n"
            "uniform mat4 World; \n"
            "attribute vec2 position; \n"
            "attribute vec2 texcoord; \n"
            "varying vec2 vtexcoord; \n"
            "void main(void) \n"
            "{ \n"
            "  vtexcoord = texcoord; \n"
            "  gl_Position = ViewProjection * (World * vec4(position.x, position.y, 0.0, 1.0)); \n"
            "} \n"
            "##s\n"
            "uniform sampler2D Texture; \n"
            "varying vec2 vtexcoord; \n"
            "void main(void) \n"
            "{ \n"
            "  vec4 col = texture2D(Texture, vtexcoord); \n"
            "  gl_FragColor = vec4(col.y,col.z,col.a,col.x); \n"
            "} \n"
            "##\n"
            "Global:ViewProjection;"
            "World:World;");

    void Textbox::OnCreate()
    {
        myTexture = Scene.Get()->GetAssetManager()->CreateFromMemory<Texture>("");
        SetText("Foobar");
        Sprite::OnCreate();
        SetProgram(TextboxProgram.Get(Scene.Get()->GetAssetManager()));
    }

    void Textbox::OnDestroy()
    {
        myTexture.Release();
        Sprite::OnDestroy();
    }

    void Textbox::SetText(std::string str)
    {
        textlib_initialize();
        textlib_set_font_size(72);
        textlib_set_background_color(0, 0, 0);
        textlib_set_quality(2);
        SDL_Surface *text = textlib_get_text((char*)str.c_str());
        myTexture->SetData(text->w, text->h, GL_RGBA, text->pixels);
    }
};
