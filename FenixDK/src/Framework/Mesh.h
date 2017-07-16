#pragma once
#include "Rendering\Buffer.h"

namespace fdk
{
  namespace Framework 
  {
    class Mesh 
    {
    public:

      Mesh();

      void load_mesh(const char* pMeshPath);

      Rendering::Buffer* vertex_buffer() { return &m_vertexBuffer; }

      Rendering::Buffer* index_buffer() { return &m_vertexBuffer; }

    private:
    
      Rendering::Buffer m_vertexBuffer;
      Rendering::Buffer m_indexBuffer;
    };
  }
}