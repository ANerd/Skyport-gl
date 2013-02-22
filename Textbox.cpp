
#include "Textbox.h"
#include "textlib/textlib.h"

namespace anengine
{
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
        GUISprite::OnPropertyChanged(id, implicit);
    }

    void Textbox::OnCreate()
    {
        myTexture = Scene.Get()->GetAssetManager()->CreateFromMemory<Texture>("");
        UpdateText();
        SetTexture(myTexture);
        GUISprite::OnCreate();
    }

    void Textbox::OnDestroy()
    {
        myTexture.Release();
        GUISprite::OnDestroy();
    }

    void Textbox::UpdateText()
    {
        textlib_initialize();
        textlib_set_font(72, NULL);
        textlib_set_quality(TEXT_QUALITY_HIGH);
        ColorRGBA c = Color.Get();
        SDL_Surface *text = textlib_get_text(Text.Get().c_str(), c[R],c[G],c[B]);
        myTexture->SetData(text->w, text->h, GL_BGRA, text->pixels);
        real height = Size.Get()[Height];
        Size.Set(SizeF2(text->w * height / text->h, height));
        SDL_FreeSurface(text);
    }
};
