
#include "Mask.hpp"

namespace SubzeroECS 
{

	Mask::Index Mask::indexOfOnlyBit( Bits oneBit )
	{
		static constexpr Index cMultiplyDeBruijnBitPosition[32] = 
		{
			0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
			31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		//125,613,361
		//2 3 5 7 11 13 17 19 23 29 31
		return cMultiplyDeBruijnBitPosition[(static_cast<uint32_t>(oneBit) * 0x077CB531U) >> 27U ];
	}


	///<TODO: make thread safe... CAS increment would suit
	Mask::Index Mask::clearFirstSet()
	{ 
		//TODO: use fsb instruction!
		Bits freeBit = (mask_ & -static_cast<signed>(mask_)); //< Get the first set bit as a mask
		mask_ ^= freeBit; //< Remove free index by unsetting the bit
		return indexOfOnlyBit( freeBit );
	}

} //END: SubzeroECS

