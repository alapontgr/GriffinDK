#include "Name.h"

#include "../External/smhasher/src/MurmurHash3.h"
#include <string.h>

namespace fdk
{
  namespace Utilities 
  {
    hash_t get_length(const char* message)
    {
      return strlen(message);
    }

    hash_t get_hash_32(const char* message, const u32 seed)
    {
      hash_t hash = 0;
      MurmurHash3_x86_32(message, get_length(message), seed, &hash);
      return hash;
    }

    hash_t get_hash_32(const char* message, u32 length, const u32 seed)
    {
      hash_t hash = 0;
      MurmurHash3_x86_32(message, length, seed, &hash);
      return hash;
    }

    Name::Name(const char* message)
    {
      u32 len = get_length(message);
      m_hash = get_hash_32(message, len, kSeed);
    }
  }
}