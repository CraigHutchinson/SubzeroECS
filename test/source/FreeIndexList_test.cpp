#include "SubzeroECS/FreeIndexList.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test {

TEST(FreeIndexListTest, InitEmpty)
{
	FreeIndexList list;
	EXPECT_TRUE(list.isEmpty());
}

TEST(FreeIndexListTest, InitNotFull)
{
	FreeIndexList list;
	EXPECT_FALSE(list.isFull());
}

TEST(FreeIndexListTest, AllocOneIsZero)
{
	FreeIndexList list;
	EXPECT_EQ(list.alloc(), 0u);
}

TEST(FreeIndexListTest, AllocManyIsSequential)
{
	FreeIndexList list;
	for (FreeIndexList::Index i = 0; i < 32; ++i) {
		EXPECT_EQ(list.alloc(), i);
	}
}

TEST(FreeIndexListTest, AllocOneNotEmpty)
{
	FreeIndexList list;
	list.alloc();
	EXPECT_FALSE(list.isEmpty());
}

TEST(FreeIndexListTest, AllocOneNotFull)
{
	FreeIndexList list;
	list.alloc();
	EXPECT_FALSE(list.isFull());
}

} // namespace Test
} // namespace SubzeroECS