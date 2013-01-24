

#include <iostream>
#include "backend/sdl/SDLEventSource.h"
#include "backend/sdl/SDLContext.h"
#include "event/EventPrinter.h"
#include "event/EventHub.h"
#include "core/Dispatcher.h"
#include "core/Debug.h"
#include "core/Error.h"
#include "assets/AssetManager.h"
#include "assets/Shader.h"
#include "assets/Texture.h"
#include "assets/Buffer.h"
#include "assets/Keymap.h"
#include "entity/SceneGraph.h"
#include "entity/Sprite.h"
#include "entity/Camera.h"
#include "filter/KeymapFilter.h"

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
        .CreateFromFile<Keymap>("assets/Navigation.kmp");

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
    Movable camMov;

    scene.GetCameraManager().SetCamera(&cam);

    camMov.InstanceId.Set(1);
    camMov.SetChild(&cam);
    c.AddChild(&camMov);

    scene.SetRoot(&c);

    dispatcher.Run();

    Debug("Bye!");
}
