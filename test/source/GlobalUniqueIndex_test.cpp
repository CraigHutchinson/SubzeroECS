#include "SubzeroECS/GlobalUniqueIndex.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test {

TEST(GlobalUniqueIndexTest, FirstIsZero)
{
	GlobalUniqueIndex id;
	EXPECT_EQ(static_cast<uint32_t>(id), 0u);
}

TEST(GlobalUniqueIndexTest, OnlyInstanceIsZero)
{
	{
		GlobalUniqueIndex id1;
		EXPECT_EQ(static_cast<uint32_t>(id1), 0u);
	}
	{
		GlobalUniqueIndex id2;
		EXPECT_EQ(static_cast<uint32_t>(id2), 0u);
	}
}

TEST(GlobalUniqueIndexTest, SecondIsOne)
{
	GlobalUniqueIndex id1;
	GlobalUniqueIndex id2;
	EXPECT_EQ(static_cast<uint32_t>(id2), 1u);
}

TEST(GlobalUniqueIndexTest, OnlySecondInstanceIsOne)
{
	GlobalUniqueIndex id1;
	{
		GlobalUniqueIndex id2;
		EXPECT_EQ(static_cast<uint32_t>(id2), 1u);
	}
	{
		GlobalUniqueIndex id2;
		EXPECT_EQ(static_cast<uint32_t>(id2), 1u);
	}
}

} // namespace Test
} // namespace SubzeroECS