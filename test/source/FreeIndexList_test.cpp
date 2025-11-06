#include "SubzeroECS/FreeIndexList32.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test {

TEST(FreeIndexList32Test, InitEmpty)
{
	FreeIndexList32 list;
	EXPECT_TRUE(list.isEmpty());
}

TEST(FreeIndexList32Test, InitNotFull)
{
	FreeIndexList32 list;
	EXPECT_FALSE(list.isFull());
}

TEST(FreeIndexList32Test, AllocOneIsZero)
{
	FreeIndexList32 list;
	EXPECT_EQ(list.alloc(), 0u);
}

TEST(FreeIndexList32Test, AllocManyIsSequential)
{
	FreeIndexList32 list;
	for (FreeIndexList32::Index i = 0; i < 32; ++i) {
		EXPECT_EQ(list.alloc(), i);
	}
}

TEST(FreeIndexList32Test, AllocOneNotEmpty)
{
	FreeIndexList32 list;
	list.alloc();
	EXPECT_FALSE(list.isEmpty());
}

TEST(FreeIndexList32Test, AllocOneNotFull)
{
	FreeIndexList32 list;
	list.alloc();
	EXPECT_FALSE(list.isFull());
}

TEST(FreeIndexList32Test, AllocAllIsFull)
{
	FreeIndexList32 list;
	for (FreeIndexList32::Index i = 0; i < FreeIndexList32::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_TRUE(list.isFull());
}

TEST(FreeIndexList32Test, AllocAllNotEmpty)
{
	FreeIndexList32 list;
	for (FreeIndexList32::Index i = 0; i < FreeIndexList32::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_FALSE(list.isEmpty());
}

TEST(FreeIndexList32Test, AllocBeyondCapacityThrows)
{
	FreeIndexList32 list;
	for (FreeIndexList32::Index i = 0; i < FreeIndexList32::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_THROW(list.alloc(), FreeIndexList32::AllocFailed);
}

TEST(FreeIndexList32Test, FreeAndReallocSameIndex)
{
	FreeIndexList32 list;
	auto index = list.alloc();
	list.free(index);
	EXPECT_EQ(list.alloc(), index);
}

TEST(FreeIndexList32Test, FreeReturnsToEmpty)
{
	FreeIndexList32 list;
	auto index = list.alloc();
	list.free(index);
	EXPECT_TRUE(list.isEmpty());
}

TEST(FreeIndexList32Test, FreeAllReturnsToEmpty)
{
	FreeIndexList32 list;
	FreeIndexList32::Index indices[FreeIndexList32::Capacity];
	
	for (size_t i = 0; i < FreeIndexList32::Capacity; ++i) {
		indices[i] = list.alloc();
	}
	
	for (size_t i = 0; i < FreeIndexList32::Capacity; ++i) {
		list.free(indices[i]);
	}
	
	EXPECT_TRUE(list.isEmpty());
}

TEST(FreeIndexList32Test, CountInitiallyZero)
{
	FreeIndexList32 list;
	EXPECT_EQ(list.count(), 0u);
}

TEST(FreeIndexList32Test, CountAfterOneAlloc)
{
	FreeIndexList32 list;
	list.alloc();
	EXPECT_EQ(list.count(), 1u);
}

TEST(FreeIndexList32Test, CountAfterMultipleAlloc)
{
	FreeIndexList32 list;
	for (size_t i = 0; i < 5; ++i) {
		list.alloc();
	}
	EXPECT_EQ(list.count(), 5u);
}

TEST(FreeIndexList32Test, CountAfterAllocAndFree)
{
	FreeIndexList32 list;
	auto index = list.alloc();
	list.alloc();
	list.alloc();
	EXPECT_EQ(list.count(), 3u);
	
	list.free(index);
	EXPECT_EQ(list.count(), 2u);
}

TEST(FreeIndexList32Test, CountAtCapacity)
{
	FreeIndexList32 list;
	for (size_t i = 0; i < FreeIndexList32::Capacity; ++i) {
		list.alloc();
	}
	EXPECT_EQ(list.count(), FreeIndexList32::Capacity);
}

TEST(FreeIndexList32Test, FreeMiddleIndexAndRealloc)
{
	FreeIndexList32 list;
	list.alloc(); // 0
	list.alloc(); // 1
	auto middle = list.alloc(); // 2
	list.alloc(); // 3
	list.alloc(); // 4
	
	list.free(middle);
	EXPECT_EQ(list.alloc(), middle);
}

TEST(FreeIndexList32Test, FreeMultipleNonSequential)
{
	FreeIndexList32 list;
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

TEST(FreeIndexList32Test, AllocFreeAllocPattern)
{
	FreeIndexList32 list;
	
	for (size_t i = 0; i < 10; ++i) {
		auto idx = list.alloc();
		EXPECT_EQ(idx, 0u);
		list.free(idx);
	}
	
	EXPECT_TRUE(list.isEmpty());
}

} // namespace Test
} // namespace SubzeroECS