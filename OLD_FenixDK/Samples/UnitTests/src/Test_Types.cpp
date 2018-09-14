#include "UnitTests.h"

#include "Utilities/types.h"

using namespace fdk;

TEST_CASE("Test type size", "[types]") 
{
  REQUIRE(sizeof(u8) == 1);
  REQUIRE(sizeof(u8) == sizeof(s8));

  REQUIRE(sizeof(u16) == 2);
  REQUIRE(sizeof(u16) == sizeof(s16));

  REQUIRE(sizeof(u32) == 4);
  REQUIRE(sizeof(u32) == sizeof(s32));

  REQUIRE(sizeof(u64) == 8);
  REQUIRE(sizeof(u64) == sizeof(s64));

  REQUIRE(sizeof(f32) == 4);
  REQUIRE(sizeof(f64) == 8);
}