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

		TEST( View, Intersect1_Ids )
		{
			World world;
			Collection<Human> collections(world);
			View<Human> view(world); 

			for ( auto humanId : { 1U, 2U, 3U, 4U, 5U, 8U, 9U } ) world.add( EntityId{humanId}, Human() );
			
			auto iEntity = view.begin();
			for ( auto expected : { 1U, 2U, 3U, 4U, 5U, 8U, 9U } )
			{
				EXPECT_EQ( EntityId{expected}, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), iEntity );
		}


		TEST( View, Intersect1_Has )
		{
			World world;
			Collection<Human> collections(world);
			View<Human> view(world); 

			for ( auto humanId : { 1U, 2U, 3U, 4U, 5U, 8U, 9U } ) world.add( EntityId{humanId}, Human() );
			
			auto iEntity = view.begin();
			for ( auto expected : { 1U, 2U, 3U, 4U, 5U, 8U, 9U } )
			{
				(void)expected; //unused
				EXPECT_TRUE( iEntity.has<Human>() );
				// Note: Only valid for 'optional' testing as this is a compile-time error here
				// EXPECT_FALSE( iEntity.has<Health>() );
				++iEntity;
			}
		}

		TEST( View, Intersect2_Ids )
		{
			World world;
			Collection<Human,Hat> collections(world);
			View<Human,Hat> view(world); 

			for ( auto humanId : { 1U, 2U, 3U, 4U, 5U, 8U, 9U } ) world.add( EntityId{humanId}, Human() );
			for ( auto hatId   : { 1U, 5U, 6U, 7U, 8U, 9U } ) world.add( EntityId{hatId}, Hat() );
			
			auto iEntity = view.begin();
			for ( auto expected : { 1U, 5U, 8U, 9U } )
			{
				EXPECT_EQ( EntityId{expected}, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), iEntity );
		}

		TEST( View, Intersect2_Values )
		{
			World world;
			Collection<Health,Shoes> collections(world);
			View<Health,Shoes> view(world);

			for ( auto id : { 1U, 2U, 3U, 4U, 5U, 8U, 9U } ) world.add( EntityId{id}, Health{id*2.0F} );
			for ( auto id : { 1U, 5U, 6U, 7U, 8U, 9U } ) world.add( EntityId{id}, Shoes{id*3.0F} );

			auto iEntity = view.begin();
			for ( auto expected : { 1U, 5U, 8U, 9U } )
			{
				EXPECT_EQ( iEntity.get<Health>(), Health{expected*2.0F} );
				EXPECT_EQ( iEntity.get<Shoes>(), Shoes{expected*3.0F} );
				++iEntity;
			}
		}

		TEST( View, Intersect3_Ids )
		{
			World world;
			Collection<Human,Hat,Health> collections(world);
			View<Human,Hat,Health> view(world); 

			for ( auto humanId  : { 1U, 2U, 3U, 4U, 5U, 8U } ) world.add( EntityId{humanId}, Human() );
			for ( auto hatId    : { 3U, 5U, 6U, 7U, 8U, 9U, 10U } ) world.add( EntityId{hatId}, Hat() );
			for ( auto healthId : { 1U, 3U, 5U, 8U, 9U } ) world.add( EntityId{healthId}, Health{100.0F} );

			auto iEntity = view.begin();
			for ( auto expected : {  3U, 5U, 8U } )
			{
				EXPECT_EQ( EntityId{expected}, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), iEntity );
		}

		TEST( View, Intersect3_Values )
		{
			World world;
			Collection<Age,Health,Shoes> collections(world);
			View<Age,Health,Shoes> view(world);

			for ( auto id : { 1U, 2U, 3U, 4U, 5U, 8U } ) world.add( EntityId{id}, Age{id} );
			for ( auto id : { 3U, 5U, 6U, 7U, 8U, 9U, 10U  } ) world.add( EntityId{id}, Health{id*2.0F} );
			for ( auto id : { 1U, 3U, 5U, 8U, 9U } ) world.add( EntityId{id}, Shoes{id*3.0F} );

			auto iEntity = view.begin();
			for ( auto expected : { 3U, 5U, 8U } )
			{
				EXPECT_EQ( iEntity.get<Age>(), Age{expected} );
				EXPECT_EQ( iEntity.get<Health>(), Health{expected*2.0F} );
				EXPECT_EQ( iEntity.get<Shoes>(), Shoes{expected*3.0F} );
				++iEntity;
			}
		}

		TEST( View, Intersect4_Ids )
		{
			World world;
			Collection<Human,Hat,Health,Glasses> collections(world);
			View<Human,Hat,Health,Glasses> view(world); 

			for ( auto humanId  : { 1U, 2U, 3U, 4U, 5U, 7U, 9U } ) world.add( EntityId{humanId}, Human() );
			for ( auto hatId    : { 3U, 5U, 6U, 7U, 8U, 9U } ) world.add( EntityId{hatId}, Hat() );
			for ( auto healthId : { 1U, 3U, 7U, 9U, 10U } ) world.add( EntityId{healthId}, Health{100.0F} );
			for ( auto glassId  : { 3U, 4U, 6U, 7U, 8U, 9U, 11U } ) world.add( EntityId{glassId}, Glasses() );

			auto iEntity = view.begin();
			for ( auto expected : {  3U, 7U, 9U } )
			{
				EXPECT_EQ( EntityId{expected}, *iEntity );
				++iEntity;
			}
			EXPECT_EQ( view.end(), iEntity );
		}

	} //END: Test
} //END: SubzeroECS