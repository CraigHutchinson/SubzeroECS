#include "SubzeroECS/Logical.hpp"

#include "SubzeroECS/World.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "SubzeroECS/Collection.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "SubzeroECS/Has.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test 
{

TEST_CLASS(Logical_Test)
{
public:
	TEST_METHOD(LogicalQuery_Has)
	{
		World world;
		Collection<Human> collections(world.collectionRegistry());
		Entity entity = world.create(Human());

		ASSERT_TRUE( entity % Has<Human>() );
		ASSERT_TRUE( query( entity, Has<Human>() ) );
	}

	TEST_METHOD(LogicalQuery_AndHas)
	{
		World world;
		Collection<Human,Health,Hat> collections(world.collectionRegistry());
		Entity entity = world.create(Human(), Health(100), Hat());

		ASSERT_TRUE(entity % (Has<Human>() && Has<Health>()) );
		ASSERT_TRUE(entity % (Has<Health>() && Has<Human>()));
		ASSERT_TRUE(entity % (Has<Human>() && Has<Health>() && Has<Hat>()) );
		ASSERT_TRUE(entity % (Has<Health>() && Has<Hat>() && Has<Human>()) );

		ASSERT_FALSE(entity % Has<Glasses>());
		ASSERT_FALSE(entity % (Has<Glasses>() && Has<Human>()) );
		ASSERT_FALSE(entity % (Has<Glasses>() && Has<Hat>() && Has<Human>()) );
	}

	TEST_METHOD(LogicalQuery_Object)
	{
		World world;
		Collection<Human,Health,Hat> collections(world.collectionRegistry());
		Entity entityA = world.create(Human(), Health(100), Hat());
		Entity entityB = world.create(Human(), Hat());

		auto hasCheck = (Has<Human>() && Has<Health>());
		ASSERT_TRUE(entityA % hasCheck);
		ASSERT_TRUE( query(entityA, hasCheck) );
		ASSERT_FALSE(entityB % hasCheck);
		ASSERT_FALSE( query(entityB, hasCheck) );
	}

	TEST_METHOD(LogicalQuery_AndGreater)
	{
		World world;
		Collection<Human,Health,Hat> collections(world.collectionRegistry());
		Entity entity = world.create(Human(), Health(100), Hat());

		ASSERT_TRUE(entity % (Has<Human>() && (Has<Health>() > 99)) );
		ASSERT_FALSE(entity % (Has<Human>() && (Has<Health>() > 100)) );
	}


	TEST_METHOD(LogicalQuery_AndGreaterEqual)
	{
		World world;
		Collection<Human,Health,Hat> collections(world.collectionRegistry());
		Entity entity = world.create(Human(), Health(100), Hat());

		ASSERT_TRUE(entity % (Has<Human>() && (Has<Health>() >= 100)) );
		ASSERT_FALSE(entity % (Has<Human>() && (Has<Health>() >= 101)) );
	}

	TEST_METHOD(LogicalQuery_AndLess)
	{
		World world;
		Collection<Human,Health,Hat> collections(world.collectionRegistry());
		Entity entity = world.create(Human(), Health(100), Hat());
		ASSERT_TRUE(entity % (Has<Human>() && (Has<Health>() < 101)) );
		ASSERT_FALSE(entity % (Has<Human>() && (Has<Health>() < 100)) );
	}

	TEST_METHOD(LogicalQuery_AndLessEqual)
	{
		World world;
		Collection<Human,Health,Hat> collections(world.collectionRegistry());
		Entity entity = world.create(Human(), Health(100), Hat());
		auto query = (Has<Human>() && (Has<Health>() <= 100));
		ASSERT_TRUE(entity % query );
		auto queryFalse = (Has<Human>() && (Has<Health>() <= 99));
		ASSERT_FALSE(entity % queryFalse );
	}

	/*
	TEST_METHOD(WorldHasSingle)
	{
	World world;
	Collection<Human,Health,Hat> collections(world.collectionRegistry());
	Entity entity = world.create(Human(), Health(cHealthPercent), Hat());

	using Has;

	auto result = (world % has<Glasses>());
	ASSERT_TRUE(result);
	ASSERT_TRUE(result.size() == 1u);
	ASSERT_TRUE(result[0] == entity);
	}*/

};

} //END: Test
} //END: SubzeroECS