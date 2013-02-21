#include "Hexborder.h"
#include "assets/AssetManager.h"

StaticAsset<VertexBuffer> Hexborder::myVertexBuffer(AssetManager
            ::CreateStaticFromFile<VertexBuffer>("assets/geometry/hexborder.gen.vbo"));
