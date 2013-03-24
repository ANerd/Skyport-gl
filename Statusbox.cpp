
#include "Statusbox.h"
#include "textlib/textlib.h"

namespace anengine
{
    PropertyInfo Statusbox::StateProperty("GameState", typeid(Statusbox));

    void Statusbox::OnPropertyChanged(const PropertyInfo *id, bool implicit)
    {
        if(id == &StateProperty)
        {
            if(IsCreated())
                UpdateStatus();
        }
        GUISprite::OnPropertyChanged(id, implicit);
    }

    void Statusbox::OnCreate()
    {
        myTexture = Scene.Get()->GetAssetManager()->CreateFromMemory<Texture>("");
        UpdateStatus();
        SetTexture(myTexture);
        GUISprite::OnCreate();
    }

    void Statusbox::OnDestroy()
    {
        myTexture.Release();
        GUISprite::OnDestroy();
    }

    void Statusbox::UpdateStatus()
    {
        const GameState &s = State.Get();
        char const** names = new const char*[s.PlayerCount()];
        int *points = new int[s.PlayerCount()];
        int i = 0;
        for(auto it = s.Players_begin(); it != s.Players_end(); it++)
        {
            names[i] = it->Name.c_str();
            points[i++] = it->Score;
        }
        SDL_Surface *stat = textlib_get_stats(s.PlayerCount(), names, points, 0, 0, 1080);
        myTexture->SetData(stat->w, stat->h, GL_BGRA, stat->pixels);
        real height = Size.Get()[Height];
        Size.Set(SizeF2(stat->w * height / stat->h, height));
        SDL_FreeSurface(stat);
        delete [] names;
        delete [] points;
    }
};
