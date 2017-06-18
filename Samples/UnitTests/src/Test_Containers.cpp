#include "UnitTests.h"

#include "Containers/Mask.h"
#include "Utilities/types.h"

using namespace fdk;

TEST_CASE("Test Mask", "[Containers]")
{
	Containers::Mask<u32, 0> mask;

	REQUIRE(mask.flags() == 0);

	mask.set(4);
	REQUIRE(mask.flags() == 4);

	mask.set(2, true);

	REQUIRE(mask.flags() == 6);

	mask.reset();

	REQUIRE(mask.flags() == 0);

	mask.set(2);

	REQUIRE(mask.flags() == 2);

	REQUIRE(!mask.is_enable(4));
	REQUIRE(mask.is_enable(2));

	mask.clear();

	REQUIRE(mask.flags() == 0);

	mask.toggle(2);
	REQUIRE(mask.flags() == 2);
	mask.toggle(2);
	REQUIRE(mask.flags() == 0);

	mask.set(4);

	Containers::Mask<u32, 0> maskB;
	maskB.set(2);

	mask |= maskB;
	REQUIRE(mask.flags() == 6);

	maskB.set(4);
	mask &= maskB;
	REQUIRE(mask.flags() == 4);

	mask.set(4);
	maskB.set(3);

	REQUIRE((mask | maskB).flags() == 7);

  mask.set(6);
  maskB.set(4);
	REQUIRE((mask & maskB).flags() == 4);

	mask.clear();
	REQUIRE((~mask).flags() == ~0U);

	mask.set(6);
	maskB.set(7);
	REQUIRE((mask ^ maskB).flags() == 1);

	REQUIRE(!(mask == maskB));
	REQUIRE(mask != maskB);
}