

#include <iostream>
#include "backend/sdl/SDLEventSource.h"
#include "backend/sdl/SDLContext.h"
#include "event/EventPrinter.h"
#include "event/EventHub.h"
#include "core/Dispatcher.h"
#include "core/Debug.h"
#include "core/Error.h"
#include "assets/Texture.h"
#include "assets/Buffer.h"
#include "assets/Keymap.h"
#include "entity/SceneGraph.h"
#include "entity/Sprite.h"
#include "entity/Camera.h"
#include "entity/Marker.h"
#include "entity/PointVisualizer.h"
#include "entity/Billboard.h"
#include "filter/KeymapFilter.h"
#include "Hexmap.h"
#include "Skybox.h"
#include "Textbox.h"
#include "NetworkService.h"
#include "GameStateService.h"
#include "SoundManager.h"

using namespace anengine;
int main(int argc, const char *argv[])
{
    std::string host;
    std::string port;
    bool fullscreen = false;
    if(argc == 3 && argv[1][0] != '-')
    {
        host = argv[1];
        port = argv[2];
    }
    else if(argc == 4 && argv[1][0] == '-' && argv[1][1] == 'f')
    {
        host = argv[2];
        port = argv[3];
        fullscreen = true;
    }
    else 
    {
        cerr<<"Usage: "<<argv[0]<<" {-f} <hostname> <port>"<<endl;
        return 1;
    }

    Dispatcher dispatcher;
    SDLEventSource source;
    SDLContext context(100, 100, fullscreen);
    EventHub hub(EventClass::Misc);
    EventHub inputHub(EventClass::Input);
    SceneGraph scene;
    KeymapFilter keymapFilter;

    NetworkService ns(host, port);
    dispatcher.AddService(ns);

    AssetRef<Keymap> keymap = scene.GetAssetManager()
        ->CreateFromFile<Keymap>("assets/Navigation.kmp");
    AssetRef<Program> skyShader = scene.GetAssetManager()
        ->CreateFromFile<Program>("assets/shaders/Sky.sp");
    AssetRef<Program> playerShader = scene.GetAssetManager()
        ->CreateFromFile<Program>("assets/shaders/Player.sp");
    AssetRef<Texture> groundTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/tiles.gen.png");
    AssetRef<Texture> emblemTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/emblems.gen.png");
    AssetRef<Texture> figureTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/figure.gen.png");
    AssetRef<Texture> laserTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/laser.gen.png");
    AssetRef<Texture> mortarTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/mortar.gen.png");
    AssetRef<Texture> droidTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/droid.gen.png");
    AssetRef<Texture> skyTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/sky.gen.png");
    AssetRef<Texture> explosionTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/explosion.gen.png");
    AssetRef<Texture> iconTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/icons.gen.png");

    keymapFilter.SetKeymap(keymap);
    hub.CreateInPin("In");
    hub.CreateInPin("GFXIn");
    hub.CreateOutPin("Context");
    hub.CreateOutPin("Dispatcher");
    hub.CreateOutPin("Scene");

    inputHub.CreateInPin("Input");
    inputHub.CreateOutPin("Printer");
    inputHub.CreateOutPin("Input");

    Pin::Connect(context, "Misc", hub, "GFXIn");
    Pin::Connect(source, "Input", inputHub, "Input");
    Pin::Connect(inputHub, "Input", keymapFilter, "Input");
    Pin::Connect(keymapFilter, "Action", scene, "Actions");
    Pin::Connect(source, "Misc", hub, "In");
    Pin::Connect(hub, "Context", context, "Misc");
    Pin::Connect(hub, "Dispatcher", dispatcher, "Quit");
    Pin::Connect(hub, "Scene", scene, "Misc");

    context.DependOn(&source);
    scene.DependOn(&context);
    scene.DependOn(&keymapFilter);

    dispatcher.AddService(context);
    dispatcher.AddService(source);
    dispatcher.AddService(scene);

    MultiContainer c;


    Skybox sky(skyTex);
    sky.SetProgram(skyShader);
    c.AddChild(&sky);

    Movable mapMov;
    mapMov.Transform.Set(MatrixF4::RotationX(3*Pi/2)*MatrixF4::RotationZ(Pi));
    Hexmap map(groundTex, emblemTex);
    mapMov.SetChild(&map);
    c.AddChild(&mapMov);

    Camera cam;
    scene.GetCameraManager().SetCamera(&cam);
    GameStateService gamestate(&c, &map, playerShader, figureTex, laserTex, 
            mortarTex, droidTex, explosionTex, iconTex, &cam);
    dispatcher.AddService(gamestate);

    gamestate.DependOn(&scene);
    ns.DependOn(&gamestate);

    SoundManager sound;
    dispatcher.AddService(sound);

    Pin::Connect(ns, "GameStates", gamestate, "StateUpdates");
    Pin::Connect(gamestate, "Done", ns, "Done");
    Pin::Connect(gamestate, "Sound", sound, "Sound");

    scene.SetRoot(&c);

    dispatcher.Run();

    Debug("Bye!");
    return 0;
}
