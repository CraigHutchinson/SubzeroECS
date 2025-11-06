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

TEST(FreeIndexListTest, AllocAllIsFull)
{
	FreeIndexList list;
	for (FreeIndexList::Index i = 0; i < FreeIndexList::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_TRUE(list.isFull());
}

TEST(FreeIndexListTest, AllocAllNotEmpty)
{
	FreeIndexList list;
	for (FreeIndexList::Index i = 0; i < FreeIndexList::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_FALSE(list.isEmpty());
}

TEST(FreeIndexListTest, AllocBeyondCapacityThrows)
{
	FreeIndexList list;
	for (FreeIndexList::Index i = 0; i < FreeIndexList::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_THROW(list.alloc(), FreeIndexList::AllocFailed);
}

TEST(FreeIndexListTest, FreeAndReallocSameIndex)
{
	FreeIndexList list;
	auto index = list.alloc();
	list.free(index);
	EXPECT_EQ(list.alloc(), index);
}

TEST(FreeIndexListTest, FreeReturnsToEmpty)
{
	FreeIndexList list;
	auto index = list.alloc();
	list.free(index);
	EXPECT_TRUE(list.isEmpty());
}

TEST(FreeIndexListTest, FreeAllReturnsToEmpty)
{
	FreeIndexList list;
	FreeIndexList::Index indices[FreeIndexList::Capacity];
	
	for (size_t i = 0; i < FreeIndexList::Capacity; ++i) {
		indices[i] = list.alloc();
	}
	
	for (size_t i = 0; i < FreeIndexList::Capacity; ++i) {
		list.free(indices[i]);
	}
	
	EXPECT_TRUE(list.isEmpty());
}

TEST(FreeIndexListTest, CountInitiallyZero)
{
	FreeIndexList list;
	EXPECT_EQ(list.count(), 0u);
}

TEST(FreeIndexListTest, CountAfterOneAlloc)
{
	FreeIndexList list;
	list.alloc();
	EXPECT_EQ(list.count(), 1u);
}

TEST(FreeIndexListTest, CountAfterMultipleAlloc)
{
	FreeIndexList list;
	for (size_t i = 0; i < 5; ++i) {
		list.alloc();
	}
	EXPECT_EQ(list.count(), 5u);
}

TEST(FreeIndexListTest, CountAfterAllocAndFree)
{
	FreeIndexList list;
	auto index = list.alloc();
	list.alloc();
	list.alloc();
	EXPECT_EQ(list.count(), 3u);
	
	list.free(index);
	EXPECT_EQ(list.count(), 2u);
}

TEST(FreeIndexListTest, CountAtCapacity)
{
	FreeIndexList list;
	for (size_t i = 0; i < FreeIndexList::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_EQ(list.count(), FreeIndexList::Capacity);
}

TEST(FreeIndexListTest, FreeMiddleIndexAndRealloc)
{
	FreeIndexList list;
	list.alloc(); // 0
	list.alloc(); // 1
	auto middle = list.alloc(); // 2
	list.alloc(); // 3
	list.alloc(); // 4
	
	list.free(middle);
	EXPECT_EQ(list.alloc(), middle);
}

TEST(FreeIndexListTest, FreeMultipleNonSequential)
{
	FreeIndexList list;
	list.alloc(); // idx0
	auto idx1 = list.alloc();
	list.alloc(); // idx2
	auto idx3 = list.alloc();
	list.alloc(); // idx4
	
	list.free(idx1);
	list.free(idx3);
	
	EXPECT_EQ(list.count(), 3u);
	EXPECT_EQ(list.alloc(), idx1);
	EXPECT_EQ(list.alloc(), idx3);
	EXPECT_EQ(list.count(), 5u);
}

TEST(FreeIndexListTest, AllocFreeAllocPattern)
{
	FreeIndexList list;
	
	for (size_t i = 0; i < 10; ++i) {
		auto idx = list.alloc();
		EXPECT_EQ(idx, 0u);
		list.free(idx);
	}
	
	EXPECT_TRUE(list.isEmpty());
}

} // namespace Test
} // namespace SubzeroECS