#include "SubzeroECS/Entity.hpp"
#include "SubzeroECS/World.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "SubzeroECS/Collection.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "SubzeroECS/Has.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test 
{
	TEST(Entity,Null)
	{
		Entity nullA;
		Entity nullB( *(World*)0, EntityId::Invalid );
		Entity nullC( *(World*)3, EntityId::Invalid );

		ASSERT_TRUE( nullA.isNull() );
		ASSERT_TRUE( nullB.isNull() );
		ASSERT_TRUE( nullC.isNull() );
		ASSERT_EQ( nullA, Entity() );
		ASSERT_EQ( nullB, Entity() );
		ASSERT_EQ( nullC, Entity() );
		ASSERT_EQ( nullB, nullA );
		ASSERT_EQ( nullC, nullA );
		ASSERT_EQ( nullC, nullB );
	}

	TEST(Entity,NotNull)
	{
		Entity entity( *(World*)456, EntityId{123U} );
		ASSERT_FALSE( entity.isNull() );
	}

	TEST(Entity,ConstructId)
	{
		Entity entity( *(World*)456, EntityId{123U} );
		ASSERT_EQ( EntityId{123U}, entity.id() );
	}

	TEST(Entity,Equality)
	{
		Entity entity( *(World*)456, EntityId{123U} );
		ASSERT_EQ( Entity(*(World*)456, EntityId{123U}), entity );
		ASSERT_NE( Entity(*(World*)456, EntityId{124U}), entity );
		ASSERT_NE( Entity(*(World*)455, EntityId{123U}), entity );
	}

	TEST(Entity,CopyConstruct)
	{
		Entity entity( *(World*)456, EntityId{123U} );
		Entity entityB( entity ); //< Entity handle - Has reference to world i.e. struct{ uint32_t, World& }
		ASSERT_EQ(entityB, entity);
	}

	TEST(Entity,CopyAssignment)
	{
		Entity entity( *(World*)456, EntityId{123U} );
		Entity entityC;
		entityC = entity;
		ASSERT_EQ(entityC, entity);
	}
	
	TEST(Entity,Has)
	{
		World world;
		Collection<Human,Health,Hat> collections(world);
		Entity entity = world.create(Human(), Health{100}, Hat());

		//Member
		ASSERT_TRUE( entity.has<Health>() );
		ASSERT_TRUE( entity.has<Human>() );
		ASSERT_TRUE( entity.has<Hat>() );
		ASSERT_FALSE( entity.has<Shoes>() );

		//Global
		ASSERT_TRUE( has<Health>(entity) );
		ASSERT_TRUE( has<Human>(entity) );
		ASSERT_TRUE( has<Hat>(entity) );
		ASSERT_FALSE( has<Shoes>(entity) );

		//Global handle
        EntityId entityId = entity.id();
		ASSERT_TRUE( has<Health>(world,entityId) );
        ASSERT_TRUE( has<Human>(world, entityId) );
		ASSERT_TRUE( has<Hat>(world,entityId) );
		ASSERT_FALSE( has<Shoes>(world,entityId) );
	}

	TEST(Entity,Get)
	{
		World world;
		Collection<Human,Health,Hat> collections(world);
		const Entity entity = world.create(Human(), Health{100}, Hat());
		const EntityId entityId = entity.id();

		//Entity
		ASSERT_NE( nullptr, &entity.get<Human>() );
		ASSERT_NE( nullptr, &entity.get<Health>() );
		ASSERT_NE( nullptr, &entity.get<Hat>() );
		
		//Global
		ASSERT_NE( nullptr, &get<Human>(entity) );
		ASSERT_NE( nullptr, &get<Health>(entity) );
		ASSERT_NE( nullptr, &get<Hat>(entity) );

		//World
		ASSERT_NE( nullptr, &world.get<Human>(entityId) );
		ASSERT_NE( nullptr, &world.get<Health>(entityId) );
		ASSERT_NE( nullptr, &world.get<Hat>(entityId) );

		//Global handle
		ASSERT_NE( nullptr, &get<Human>(world,entityId) );
		ASSERT_NE( nullptr, &get<Health>(world,entityId) );
		ASSERT_NE( nullptr, &get<Hat>(world,entityId) );

		//Verify that the same objects returned on all interfaces
 		ASSERT_EQ( &get<Human>(entity), &entity.get<Human>() );
		ASSERT_EQ( &entity.get<Human>(), &get<Human>(world,entity.id()) );
	}

	TEST(Entity,GetHasCorrectValue)
	{
		float cHealthPercent = rand() * (100.0f/float(RAND_MAX)); //< Random to ensure component memory initialised

		World world;
		Collection<Health> collections(world.collectionRegistry());
		Entity entity = world.create( Health{cHealthPercent} );

		ASSERT_EQ( cHealthPercent, entity.get<Health>().percent );
	}

	TEST(Entity,Find)
	{
		World world;
		Collection<Human,Health,Hat> collections(world);
		Entity entity = world.create(Human(), Health{100}, Hat());

		//Member
		ASSERT_NE( nullptr, entity.find<Human>() );
		ASSERT_NE( nullptr, entity.find<Health>() );
		ASSERT_NE( nullptr, entity.find<Hat>() );
		ASSERT_EQ( nullptr, entity.find<Shoes>() );

		//Global
		ASSERT_NE( nullptr, find<Human>(entity) );
		ASSERT_NE( nullptr, find<Health>(entity) );
		ASSERT_NE( nullptr, find<Hat>(entity) );
		ASSERT_EQ( nullptr, find<Shoes>(entity) );

		//Global handle
		ASSERT_NE( nullptr, find<Human>(world,entity.id()) );
		ASSERT_NE( nullptr, find<Health>(world,entity.id()) );
		ASSERT_NE( nullptr, find<Hat>(world,entity.id()) );
		ASSERT_EQ( nullptr, find<Shoes>(world,entity.id()) );

		//Verify that the same objects returned on all interfaces
		ASSERT_EQ( find<Human>(entity), entity.find<Human>() );
		ASSERT_EQ( entity.find<Human>(), find<Human>(world,entity.id()) );
	}

	TEST(Entity,FindHasCorrectValue)
	{
		const float cHealthPercent = rand() * (100.0f/float(RAND_MAX)); //< Random to ensure component memory initialised
		const float cHealthPercentB = rand() * (100.0f/float(RAND_MAX));
		const float cShoeSize = rand() / 100.0F;
		const float cShoeSizeB = rand() / 100.0F;

		World world;
		Collection<Health,Shoes> collections(world);
		Entity entity = world.create( Health{cHealthPercent}, Shoes{cShoeSize} );
		Entity entityB = world.create( Health{cHealthPercentB}, Shoes{cShoeSizeB} );

		ASSERT_EQ( cHealthPercent, entity.find<Health>()->percent);
		ASSERT_EQ( cShoeSize, entity.find<Shoes>()->size);

		ASSERT_EQ( cHealthPercentB, entityB.find<Health>()->percent);
		ASSERT_EQ( cShoeSizeB, entityB.find<Shoes>()->size);
	}

	TEST(Entity,GetInvalid_Throws)
	{
		World world;
		Entity entity = world.create();

		//Member
		ASSERT_THROW( entity.get<Shoes>(), std::invalid_argument );

		//Global
		ASSERT_THROW( get<Health>(entity), std::invalid_argument );

		//Global handle
		ASSERT_THROW( get<Hat>(world,entity.id()), std::invalid_argument );
	}

	TEST(Entity,Add)
	{
		World world;
		Collection<Human,Health,Hat,Shoes> collections(world.collectionRegistry());
		Entity entity = world.create(Human());

		ASSERT_TRUE( has<Human>(entity) );
		ASSERT_FALSE( has<Health>(entity) );
		ASSERT_FALSE( has<Hat>(entity) );
		ASSERT_FALSE( has<Shoes>(entity) );

		entity.add( Health{100} );
		ASSERT_TRUE( has<Human>(entity) );
		ASSERT_TRUE( has<Health>(entity) );
		ASSERT_FALSE( has<Hat>(entity) );
		ASSERT_FALSE( has<Shoes>(entity) );

		add( entity, Hat() );
		ASSERT_TRUE( has<Human>(entity) );
		ASSERT_TRUE( has<Health>(entity) );
		ASSERT_TRUE( has<Hat>(entity) );
		ASSERT_FALSE( has<Shoes>(entity) );

		add( world, entity.id(), Shoes{9} );
		ASSERT_TRUE( has<Human>(entity) );
		ASSERT_TRUE( has<Health>(entity) );
		ASSERT_TRUE( has<Hat>(entity) );
		ASSERT_TRUE( has<Shoes>(entity) );
	}


	TEST(Entity,AddHasCorrectValue)
	{
		float cHealthPercent = rand() * (100.0f/float(RAND_MAX)); //< Random to ensure component memory initialised
		float cHealthShoeSize = rand() * (12.0f/float(RAND_MAX)); //< Random to ensure component memory initialised

		World world;
		Collection<Health,Shoes> collections(world.collectionRegistry());
		Entity entity = world.create();

		entity.add( Health{cHealthPercent} );
		ASSERT_EQ( cHealthPercent, get<Health>(entity).percent );

		add( world, entity.id(), Shoes{cHealthShoeSize} );
		ASSERT_EQ( cHealthShoeSize, get<Shoes>(entity).size );
	}

	/*
	TEST(Entity,GetMore)
	{
	World world;
	Collection<Human,Health,Hat> collections(world.collectionRegistry());
	Entity entity = world.create(Human(), Health(cHealthPercent), Hat());

	std::tuple<Human,Health,Hat> hhh = get<Human,Health,Hat>(entity);
	}*/
	/*
	TEST_METHOD(World_Get)
	{
	const float cHealthPercent = cHealthPercent;

	World world;
	Collection<Human,Health,Hat> collections(world.collectionRegistry());
	(void)world.create(Human(), Health(cHealthPercent), Hat());
	(void)world.create(Human(), Health(cHealthPercent) );
	(void)world.create(Human());

	ASSERT_EQ( get<Human>(world).size(), 3u );
	ASSERT_EQ( get<Health>(world).size(), 2u );
	ASSERT_EQ( get<Hat>(world).size(), 1 );
	ASSERT_EQ( get<Shoes>(world).size(), 0u );


	ASSERT_EQ( get<Health>(world)[0].percent, cHealthPercent );
	}
	*/

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


} //END: Test
} //END: SubzeroECS