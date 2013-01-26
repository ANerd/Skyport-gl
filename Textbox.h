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
        protected:
        void OnCreate();
        void OnDestroy();
        public:
        
        Textbox() 
            : Sprite(AssetRef<Texture>()) { }
        virtual ~Textbox() { }

        void SetText(std::string str);
    };
};

#endif
