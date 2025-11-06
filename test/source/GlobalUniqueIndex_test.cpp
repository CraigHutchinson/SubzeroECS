#include "SubzeroECS/UniqueIndex32.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test {

TEST(UniqueIndex32Test, FirstIsZero)
{
	UniqueIndex32 id;
	EXPECT_EQ(static_cast<uint32_t>(id), 0u);
}

TEST(UniqueIndex32Test, OnlyInstanceIsZero)
{
	{
		UniqueIndex32 id1;
		EXPECT_EQ(static_cast<uint32_t>(id1), 0u);
	}
	{
		UniqueIndex32 id2;
		EXPECT_EQ(static_cast<uint32_t>(id2), 0u);
	}
}

TEST(UniqueIndex32Test, SecondIsOne)
{
	UniqueIndex32 id1;
	UniqueIndex32 id2;
	EXPECT_EQ(static_cast<uint32_t>(id2), 1u);
}

TEST(UniqueIndex32Test, OnlySecondInstanceIsOne)
{
	UniqueIndex32 id1;
	{
		UniqueIndex32 id2;
		EXPECT_EQ(static_cast<uint32_t>(id2), 1u);
	}
	{
		UniqueIndex32 id2;
		EXPECT_EQ(static_cast<uint32_t>(id2), 1u);
	}
}

} // namespace Test
} // namespace SubzeroECS