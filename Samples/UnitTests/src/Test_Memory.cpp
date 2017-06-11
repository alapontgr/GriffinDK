#include "UnitTests.h"

#include "Utilities/types.h"
#include "Memory/MemAllocator.h" 

#include "Memory/MemUtils.h"
#include "Memory/MallocAllocator.h"
#include "Memory/StackAllocator.h"

using namespace fdk;
using namespace fdk::Memory;

struct A 
{
  char a; //0
  // padd 3
  f32 b; //4
  char c; //8
  // padd 1
  s16 e; //10
};

TEST_CASE("Test memory helper functions", "[Memory]") 
{
  mem_ptr_t pData = allocate_memory(32, 16);

  REQUIRE(is_aligned(&pData[4], 4));
  REQUIRE(!is_aligned(&pData[3], 4));
  REQUIRE(is_aligned(&pData[0], 16));
  REQUIRE(!is_aligned(&pData[14], 16));

  REQUIRE(get_alignment_offset(&pData[1], 16) == 15);
  REQUIRE(get_alignment_offset(&pData[1], 4) == 3);
  REQUIRE(get_alignment_offset(&pData[0], 4) == 0);
  
  REQUIRE(mem_distance(&pData[0], &pData[16]) == 16);

  release_memory(pData);
}

TEST_CASE("MallocMemAllocator", "[Memory], [Allocators]") 
{
  MallocAllocator allocator;
  MemBlock block;
  block.allocate(allocator, 32, 16);
  
  REQUIRE(is_aligned(block.memory(), 16));
  REQUIRE(block.size() == 32);

  block.release(allocator);

  REQUIRE(block.memory() == nullptr);
  REQUIRE(block.size() == 0);
}

TEST_CASE("StackAllocator", "[Memory], [Allocators]") 
{
  MallocAllocator mallocAlloc;
  StackAllocator stackAllocator;
  stackAllocator.init(mallocAlloc, 64, 16);
  REQUIRE(stackAllocator.capacity() == 64);
  REQUIRE(stackAllocator.size() == 0);

  REQUIRE(stackAllocator.space_left() == 64);

  mem_ptr_t pData = stackAllocator.allocate(5, 16);
  REQUIRE(is_aligned(pData, 16));
  
  REQUIRE(stackAllocator.size() == 5);
  REQUIRE(stackAllocator.space_left() == (64 - 5));

  A* pA = stackAllocator.create<A>();
  REQUIRE(is_aligned(reinterpret_cast<mem_ptr_t>(pA), alignof(A)));

  stackAllocator.reset();

  REQUIRE(stackAllocator.space_left() == 64);

  pData = stackAllocator.allocate(5, 16);
  REQUIRE(is_aligned(pData, 16));

  REQUIRE(stackAllocator.space_left() == (64 - 5));

  pA = stackAllocator.create<A>();
  REQUIRE(is_aligned(reinterpret_cast<mem_ptr_t>(pA), alignof(A)));

  stackAllocator.free_memory(mallocAlloc);
}