#pragma once
#include <vector>

namespace fdk
{
  namespace Rendering 
  {
    class Buffer;
  }
  namespace Framework 
  {
    class Mesh;

    namespace Importers 
    {
      using MeshList = std::vector<Mesh>;

      void load_obj(const char* filename, Rendering::Buffer& rVertexBuffer, Rendering::Buffer& rIndexBuffer, MeshList& rOutMeshList);
    }
  }
}