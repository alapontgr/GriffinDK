#pragma once
#include "types.h"

namespace fdk
{
  namespace Utilities 
  {
    using hash_t = u32;

    class Name 
    {
    public:

      enum : u32
      {
        kSeed = 0x9e90b23du,
        kInvalidHash = 0xFFFFffffu
      };

      Name(hash_t hash = kInvalidHash) : m_hash{ hash } {}

      explicit Name(const char* message);

      hash_t get_hash() const { return m_hash; }

      void set_invalid() { m_hash = kInvalidHash; }

      bool is_valid() const { return m_hash != kInvalidHash; }

      bool operator==(const Name& name) const { return m_hash == name.m_hash; }
      
      bool operator!=(const Name& name) const { return m_hash != name.m_hash; }
      
      bool operator<=(const Name& name) const { return m_hash <= name.m_hash; }
      
      bool operator>=(const Name& name) const { return m_hash >= name.m_hash; }
      
      bool operator<(const Name& name) const { return m_hash < name.m_hash; }
      
      bool operator>(const Name& name) const { return m_hash > name.m_hash; }

    private:
      hash_t m_hash = kInvalidHash; // u32
    };

    u32 get_length(const char* message);

    hash_t get_hash_32(const char* message, const u32 seed);

    hash_t get_hash_32(const char* message, u32 length, const u32 seed);
  }
}