

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
#include "Textbox.h"
#include "NetworkService.h"
#include "GameStateService.h"

using namespace anengine;

int main(int argc, const char *argv[])
{
    std::string host;
    std::string port;
    if(argc == 3)
    {
        host = argv[1];
        port = argv[2];
    }
    else if(argc != 1)
    {
        cerr<<"Usage: "<<argv[0]<<" <hostname> <port>"<<endl;
        return 1;
    }

    Dispatcher dispatcher;
    SDLEventSource source;
    EventPrinter printer;
    SDLContext context(100, 100, false);
    EventHub hub(EventClass::Misc);
    EventHub inputHub(EventClass::Input);
    SceneGraph scene;
    KeymapFilter keymapFilter;

    NetworkService ns(host, port);
    dispatcher.AddService(ns);

    AssetRef<Keymap> keymap = scene.GetAssetManager()
        ->CreateFromFile<Keymap>("assets/Navigation.kmp");
    AssetRef<Program> groundShader = scene.GetAssetManager()
        ->CreateFromFile<Program>("assets/shaders/Ground.sp");
    AssetRef<Texture> groundTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/tiles.gen.png");
    AssetRef<Texture> emblemTex = scene.GetAssetManager()
        ->CreateFromFile<Texture>("assets/textures/emblems.gen.png");

    keymapFilter.SetKeymap(keymap);
    printer.CreatePin(EventClass::Input, "Input");
    hub.CreateInPin("In");
    hub.CreateOutPin("Context");
    //hub.CreateOutPin("Printer");
    hub.CreateOutPin("Dispatcher");
    hub.CreateOutPin("Scene");

    inputHub.CreateInPin("Input");
    inputHub.CreateOutPin("Printer");
    inputHub.CreateOutPin("Input");

    Pin::Connect(source, "Input", inputHub, "Input");
    //Pin::Connect(inputHub, "Printer", printer, "Input");
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
    Camera cam;
    cam.Near.Set(0.5);
    cam.Far.Set(50);
    cam.FOV.Set(3.14/4);
    Movable camMov;
    camMov.Transform.Set(MatrixF4::Translation(VectorF4(0,1,1)));
    Marker camMarker;
    camMov.SetPointAt(&camMarker);
    c.AddChild(&camMarker);
    PointVisualizer center;
    center.Color.Set(ColorF(1,1,1,1));
    center.Size.Set(0.05f);
    c.AddChild(&center);

    Movable topMov;
    topMov.Transform.Set(MatrixF4::Translation(VectorF4(0,0.1,0)));
    PointVisualizer top;
    top.Color.Set(ColorF(0,1,0,1));
    top.Size.Set(0.05f);
    topMov.SetChild(&top);
    c.AddChild(&topMov);

    Movable rightMov;
    rightMov.Transform.Set(MatrixF4::Translation(VectorF4(0.1,0,0)));
    PointVisualizer right;
    right.Color.Set(ColorF(1,0,0,1));
    right.Size.Set(0.05f);
    rightMov.SetChild(&right);
    c.AddChild(&rightMov);

    Movable frontMov;
    frontMov.Transform.Set(MatrixF4::Translation(VectorF4(0,0,0.1)));
    PointVisualizer front;
    front.Color.Set(ColorF(0,0,1,1));
    front.Size.Set(0.05f);
    frontMov.SetChild(&front);
    c.AddChild(&frontMov);

    scene.GetCameraManager().SetCamera(&cam);

    camMov.InstanceId.Set(1);
    camMov.SetChild(&cam);
    c.AddChild(&camMov);

    //Add hextiles
    //Marker mapMark;
    //PointVisualizer markV;
    //markV.Color.Set(ColorF(1,0,1,1));
    //markV.Size.Set(0.05f);
    //Movable mapMarkMov;
    //mapMarkMov.InstanceId.Set(2);
    //mapMarkMov.Transform.Set(MatrixF4::Translation(UnitF4[Z]));
    //MultiContainer markC;
    //markC.AddChild(&markV);
    //markC.AddChild(&mapMark);
    //mapMarkMov.SetChild(&markC);
    //c.AddChild(&mapMarkMov);

    Movable mapMov;
    mapMov.Transform.Set(MatrixF4::RotationX(3*Pi/2));
    //mapMov.SetPointAt(&mapMark);
    Hexmap map(groundShader, groundTex, emblemTex);
    mapMov.SetChild(&map);
    c.AddChild(&mapMov);

    //Billboard b(billboardTex);
    //Movable bMovable;
    //bMovable.Transform.Set(MatrixF4::Translation(VectorF4(-5,0.5,-2)));
    //bMovable.SetChild(&b);
    //c.AddChild(&bMovable);
    //Textbox text;
    //text.Color.Set(ColorRGBA(255,255,0,255));
    //text.Text.Set("Foobar");
    //tileMov.SetChild(&text);
    //c.AddChild(&tileMov);
    //c.AddChild(&text);

    GameStateService gamestate(&c, &map);
    dispatcher.AddService(gamestate);

    Pin::Connect(ns, "GameStates", gamestate, "StateUpdates");
    Pin::Connect(gamestate, "Done", ns, "Done");

    scene.SetRoot(&c);

    dispatcher.Run();

    Debug("Bye!");
    return 0;
}
