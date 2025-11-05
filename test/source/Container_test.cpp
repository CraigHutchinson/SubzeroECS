#include "SubzeroECS/World.hpp"
#include "SubzeroECS/Collection.hpp"
#include "SubzeroECS/Has.hpp"
#include "SubzeroECS/Query.hpp"
#include "SubzeroECS/Logical.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>


namespace SubzeroECS {	
namespace Test {
		
	TEST( World, CreateEntity)
	{
		World world;
		Entity entity = world.create();
		Entity entityB = world.create();
		ASSERT_FALSE( entity.isNull() );
		ASSERT_NE( Entity(), entity );
		ASSERT_NE( entity, entityB );
	}

	/*
	TEST( World, Has)
	{

	World world;
	Collection<Human,Health,Hat> collections(world.collectionRegistry());
	(void)world.create(Human(), Health(cHealthPercent), Hat());

	ASSERT_TRUE( has<Health>(world) );
	ASSERT_TRUE( has<Human>(world) );
	ASSERT_TRUE( has<Hat>(world) );
	ASSERT_FALSE( has<Shoes>(world) );
	}*/

} //END: Test
} //END: SubzeroECS