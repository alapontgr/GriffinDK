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

}