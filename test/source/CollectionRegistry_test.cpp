#include "SubzeroECS/CollectionRegistry.hpp"

#include "SubzeroECS/Collection.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

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

	TEST(CollectionRegistry, MultipleRegistries_Independent)
	{
		CollectionRegistry registry1;
		CollectionRegistry registry2;
		
		Collection<Human> human1(registry1);
		Collection<Human> human2(registry2);
		
		// Each registry should have its own collection instance
		ASSERT_NE(&human1, &human2);
		ASSERT_EQ(&registry1.get<Human>(), &human1);
		ASSERT_EQ(&registry2.get<Human>(), &human2);
	}

	TEST(CollectionRegistry, MultipleRegistries_SeparateTypes)
	{
		CollectionRegistry registry1;
		CollectionRegistry registry2;
		
		Collection<Human> humanInRegistry1(registry1);
		Collection<Health> healthInRegistry2(registry2);
		
		// Registry1 should only have Human
		ASSERT_EQ(&registry1.get<Human>(), &humanInRegistry1);
		ASSERT_EQ(nullptr, registry1.find<Health>());
		
		// Registry2 should only have Health
		ASSERT_EQ(&registry2.get<Health>(), &healthInRegistry2);
		ASSERT_EQ(nullptr, registry2.find<Human>());
	}

	TEST(CollectionRegistry, MultipleRegistries_SameTypeDifferentInstances)
	{
		CollectionRegistry registry1;
		CollectionRegistry registry2;
		CollectionRegistry registry3;
		
		Collection<Hat> hat1(registry1);
		Collection<Hat> hat2(registry2);
		Collection<Hat> hat3(registry3);
		
		// All three should be different instances
		ASSERT_NE(&hat1, &hat2);
		ASSERT_NE(&hat1, &hat3);
		ASSERT_NE(&hat2, &hat3);
		
		// Each registry should reference its own collection
		ASSERT_EQ(&registry1.get<Hat>(), &hat1);
		ASSERT_EQ(&registry2.get<Hat>(), &hat2);
		ASSERT_EQ(&registry3.get<Hat>(), &hat3);
	}

	TEST(CollectionRegistry, MultipleRegistries_MixedTypes)
	{
		CollectionRegistry registry1;
		CollectionRegistry registry2;
		
		Collection<Human, Health> collections1(registry1);
		Collection<Hat, Shoes> collections2(registry2);
		
		// Registry1 should have Human and Health
		ASSERT_EQ(&registry1.get<Human>(), &collections1.get<Human>());
		ASSERT_EQ(&registry1.get<Health>(), &collections1.get<Health>());
		ASSERT_EQ(nullptr, registry1.find<Hat>());
		ASSERT_EQ(nullptr, registry1.find<Shoes>());
		
		// Registry2 should have Hat and Shoes
		ASSERT_EQ(&registry2.get<Hat>(), &collections2.get<Hat>());
		ASSERT_EQ(&registry2.get<Shoes>(), &collections2.get<Shoes>());
		ASSERT_EQ(nullptr, registry2.find<Human>());
		ASSERT_EQ(nullptr, registry2.find<Health>());
	}

	TEST(CollectionRegistry, MultipleRegistries_ScopedUnregister)
	{
		CollectionRegistry registry1;
		CollectionRegistry registry2;
		
		Collection<Human> human1(registry1);
		
		{
			Collection<Human> human2(registry2);
			ASSERT_NE(nullptr, registry2.find<Human>());
		}
		
		// Registry2 should have unregistered Human
		ASSERT_EQ(nullptr, registry2.find<Human>());
		
		// Registry1 should still have Human
		ASSERT_EQ(&registry1.get<Human>(), &human1);
	}

	TEST(CollectionRegistry, MultipleRegistries_OverlappingTypes)
	{
		CollectionRegistry registry1;
		CollectionRegistry registry2;
		
		Collection<Human, Health, Hat> collections1(registry1);
		Collection<Health, Hat, Shoes> collections2(registry2);
		
		// Both should have Health and Hat, but different instances
		ASSERT_NE(&registry1.get<Health>(), &registry2.get<Health>());
		ASSERT_NE(&registry1.get<Hat>(), &registry2.get<Hat>());
		
		// Registry1 should have Human but not Shoes
		ASSERT_NE(nullptr, registry1.find<Human>());
		ASSERT_EQ(nullptr, registry1.find<Shoes>());
		
		// Registry2 should have Shoes but not Human
		ASSERT_NE(nullptr, registry2.find<Shoes>());
		ASSERT_EQ(nullptr, registry2.find<Human>());
	}

	TEST(CollectionRegistry, ExceedCapacity_Throws)
	{
		// CollectionRegistry capacity is 32 (from UniqueIndex32)
		constexpr size_t capacity = CollectionRegistry::Capacity;
		ASSERT_EQ(capacity, 32u);
		
		// Create an array of registries up to capacity
		std::vector<std::unique_ptr<CollectionRegistry>> registries;
		registries.reserve(capacity);
		
		// Should be able to create exactly 'capacity' registries
		for (size_t i = 0; i < capacity; ++i) {
			ASSERT_NO_THROW(registries.push_back(std::make_unique<CollectionRegistry>()));
		}
		
		ASSERT_EQ(registries.size(), capacity);
		
		// Attempting to create one more should throw
		ASSERT_THROW(CollectionRegistry{}, std::overflow_error);
	}


} //END: Test
} //END: SubzeroECS



