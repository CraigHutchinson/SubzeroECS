#include "SubzeroECS/GlobalUniqueIndex.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test 
{	

	TEST_CLASS(GlobalUniqueIndex_Test)
	{
	public:

		TEST_METHOD(GlobalUniqueIndex_First_Is0)
		{
			GlobalUniqueIndex id;
			ASSERT_EQ( (uint32_t)id, 0u );
		}

		TEST_METHOD(GlobalUniqueIndex_OnlyInstance_Is0)
		{
			{
				GlobalUniqueIndex id1;
				ASSERT_EQ( (uint32_t)id1, 0u );
			}
			{
				GlobalUniqueIndex id2;
				ASSERT_EQ( (uint32_t)id2, 0u );
			}
		}

		TEST_METHOD(GlobalUniqueIndex_Second_Is1)
		{
			GlobalUniqueIndex id1;
			GlobalUniqueIndex id2;
			ASSERT_EQ( (uint32_t)id2, 1u );
		}

		TEST_METHOD(GlobalUniqueIndex_OnlySecondInstance_Is1)
		{
			GlobalUniqueIndex id1;
			{
				GlobalUniqueIndex id2; 
				ASSERT_EQ( (uint32_t)id2, 1u );
			}
			{
				GlobalUniqueIndex id2;
				ASSERT_EQ( (uint32_t)id2, 1u );
			}
		}
	};

} //END: Test
} //END: SubzeroECS