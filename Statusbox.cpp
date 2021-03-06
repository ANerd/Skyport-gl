
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
        char const** pweapons = new const char*[s.PlayerCount()];
        char const** sweapons = new const char*[s.PlayerCount()];
        int i = 0;
        for(auto it = s.Players_begin(); it != s.Players_end(); it++)
        {
            names[i] = it->Name.c_str();
            points[i] = it->Score;
            pweapons[i] = GetWeaponName(it->PrimaryWeapon);
            sweapons[i++] = GetWeaponName(it->SecondaryWeapon);
        }
        SDL_Surface *stat = textlib_get_stats(s.PlayerCount(), names, points, 
                pweapons, sweapons, 1920);
        if(stat != NULL)
        {
            myTexture->SetData(stat->w, stat->h, GL_BGRA, stat->pixels);
            real width = Size.Get()[Width];
            Size.Set(SizeF2(width, stat->h * width / stat->w));
            SDL_FreeSurface(stat);
        }
        delete [] names;
        delete [] points;
        delete [] pweapons;
        delete [] sweapons;
    }
};
