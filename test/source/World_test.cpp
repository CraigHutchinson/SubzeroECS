#include "SubzeroECS/World.hpp"
#include "SubzeroECS/Collection.hpp"
#include "SubzeroECS/Has.hpp"
#include "SubzeroECS/Query.hpp"
#include "SubzeroECS/Logical.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>


namespace SubzeroECS {	
namespace Test {
		
	TEST(World, CreateEntity)
	{
		World world;
		Entity entity = world.create();
		Entity entityB = world.create();
		ASSERT_FALSE(entity.isNull());
		ASSERT_NE(Entity(), entity);
		ASSERT_NE(entity, entityB);
	}

	TEST(World, CreateWithComponents)
	{
		World world;
		Collection<Human, Health, Hat> collections(world);
		
		Entity entity = world.create(Human{}, Health{50.0f}, Hat{});
		
		ASSERT_TRUE(entity.has<Human>());
		ASSERT_TRUE(entity.has<Health>());
		ASSERT_TRUE(entity.has<Hat>());
		ASSERT_EQ(entity.get<Health>().percent, 50.0f);
	}

	TEST(World, AddComponent)
	{
		World world;
		Collection<Health> healthCollection(world);
		
		Entity entity = world.create();
		world.add(entity.id(), Health{75.0f});
		
		ASSERT_TRUE(world.has<Health>(entity.id()));
		ASSERT_EQ(world.get<Health>(entity.id()).percent, 75.0f);
	}

	TEST(World, FindComponent)
	{
		World world;
		Collection<Health> healthCollection(world);
		
		Entity entity = world.create();
		
		// Should return nullptr when component doesn't exist
		ASSERT_EQ(world.find<Health>(entity.id()), nullptr);
		
		// Should return pointer when component exists
		world.add(entity.id(), Health{100.0f});
		Health* health = world.find<Health>(entity.id());
		ASSERT_NE(health, nullptr);
		ASSERT_EQ(health->percent, 100.0f);
	}

	TEST(World, MultipleEntitiesIndependent)
	{
		World world;
		Collection<Health, Hat> collections(world);
		
		Entity entity1 = world.create(Health{50.0f});
		Entity entity2 = world.create(Health{100.0f});
		
		ASSERT_NE(entity1.id(), entity2.id());
		ASSERT_EQ(world.get<Health>(entity1.id()).percent, 50.0f);
		ASSERT_EQ(world.get<Health>(entity2.id()).percent, 100.0f);
	}

	TEST(World, SequentialEntityIds)
	{
		World world;
		Entity entity1 = world.create();
		Entity entity2 = world.create();
		Entity entity3 = world.create();
		
		// Entities should have sequential IDs
		ASSERT_EQ(entity2.id().value, entity1.id().value + 1);
		ASSERT_EQ(entity3.id().value, entity2.id().value + 1);
	}

	TEST(World, AsCollectionRegistry)
	{
		World world;
		Collection<Human> humanCollection(world);
		
		// World inherits from CollectionRegistry
		CollectionRegistry& registry = world;
		ASSERT_EQ(&registry.get<Human>(), &humanCollection);
	}

} //END: Test
} //END: SubzeroECS