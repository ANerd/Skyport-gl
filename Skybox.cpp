#include "Skybox.h"
#include "assets/AssetManager.h"

StaticAsset<VertexBuffer> Skybox::myVertexBuffer(AssetManager
            ::CreateStaticFromFile<VertexBuffer>("assets/geometry/skybox.gen.vbo"));
