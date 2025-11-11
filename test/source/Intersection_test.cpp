#include "SubzeroECS/Intersection.hpp"
#include "SubzeroECS/EntityId.hpp"

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

namespace SubzeroECS {
	namespace Test {

		// Helper function to create sorted vectors of EntityIds
		std::vector<EntityId> makeEntityIds(std::initializer_list<uint32_t> ids)
		{
			std::vector<EntityId> result;
			result.reserve(ids.size());
			for (uint32_t id : ids)
			{
				result.push_back(EntityId(id));
			}
			return result;
		}

		// ========================================================================
		// 2-Way Intersection Tests (if constexpr specialization)
		// ========================================================================

		// Note: The Intersection functions expect non-empty sets (callers check for 
		// end iterators before calling). Testing empty sets would cause undefined behavior.

		TEST(Intersection, TwoWay_NoIntersection)
		{
			auto vec1 = makeEntityIds({1, 3, 5, 7});
			auto vec2 = makeEntityIds({2, 4, 6, 8});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, TwoWay_SingleIntersection)
		{
			auto vec1 = makeEntityIds({1, 3, 5, 7});
			auto vec2 = makeEntityIds({2, 4, 5, 8});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(5));
		}

		TEST(Intersection, TwoWay_MultipleIntersections)
		{
			auto vec1 = makeEntityIds({1, 3, 5, 7, 9});
			auto vec2 = makeEntityIds({3, 5, 7, 10});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			// Find first intersection
			bool found = Intersection::intersectN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(3));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(3));
		}

		TEST(Intersection, TwoWay_AllSame)
		{
			auto vec1 = makeEntityIds({1, 2, 3, 4, 5});
			auto vec2 = makeEntityIds({1, 2, 3, 4, 5});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(1));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(1));
		}

		TEST(Intersection, TwoWay_DisjointRanges)
		{
			auto vec1 = makeEntityIds({1, 2, 3});
			auto vec2 = makeEntityIds({10, 20, 30});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		// ========================================================================
		// 3-Way Intersection Tests (Galloping algorithm)
		// ========================================================================

		// Note: The Intersection functions expect non-empty sets (callers check for 
		// end iterators before calling). Testing empty sets would cause undefined behavior.

		TEST(Intersection, ThreeWay_NoIntersection)
		{
			auto vec1 = makeEntityIds({1, 4, 7});
			auto vec2 = makeEntityIds({2, 5, 8});
			auto vec3 = makeEntityIds({3, 6, 9});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, ThreeWay_SingleIntersection)
		{
			auto vec1 = makeEntityIds({1, 3, 5, 7, 10});
			auto vec2 = makeEntityIds({2, 5, 8, 10, 15});
			auto vec3 = makeEntityIds({5, 6, 10, 20});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(5));
		}

		TEST(Intersection, ThreeWay_MultipleIntersections)
		{
			auto vec1 = makeEntityIds({5, 10, 15, 20});
			auto vec2 = makeEntityIds({5, 10, 12, 20, 25});
			auto vec3 = makeEntityIds({1, 5, 10, 20});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			// Find first intersection (5)
			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(5));
		}

		TEST(Intersection, ThreeWay_AllSame)
		{
			auto vec1 = makeEntityIds({1, 2, 3, 4, 5});
			auto vec2 = makeEntityIds({1, 2, 3, 4, 5});
			auto vec3 = makeEntityIds({1, 2, 3, 4, 5});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(1));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(1));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(1));
		}

		// ========================================================================
		// Galloping Threshold Tests (Linear vs Binary Search)
		// ========================================================================

		TEST(Intersection, ThreeWay_SmallGap_LinearScan)
		{
			// Gap of 10 elements (< 32 threshold) - should use linear scan
			auto vec1 = makeEntityIds({1, 20});
			auto vec2 = makeEntityIds({2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 20});
			auto vec3 = makeEntityIds({20, 30});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(20));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(20));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(20));
		}

		TEST(Intersection, ThreeWay_LargeGap_BinarySearch)
		{
			// Gap of 100 elements (> 32 threshold) - should trigger binary search (galloping)
			std::vector<EntityId> vec1 = makeEntityIds({1, 150});
			
			std::vector<EntityId> vec2;
			for (uint32_t i = 2; i <= 149; ++i)
			{
				vec2.push_back(EntityId(i));
			}
			vec2.push_back(EntityId(150));

			std::vector<EntityId> vec3 = makeEntityIds({150, 200});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(150));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(150));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(150));
		}

		// ========================================================================
		// 4-Way Intersection Tests
		// ========================================================================

		TEST(Intersection, FourWay_SingleIntersection)
		{
			auto vec1 = makeEntityIds({5, 10, 15});
			auto vec2 = makeEntityIds({5, 12, 20});
			auto vec3 = makeEntityIds({1, 5, 25});
			auto vec4 = makeEntityIds({5, 8, 30});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin(), vec4.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end(), vec4.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2, 3>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<3>(iterators), EntityId(5));
		}

		TEST(Intersection, FourWay_NoIntersection)
		{
			auto vec1 = makeEntityIds({1, 5, 9});
			auto vec2 = makeEntityIds({2, 6, 10});
			auto vec3 = makeEntityIds({3, 7, 11});
			auto vec4 = makeEntityIds({4, 8, 12});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin(), vec4.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end(), vec4.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2, 3>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		// ========================================================================
		// Larger N-Way Intersection Tests (5+)
		// ========================================================================

		TEST(Intersection, FiveWay_SingleIntersection)
		{
			auto vec1 = makeEntityIds({10, 20, 30});
			auto vec2 = makeEntityIds({5, 10, 15, 20});
			auto vec3 = makeEntityIds({10, 25});
			auto vec4 = makeEntityIds({1, 10, 100});
			auto vec5 = makeEntityIds({10, 50});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin(), vec4.begin(), vec5.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end(), vec4.end(), vec5.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2, 3, 4>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(10));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(10));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(10));
			ASSERT_EQ(*std::get<3>(iterators), EntityId(10));
			ASSERT_EQ(*std::get<4>(iterators), EntityId(10));
		}

		TEST(Intersection, SixWay_MultipleIntersections)
		{
			auto vec1 = makeEntityIds({5, 10, 15, 20});
			auto vec2 = makeEntityIds({5, 10, 15, 20, 25});
			auto vec3 = makeEntityIds({1, 5, 10, 15, 20});
			auto vec4 = makeEntityIds({5, 10, 15, 20, 30});
			auto vec5 = makeEntityIds({5, 10, 15, 20});
			auto vec6 = makeEntityIds({5, 10, 15, 20, 35});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin(), vec4.begin(), vec5.begin(), vec6.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end(), vec4.end(), vec5.end(), vec6.end());

			// Find first intersection (5)
			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2, 3, 4, 5>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));

			// Find second intersection (10)
			found = Intersection::incrementN(std::index_sequence<0, 1, 2, 3, 4, 5>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(10));

			// Find third intersection (15)
			found = Intersection::incrementN(std::index_sequence<0, 1, 2, 3, 4, 5>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(15));

			// Find fourth intersection (20)
			found = Intersection::incrementN(std::index_sequence<0, 1, 2, 3, 4, 5>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(20));

			// No more intersections
			found = Intersection::incrementN(std::index_sequence<0, 1, 2, 3, 4, 5>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, EightWay_NoIntersection)
		{
			auto vec1 = makeEntityIds({1, 9, 17});
			auto vec2 = makeEntityIds({2, 10, 18});
			auto vec3 = makeEntityIds({3, 11, 19});
			auto vec4 = makeEntityIds({4, 12, 20});
			auto vec5 = makeEntityIds({5, 13, 21});
			auto vec6 = makeEntityIds({6, 14, 22});
			auto vec7 = makeEntityIds({7, 15, 23});
			auto vec8 = makeEntityIds({8, 16, 24});

			auto iterators = std::make_tuple(
				vec1.begin(), vec2.begin(), vec3.begin(), vec4.begin(),
				vec5.begin(), vec6.begin(), vec7.begin(), vec8.begin()
			);
			auto endIterators = std::make_tuple(
				vec1.end(), vec2.end(), vec3.end(), vec4.end(),
				vec5.end(), vec6.end(), vec7.end(), vec8.end()
			);

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2, 3, 4, 5, 6, 7>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, EightWay_SingleIntersection)
		{
			auto vec1 = makeEntityIds({42, 100, 200});
			auto vec2 = makeEntityIds({1, 42, 150});
			auto vec3 = makeEntityIds({5, 42, 180});
			auto vec4 = makeEntityIds({10, 42, 190});
			auto vec5 = makeEntityIds({15, 42, 195});
			auto vec6 = makeEntityIds({20, 42, 198});
			auto vec7 = makeEntityIds({25, 42, 199});
			auto vec8 = makeEntityIds({30, 42, 201});

			auto iterators = std::make_tuple(
				vec1.begin(), vec2.begin(), vec3.begin(), vec4.begin(),
				vec5.begin(), vec6.begin(), vec7.begin(), vec8.begin()
			);
			auto endIterators = std::make_tuple(
				vec1.end(), vec2.end(), vec3.end(), vec4.end(),
				vec5.end(), vec6.end(), vec7.end(), vec8.end()
			);

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2, 3, 4, 5, 6, 7>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<3>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<4>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<5>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<6>(iterators), EntityId(42));
			ASSERT_EQ(*std::get<7>(iterators), EntityId(42));
		}

		// ========================================================================
		// beginN Tests
		// ========================================================================

		TEST(Intersection, BeginN_AlreadyAtIntersection)
		{
			auto vec1 = makeEntityIds({5, 10, 15});
			auto vec2 = makeEntityIds({5, 12, 20});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(5));
		}

		TEST(Intersection, BeginN_NotAtIntersection)
		{
			auto vec1 = makeEntityIds({1, 10, 15});
			auto vec2 = makeEntityIds({5, 10, 20});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(10));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(10));
		}

		TEST(Intersection, BeginN_EmptySet_FirstPosition)
		{
			auto vec1 = makeEntityIds({});
			auto vec2 = makeEntityIds({5, 10, 20});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, BeginN_EmptySet_SecondPosition)
		{
			auto vec1 = makeEntityIds({5, 10, 20});
			auto vec2 = makeEntityIds({});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, BeginN_EmptySet_ThreeWay_FirstPosition)
		{
			auto vec1 = makeEntityIds({});
			auto vec2 = makeEntityIds({5, 10});
			auto vec3 = makeEntityIds({5, 10});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, BeginN_EmptySet_ThreeWay_MiddlePosition)
		{
			auto vec1 = makeEntityIds({5, 10});
			auto vec2 = makeEntityIds({});
			auto vec3 = makeEntityIds({5, 10});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, BeginN_EmptySet_ThreeWay_LastPosition)
		{
			auto vec1 = makeEntityIds({5, 10});
			auto vec2 = makeEntityIds({5, 10});
			auto vec3 = makeEntityIds({});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, BeginN_EmptySet_AllEmpty)
		{
			auto vec1 = makeEntityIds({});
			auto vec2 = makeEntityIds({});
			auto vec3 = makeEntityIds({});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::beginN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		// ========================================================================
		// incrementN Tests
		// ========================================================================

		TEST(Intersection, IncrementN_FindNext)
		{
			auto vec1 = makeEntityIds({5, 10, 15, 20});
			auto vec2 = makeEntityIds({5, 10, 12, 20});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			// Start at first intersection (5)
			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));

			// Increment to next intersection (10)
			found = Intersection::incrementN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(10));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(10));

			// Increment to next intersection (20)
			found = Intersection::incrementN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(20));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(20));

			// No more intersections
			found = Intersection::incrementN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, IncrementN_AlreadyAtNextIntersection)
		{
			// Special case: after increment, iterators are already at intersection
			auto vec1 = makeEntityIds({5, 6, 10});
			auto vec2 = makeEntityIds({5, 6, 10});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			// Start at 5
			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));

			// Increment - should land at 6 (consecutive intersection)
			found = Intersection::incrementN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(6));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(6));
		}

		TEST(Intersection, IncrementN_ReachEnd)
		{
			auto vec1 = makeEntityIds({5});
			auto vec2 = makeEntityIds({5, 10});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end());

			// Start at 5
			bool found = Intersection::beginN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(5));

			// Increment - vec1 will reach end
			found = Intersection::incrementN(std::index_sequence<0, 1>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		// ========================================================================
		// Edge Cases and Stress Tests
		// ========================================================================

		TEST(Intersection, SingleElement_Match)
		{
			auto vec1 = makeEntityIds({42});
			auto vec2 = makeEntityIds({42});
			auto vec3 = makeEntityIds({42});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(42));
		}

		TEST(Intersection, SingleElement_NoMatch)
		{
			auto vec1 = makeEntityIds({1});
			auto vec2 = makeEntityIds({2});
			auto vec3 = makeEntityIds({3});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_FALSE(found);
		}

		TEST(Intersection, SkewedSizes)
		{
			// One very large set, others small - tests galloping efficiency
			std::vector<EntityId> vec1;
			for (uint32_t i = 1; i <= 1000; ++i)
			{
				vec1.push_back(EntityId(i));
			}

			auto vec2 = makeEntityIds({100, 500, 900});
			auto vec3 = makeEntityIds({50, 100, 500, 900, 950});

			auto iterators = std::make_tuple(vec1.begin(), vec2.begin(), vec3.begin());
			auto endIterators = std::make_tuple(vec1.end(), vec2.end(), vec3.end());

			// Find first intersection (100)
			bool found = Intersection::intersectN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(100));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(100));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(100));

			// Find next intersection (500)
			found = Intersection::incrementN(std::index_sequence<0, 1, 2>{}, iterators, endIterators);
			ASSERT_TRUE(found);
			ASSERT_EQ(*std::get<0>(iterators), EntityId(500));
			ASSERT_EQ(*std::get<1>(iterators), EntityId(500));
			ASSERT_EQ(*std::get<2>(iterators), EntityId(500));
		}

	} // namespace Test
} // namespace SubzeroECS
