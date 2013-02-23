#ifndef NAMETAG_H_
#define NAMETAG_H_

#include "entity/Billboard.h"

namespace anengine
{
    class Nametag : public Billboard
    {
        Asset<Texture> myTexture;
        void UpdateStatus();
        protected:
        virtual void OnPropertyChanged(const PropertyInfo *id, bool implicit);
        virtual void OnCreate();
        virtual void OnDestroy();
        virtual void OnNewProgram();
        public:
        static PropertyInfo PlayerNameProperty;
        Property<std::string> PlayerName;
        static PropertyInfo HealthProperty;
        Property<real> Health;

        Nametag() 
            : Billboard(),
            PlayerName(&PlayerNameProperty, this),
            Health(&HealthProperty, this)
        { }
        virtual ~Nametag() { }
    };
};

#endif
