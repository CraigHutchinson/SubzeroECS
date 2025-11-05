#include "SubzeroECS/CollectionRegistry.hpp"

#include "SubzeroECS/Collection.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test 
{

	TEST(CollectionRegistry,Collection_Register)
	{
		CollectionRegistry collectionRegistry;
		Collection<Human> humanCollection(collectionRegistry);
		Collection<Health> healthCollection(collectionRegistry);
		Collection<Hat> hatCollection(collectionRegistry);

		ASSERT_EQ( &collectionRegistry.get<Hat>(), &hatCollection );
		ASSERT_EQ( &collectionRegistry.get<Human>(), &humanCollection );
		ASSERT_EQ( &collectionRegistry.get<Health>(), &healthCollection );
	}

	TEST(CollectionRegistry,Collections_Register)
	{
		CollectionRegistry collectionRegistry;
		Collection<Human,Health,Hat> collections(collectionRegistry);

		ASSERT_EQ( &collectionRegistry.get<Hat>(), &collections.get<Hat>() );
		ASSERT_EQ( &collectionRegistry.get<Human>(), &collections.get<Human>() );
		ASSERT_EQ( &collectionRegistry.get<Health>(), &collections.get<Health>() );
	}

	TEST(CollectionRegistry,GetInvalid_Throws)
	{
		CollectionRegistry collectionRegistry;

		ASSERT_EQ( nullptr, collectionRegistry.find<Shoes>() );
		ASSERT_EQ( nullptr, collectionRegistry.find<Hat>() );
		ASSERT_EQ( nullptr, collectionRegistry.find<Health>() );

		//Get should throw
		ASSERT_THROW( collectionRegistry.get<Shoes>(), std::invalid_argument );
		ASSERT_THROW( collectionRegistry.get<Hat>(), std::invalid_argument );
		ASSERT_THROW( collectionRegistry.get<Health>(), std::invalid_argument );
	}

	/** Collection will unregister when out of scope
	*/
	TEST(CollectionRegistry,Unregister)
	{
		CollectionRegistry collectionRegistry;
		{
			Collection<Human> instance( collectionRegistry );
			// Note: Collection de-registers when desctructed
		}
		ASSERT_EQ( nullptr, collectionRegistry.find<Human>() );
	}

	TEST(CollectionRegistry,DuplicateType_Throws)
	{
		CollectionRegistry collectionRegistry;
		Collection<Human> instanceA( collectionRegistry );
		ASSERT_THROW( Collection<Human> instanceB( collectionRegistry ), std::invalid_argument );
	}

	TEST(CollectionRegistry,ReRegisterType)
	{
		CollectionRegistry collectionRegistry;
		{
			Collection<Human> instanceA( collectionRegistry ); // Note: Collection de-registers when desctructed
		}

		Collection<Human> instanceB( collectionRegistry ); // Second Collection
		ASSERT_EQ( &instanceB, collectionRegistry.find<Human>() );
	}
/*
	TEST(CollectionRegistry,StackCollectionsRegistration )
	{
		CollectionRegistry collectionRegistry;
		Collection<Human,Health,Hat> collections(collectionRegistry);
	}
	*/

} //END: Test
} //END: SubzeroECS



