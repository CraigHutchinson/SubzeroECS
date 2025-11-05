#include "SubzeroECS/World.hpp"
#include "SubzeroECS/Collection.hpp"
#include "SubzeroECS/Collection.hpp"
#include "SubzeroECS/View.hpp"

#include "TestTypes.hpp"
#include <gtest/gtest.h>


namespace SubzeroECS {	
	namespace Test {

		TEST( View, GetCollection )
		{
			CollectionRegistry registry;
			Collection<Human> humanCollection(registry);
			Collection<Health> healthCollection(registry);
			Collection<Hat> hatCollection(registry);

			View<Health, Human, Hat> humanHealthHatView(registry);
			ASSERT_EQ( &humanHealthHatView.getCollection<Human>(), &humanCollection );
			ASSERT_EQ( &humanHealthHatView.getCollection<Health>(), &healthCollection );
			ASSERT_EQ( &humanHealthHatView.getCollection<Hat>(), &hatCollection );
		}

		TEST(View,GetCollection_Throws)
		{
			CollectionRegistry registry;
			ASSERT_THROW( View<Human> humanView(registry), std::invalid_argument );
		}


		TEST( View, Begin_Empty )
		{
			World registry;
			Collection<Health,Hat> collections(registry);
			View<Health, Hat> view(registry);
			ASSERT_EQ( view.begin(), view.end() );
		}

		TEST( View, Begin_NotEmpty )
		{
			World registry;
			Collection<Health,Hat> collections(registry);
			(void)registry.create( Health{100.0F}, Hat() );

			View<Health, Hat> view(registry);
			ASSERT_NE( view.begin(), view.end() );
		}

		TEST( View, Begin_HasPartial )
		{
			World registry;
			Collection<Health,Hat> collections(registry);
			View<Health, Hat> view(registry);
			(void)registry.create( Health{100.0F}/* , Hat() -- Missing hat! */ );
			auto iBegin = view.begin();
			auto iEnd = view.end();
			ASSERT_EQ( iBegin, iEnd );
		}

#if 0 //NOT YET IMPLEMENTED: Hat* optional component
		TEST( View, Begin_NotCompleteOptional )
		{
			World registry;
			Collection<Health,Hat> collections(registry);
			View<Health, Hat*> view(registry); //< Hat is optional with '*'
			(void)registry.create( Health{100.0F}/* , Hat() -- Missing hat! */ );
			ASSERT_EQ( view.begin(), view.end() );
		}
#endif

		TEST( View, Intersect2 )
		{
			World world;
			Collection<Human,Hat> collections(world);
			View<Human,Hat> view(world); 

			for ( EntityId humanId : { 1, 2, 3, 4, 5, 8, 9 } ) world.add( humanId, Human() );
			for ( EntityId hatId   : { 1, 5, 6, 7, 8, 9 } ) world.add( hatId, Hat() );
			
			auto iEntity = view.begin();
			for ( EntityId entityId : { 1, 5, 8, 9 } )
			{
				EXPECT_EQ( entityId, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), iEntity );
		}

#if 0 //NOT YET IMPLEMENTED: N-way intersection
		TEST( View, Intersect3 )
		{
			World world;
			Collection<Human,Hat,Health> collections(world);
			View<Human,Hat,Health> view(world); 

			for ( EntityId humanId  : { 1, 2, 3, 4, 5, 8 } ) world.add( humanId, Human() );
			for ( EntityId hatId    : { 3, 5, 6, 7, 8, 9, 10 } ) world.add( hatId, Hat() );
			for ( EntityId healthId : { 1, 3, 5, 8, 9 } ) world.add( healthId, Health{100.0F} );

			auto iEntity = view.begin();
			for ( EntityId entityId : {  3, 5, 8 } )
			{
				EXPECT_EQ( entityId, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), ++iEntity );
		}


		TEST( View, Intersect4 )
		{
			World world;
			Collection<Human,Hat,Health,Glasses> collections(world);
			View<Human,Hat,Health,Glasses> view(world); 

			for ( EntityId humanId  : { 1, 2, 3, 4, 5, 7, 9 } ) world.add( humanId, Human() );
			for ( EntityId hatId    : { 3, 5, 6, 7, 8, 9 } ) world.add( hatId, Hat() );
			for ( EntityId healthId : { 1, 3, 7, 9, 10 } ) world.add( healthId, Health{100.0F} );
			for ( EntityId glassId  : { 3, 4, 6, 7, 8, 9, 11 } ) world.add( glassId, Glasses() );

			auto iEntity = view.begin();
			for ( EntityId entityId : {  3, 7, 9 } )
			{
				EXPECT_EQ( entityId, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), ++iEntity );
		}
#endif

	} //END: Test
} //END: SubzeroECS