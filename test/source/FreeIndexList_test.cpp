#include "SubzeroECS/FreeIndexList.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test 
{

	TEST_CLASS(FreeIndexList_Test)
	{
	public:

		TEST_METHOD(FreeIndexList_Init_Empty)
		{
			FreeIndexList list;
			ASSERT_TRUE( list.isEmpty() );
		}

		TEST_METHOD(FreeIndexList_Init_NotFull)
		{
			FreeIndexList list;
			ASSERT_FALSE( list.isFull() );
		}

		TEST_METHOD(FreeIndexList_Alloc1_IsZero)
		{
			FreeIndexList list;
			ASSERT_EQ( list.alloc(), 0u );
		}
		TEST_METHOD(FreeIndexList_AllocN_IsN)
		{
			FreeIndexList list;
			for ( FreeIndexList::Index i = 0; i < 32; ++i )
			{ 
				ASSERT_EQ( list.alloc(), i );
			}
		}

		TEST_METHOD(FreeIndexList_Alloc1_NotEmpty)
		{
			FreeIndexList list;
			list.alloc();
			ASSERT_FALSE( list.isEmpty() );
		}

		TEST_METHOD(FreeIndexList_Alloc1_NotFull)
		{
			FreeIndexList list;
			list.alloc();
			ASSERT_FALSE( list.isFull() );
		}
	};

} //END: Test
} //END: SubzeroECS