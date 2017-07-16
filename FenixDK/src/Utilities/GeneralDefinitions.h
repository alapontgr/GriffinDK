#pragma once
#include "types.h"

namespace fdk
{

enum EResultType : u32
{
	kNoError = 0,
	kUnknownError,
  kInvalidLoadedFile
};

enum EVertexParameterType : u32 
{
  Float,
  Float2,
  Float3,
  Float4
};

}