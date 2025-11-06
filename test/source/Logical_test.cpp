#include "SubzeroECS/Logical.hpp"

#include "SubzeroECS/World.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "SubzeroECS/Collection.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "SubzeroECS/Has.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test {

TEST(LogicalTest, LogicalQueryHas)
{
	World world;
	Collection<Human> collections(world);
	Entity entity = world.create(Human());

	EXPECT_TRUE(entity % Has<Human>());
	EXPECT_TRUE(query(entity, Has<Human>()));
}

TEST(LogicalTest, LogicalQueryAndHas)
{
	World world;
	Collection<Human, Health, Hat> collections(world);
	Entity entity = world.create(Human(), Health{100}, Hat());

	EXPECT_TRUE(entity % (Has<Human>() && Has<Health>()));
	EXPECT_TRUE(entity % (Has<Health>() && Has<Human>()));
	EXPECT_TRUE(entity % (Has<Human>() && Has<Health>() && Has<Hat>()));
	EXPECT_TRUE(entity % (Has<Health>() && Has<Hat>() && Has<Human>()));

	EXPECT_FALSE(entity % Has<Glasses>());
	EXPECT_FALSE(entity % (Has<Glasses>() && Has<Human>()));
	EXPECT_FALSE(entity % (Has<Glasses>() && Has<Hat>() && Has<Human>()));
}

TEST(LogicalTest, LogicalQueryObject)
{
	World world;
	Collection<Human, Health, Hat> collections(world);
	Entity entityA = world.create(Human(), Health{100}, Hat());
	Entity entityB = world.create(Human(), Hat());

	auto hasCheck = (Has<Human>() && Has<Health>());
	EXPECT_TRUE(entityA % hasCheck);
	EXPECT_TRUE(query(entityA, hasCheck));
	EXPECT_FALSE(entityB % hasCheck);
	EXPECT_FALSE(query(entityB, hasCheck));
}

TEST(LogicalTest, LogicalQueryAndGreater)
{
	World world;
	Collection<Human, Health, Hat> collections(world);
	Entity entity = world.create(Human(), Health{100}, Hat());

	static_assert( Health{100} > Health{99} ); //Sanity check
	static_assert( !(Health{100} > Health{100}) ); //Sanity check

	EXPECT_TRUE(entity % (Has<Human>() && (Has<Health>() > Health{99})));
	EXPECT_FALSE(entity % (Has<Human>() && (Has<Health>() > Health{100})));
}

TEST(LogicalTest, LogicalQueryAndGreaterEqual)
{
	World world;
	Collection<Human, Health, Hat> collections(world);
	Entity entity = world.create(Human(), Health{100}, Hat());

	static_assert( Health{100} >= Health{100} ); //Sanity check
	static_assert( !(Health{100} >= Health{101}) ); //Sanity check

	EXPECT_TRUE(entity % (Has<Human>() && (Has<Health>() >= Health{100})));
	EXPECT_FALSE(entity % (Has<Human>() && (Has<Health>() >= Health{101})));
}

TEST(LogicalTest, LogicalQueryAndLess)
{
	World world;
	Collection<Human, Health, Hat> collections(world);
	Entity entity = world.create(Human(), Health{100}, Hat());
	EXPECT_TRUE(entity % (Has<Human>() && (Has<Health>() < Health{101})));
	EXPECT_FALSE(entity % (Has<Human>() && (Has<Health>() < Health{100})));
}

TEST(LogicalTest, LogicalQueryAndLessEqual)
{
	World world;
	Collection<Human, Health, Hat> collections(world);
	Entity entity = world.create(Human(), Health{100}, Hat());
	auto queryExpr = (Has<Human>() && (Has<Health>() <= Health{100}));
	EXPECT_TRUE(entity % queryExpr);
	auto queryFalse = (Has<Human>() && (Has<Health>() <= Health{99}));
	EXPECT_FALSE(entity % queryFalse);
}

/*
TEST(LogicalTest, WorldHasSingle)
{
World world;
Collection<Human,Health,Hat> collections(world);
Entity entity = world.create(Human(), Health(cHealthPercent), Hat());

using Has;

auto result = (world % has<Glasses>());
EXPECT_TRUE(result);
EXPECT_TRUE(result.size() == 1u);
EXPECT_TRUE(result[0] == entity);
}*/

} // namespace Test
} // namespace SubzeroECS