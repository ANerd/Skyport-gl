

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
#include "filter/KeymapFilter.h"
#include "Hextile.h"
#include "Textbox.h"

using namespace anengine;

int main(int argc, const char *argv[])
{
    Dispatcher dispatcher;
    SDLEventSource source;
    EventPrinter printer;
    SDLContext context(100, 100, false);
    EventHub hub(EventClass::Misc);
    EventHub inputHub(EventClass::Input);
    SceneGraph scene;
    KeymapFilter keymapFilter;

    AssetRef<Keymap> keymap = scene.GetAssetManager()
        ->CreateFromFile<Keymap>("assets/Navigation.kmp");
    AssetRef<Program> groundShader = scene.GetAssetManager()
        ->CreateFromFile<Program>("assets/shaders/Ground.sp");

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
    Pin::Connect(inputHub, "Printer", printer, "Input");
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
    cam.Near.Set(0.1);
    cam.Far.Set(10);
    cam.FOV.Set(3.14/4);
    Movable camMov;
    MatrixF4 cm;
    cm.CreateTranslation(VectorF4(0, 1, 1));
    camMov.Transform.Set(cm);
    Marker camMarker;
    camMov.SetPointAt(&camMarker);
    c.AddChild(&camMarker);
    PointVisualizer pv;
    pv.Color.Set(ColorF(0,0,1,1));
    pv.Size.Set(0.05f);
    c.AddChild(&pv);

    scene.GetCameraManager().SetCamera(&cam);

    camMov.InstanceId.Set(1);
    camMov.SetChild(&cam);
    c.AddChild(&camMov);

    //Add hextiles
    Marker tileMark;
    Movable tileMarkMov;
    tileMarkMov.InstanceId.Set(2);
    //MatrixF4 m;m.CreateRotationX(90);
    MatrixF4 m;m.CreateTranslation(UnitF4[Z]);
    tileMarkMov.Transform.Set(m);
    tileMarkMov.SetChild(&tileMark);
    c.AddChild(&tileMarkMov);

    Movable tileMov;
    tileMov.SetPointAt(&tileMark);
    Hextile tile;
    tile.SetProgram(groundShader);
    tileMov.SetChild(&tile);
    c.AddChild(&tileMov);

    Textbox text;
    c.AddChild(&text);

    scene.SetRoot(&c);

    dispatcher.Run();

    Debug("Bye!");
    return 0;
}
