#include "OBJLoader.h"

#include <string>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "../../../../External/tiny_obj_loader/tiny_obj_loader.h"
#include "../Mesh.h"

namespace fdk
{
  namespace Framework 
  {
    namespace Importers 
    {
    
      struct BasicVertexFormat 
      {
        v3 m_pos;
        v3 m_normal;
        v2 m_uvs;
      };

      static constexpr size_t kFaceVertexCount = 3;

      tinyobj::attrib_t g_attrib;
      std::vector<tinyobj::shape_t> g_shapes;
      std::vector<tinyobj::material_t> g_materials;
      std::string g_error;

      void load_obj(
        const char* filename, 
        Memory::MemAllocator& rTmpAllocator,
        MeshList& rOutMeshList)
      {        
        auto result = tinyobj::LoadObj(
          &g_attrib, 
          &g_shapes, 
          &g_materials, 
          &g_error, 
          filename);
        if (!result) 
        {
          printf("ERROR loading obj <%s>: %s\n", g_error.c_str());
        }
        else 
        {
          // Loop over shapes
          for (size_t s = 0; s < g_shapes.size(); s++) {
            // Loop over faces(polygon)
            auto& rShape = g_shapes[s];

            rOutMeshList.push_back(Mesh());
            auto& rMesh = rOutMeshList[rOutMeshList.size() - 1];

            // Allocate chunks of data to store the info of the mesh
            u32 vertexCount = static_cast<u32>(rShape.mesh.num_face_vertices.size());
            f32* pVertexData = reinterpret_cast<f32*>(rTmpAllocator.allocate(sizeof(BasicVertexFormat)*vertexCount, alignof(BasicVertexFormat)));
            u32 indexCount = static_cast<u32>(rShape.mesh.indices.size());
            u16* pIndexData = reinterpret_cast<u16*>(rTmpAllocator.allocate(sizeof(u16)*indexCount, alignof(u16)));

            // Copy the indices to our final format
            for (size_t idx=0; idx<rShape.mesh.indices.size(); ++idx) 
            {
              pIndexData[idx] = static_cast<u16>(rShape.mesh.indices[idx].vertex_index);
            }

            // Copy the vertex data to our final format
            size_t index_offset = 0;
            BasicVertexFormat* pVertexDataIter = reinterpret_cast<BasicVertexFormat*>(pVertexData);
            for (size_t f = 0; f < rShape.mesh.num_face_vertices.size(); f++) {
              size_t fv = rShape.mesh.num_face_vertices[f];
              FDASSERT(fv == kFaceVertexCount, "We use triangulate so this value should always be 3");
              // Loop over vertices in the face.
              for (size_t v = 0; v < kFaceVertexCount; v++) {
                // access to vertex
                tinyobj::index_t idx = rShape.mesh.indices[index_offset + v];
                // Copy vertex position
                pVertexDataIter->m_pos.x = g_attrib.vertices[3 * idx.vertex_index + 0];
                pVertexDataIter->m_pos.y = g_attrib.vertices[3 * idx.vertex_index + 1];
                pVertexDataIter->m_pos.z = g_attrib.vertices[3 * idx.vertex_index + 2];
                // Copy normal
                pVertexDataIter->m_normal.x = g_attrib.normals[3 * idx.normal_index + 0];
                pVertexDataIter->m_normal.y = g_attrib.normals[3 * idx.normal_index + 1];
                pVertexDataIter->m_normal.z = g_attrib.normals[3 * idx.normal_index + 2];
                // Copy UVs
                pVertexDataIter->m_uvs.x = g_attrib.texcoords[2 * idx.texcoord_index + 0];
                pVertexDataIter->m_uvs.y = g_attrib.texcoords[2 * idx.texcoord_index + 1];
                
                pVertexDataIter++;
              }
              index_offset += fv;

              // per-face material
              //rShape.mesh.material_ids[f];
            }
            rMesh.init_data(pVertexData, vertexCount, sizeof(BasicVertexFormat), pIndexData, indexCount, sizeof(u16));
          }
        }
      }

    }
  }
}

