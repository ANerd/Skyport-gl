#ifndef STATUSBOX_H_
#define STATUSBOX_H_

#include "entity/GUISprite.h"
#include "GameState.h"

namespace anengine
{
    class Statusbox : public GUISprite
    {
        Asset<Texture> myTexture;
        void UpdateStatus();
        protected:
        virtual void OnPropertyChanged(const PropertyInfo *id, bool implicit);
        virtual void OnCreate();
        virtual void OnDestroy();
        public:
        static PropertyInfo ColorProperty;
        Property<ColorRGBA> Color;
        static PropertyInfo StateProperty;
        Property<GameState> State;

        Statusbox() 
            : GUISprite(AssetRef<Texture>()),
            Color(&ColorProperty, this),
            State(&StateProperty, this)
        { }
        virtual ~Statusbox() { }
    };
};

#endif
