#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "entity/Sprite.h"

namespace anengine
{
    class Textbox : public Sprite
    {
        static StaticAsset<Program> TextboxProgram;
        struct TextlibInit
        {
            TextlibInit();
            ~TextlibInit();
        };
        static TextlibInit myTexlibInit;
        Asset<Texture> myTexture;
        void UpdateText();
        protected:
        virtual void OnPropertyChanged(const PropertyInfo *id, bool implicit);
        virtual void OnCreate();
        virtual void OnDestroy();
        public:
        static PropertyInfo ColorProperty;
        Property<ColorRGBA> Color;
        static PropertyInfo TextProperty;
        Property<std::string> Text;
        Textbox() 
            : Sprite(AssetRef<Texture>()),
            Color(&ColorProperty, this),
            Text(&TextProperty, this, EventDirection::None, "F")
        { }
        virtual ~Textbox() { }
    };
};

#endif
