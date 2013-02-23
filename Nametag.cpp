
#include "Nametag.h"
#include "textlib/textlib.h"

namespace anengine
{
    PropertyInfo Nametag::PlayerNameProperty("PlayerName", typeid(Nametag));
    PropertyInfo Nametag::HealthProperty("Health", typeid(Nametag));

    void Nametag::OnPropertyChanged(const PropertyInfo *id, bool implicit)
    {
        if(id == &PlayerNameProperty || id == &HealthProperty)
        {
            if(IsCreated())
                UpdateStatus();
        }
        Billboard::OnPropertyChanged(id, implicit);
    }

    void Nametag::OnCreate()
    {
        myTexture = Scene.Get()->GetAssetManager()->CreateFromMemory<Texture>("");
        UpdateStatus();
        SetTexture(myTexture);
        Billboard::OnCreate();
    }

    void Nametag::OnDestroy()
    {
        myTexture.Release();
        Billboard::OnDestroy();
    }

    void Nametag::OnNewProgram()
    {
        ProgramState().SetUniform("Flip", VectorF2(0,1));
        Billboard::OnNewProgram();
    }

    void Nametag::UpdateStatus()
    {
        SDL_Surface *stat = textlib_get_nametag(PlayerName.Get().c_str(), Health.Get());
        SDL_LockSurface(stat);
        myTexture->SetData(stat->w, stat->h, GL_BGRA, stat->pixels);
        SDL_UnlockSurface(stat);
        SDL_FreeSurface(stat);
    }
};
