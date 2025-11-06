#include "SubzeroECS/CollectionRegistry.hpp"
#include "SubzeroECS/Collection.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>

namespace SubzeroECS {
	namespace Test 
	{

		TEST(Collection,Create)
		{
			CollectionRegistry collectionRegistry;
			Collection<Human> humanCollection(collectionRegistry);
			ASSERT_NE( nullptr, humanCollection.create( EntityId{0U}, Human() ) );
		}


	} //END: Test
} //END: SubzeroECS



