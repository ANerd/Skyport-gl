
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
            "  vtexcoord = vec2(texcoord.x, 1.0-texcoord.y); \n"
            "  gl_Position = ViewProjection * (World * vec4(position.x, position.y, 0.0, 1.0)); \n"
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
            "World:World;");

    
    PropertyInfo Textbox::ColorProperty("Color", typeid(Textbox));
    PropertyInfo Textbox::TextProperty("Text", typeid(Textbox));

    Textbox::TextlibInit Textbox::myTexlibInit;

    Textbox::TextlibInit::TextlibInit()
    {
        Debug("Textlib init");
        textlib_initialize();
    }
    Textbox::TextlibInit::~TextlibInit()
    {
        Debug("Textlib quit");
        textlib_quit();
    }

    void Textbox::OnPropertyChanged(const PropertyInfo *id, bool implicit)
    {
        if(id == &TextProperty || id == &ColorProperty)
        {
            if(IsCreated())
                UpdateText();
        }
        Sprite::OnPropertyChanged(id, implicit);
    }

    void Textbox::OnCreate()
    {
        myTexture = Scene.Get()->GetAssetManager()->CreateFromMemory<Texture>("");
        UpdateText();
        Sprite::OnCreate();
        SetProgram(TextboxProgram.Get(Scene.Get()->GetAssetManager()));
    }

    void Textbox::OnDestroy()
    {
        myTexture.Release();
        Sprite::OnDestroy();
    }

    void Textbox::UpdateText()
    {
        textlib_initialize();
        textlib_set_font(72, NULL);
        textlib_set_quality(TEXT_QUALITY_HIGH);
        ColorRGBA c = Color.Get();
        SDL_Surface *text = textlib_get_text(Text.Get().c_str(), c[R],c[G],c[B]);
        myTexture->SetData(text->w, text->h, GL_BGRA, text->pixels);
        SDL_FreeSurface(text);
    }
};
