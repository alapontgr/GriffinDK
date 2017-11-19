#include "OBJLoader.h"

#include <string>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "../../../../External/tiny_obj_loader/tiny_obj_loader.h"

namespace fdk
{
  namespace Framework 
  {
    namespace Importers 
    {
    
      tinyobj::attrib_t g_attrib;
      std::vector<tinyobj::shape_t> g_shapes;
      std::vector<tinyobj::material_t> g_materials;
      std::string g_error;

      void load_obj(const char* filename)
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
          // TODO
        }
      }

    }
  }
}

