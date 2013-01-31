#include "Hextile.h"
#include "assets/AssetManager.h"

StaticAsset<VertexBuffer> Hextile::myVertexBuffer(AssetManager
            ::CreateStaticFromFile<VertexBuffer>("assets/geometry/hextile.gen.vbo"));
