#pragma once
#include <vector>
#include "Memory\MemAllocator.h"

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

      void load_obj(const char* filename, Memory::MemAllocator& rTmpAllocator, MeshList& rOutMeshList);
    }
  }
}