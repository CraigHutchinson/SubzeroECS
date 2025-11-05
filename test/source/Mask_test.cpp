#include "SubzeroECS/Mask.hpp"

#include <gtest/gtest.h>

namespace SubzeroECS {
namespace Test 
{

	TEST(Mask,Init_Empty)
	{
		Mask mask;
		ASSERT_EQ( Mask::cNone, mask.bits() );
	}

	TEST(Mask,Init_Construct)
	{
		const Mask::Bits bits = 0x19473628;
		Mask mask(bits);
		ASSERT_EQ( bits, mask.bits() );
	}

	TEST(Mask,Init_All)
	{
		Mask mask(Mask::cAll);
		ASSERT_EQ( Mask::cAll, mask.bits() );
	}

	TEST(Mask,at)
	{
		Mask::Bits bits = 0x32178546;
		for ( Mask::Index i =0; i < Mask::capacity(); ++i )
		{
			ASSERT_EQ( (bits & (1u<<i)) != 0,   Mask(bits).at(i) );
		}
	}

	TEST(Mask,set)
	{
		Mask mask;
		mask.set(5);
		ASSERT_TRUE( mask.at(5) );
	}

	TEST(Mask,clear)
	{
		Mask mask(Mask::cAll);
		mask.clear(5);
		ASSERT_FALSE( mask.at(5) );
	}

	TEST(Mask,indexOfOnlyBit)
	{
		for ( Mask::Index i =0; i < Mask::capacity(); ++i )
		{
			ASSERT_EQ( i, Mask::indexOfOnlyBit(1<<i) );
		}
	}

} //END: Test
} //END: SubzeroECS

