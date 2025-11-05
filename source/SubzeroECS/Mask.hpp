#pragma once

#include <cstdint>

namespace SubzeroECS 
{

/** Bitmask 
*/
class Mask
{
public:
	typedef std::uint_fast16_t Index; //< Index/FreeList storage type of atleast 16bits
	typedef std::uint32_t Bits; //< Bits type
	
	static const Bits cNone = static_cast<Bits>(0U); //< No bits set
	static const Bits cAll = ~cNone; //< All bit set
	
	/** Returns index to the only set bit in a mask 
	@paran oneBit Bit-mask with only a single bit set
	@warning If multiple bits are set on oneBit the result is undefined
	@remark Uses Multiply-DeBruijn-Bit-Position
	@returns Index to the only set bit within oneBit or undefined if multiple bits are set
	*/
	static Index indexOfOnlyBit( Bits oneBit );

	/** @return Number of bits in the mask  type
	*/
	constexpr static size_t capacity()
	{ return sizeof(Bits) * 8u; }

public:
	Mask( Bits bits = cNone ) : mask_(bits) {}

	/** Finds the first set bit and clears it
	@return The index to the bit that was cleared 
	*/
	Index clearFirstSet();

	/** Set the specified bit index to 1/true
	@todo Make thread safe version!?
	*/
	void set( Index index )
	{ mask_ |= (static_cast<Bits>(1U) << index); }

	//void set( Index index, bool state )
	//{ mask_ ^= (-mask_ ^ mask_) & (static_cast<Bits>(1U) << index); }

	/** Set the specified bit index to 0/false
	@todo Make thread safe version!?
	*/
	void clear( Index index )
	{ mask_ &= ~(static_cast<Bits>(1U) << index); }

	/** Flips the specified bit index from 0/false to 1/true and visa-versa
	@todo Make thread safe version!?
	*/
	//void toggle( Index index )
	//{ mask_ ^= (static_cast<Bits>(1U) << index); }

	/** Check if the specified index is set to 1/true 
	*/
	bool at( Index index ) const
	{ return (mask_ & (static_cast<Bits>(1U) << index)) != 0U;  }

	bool operator == ( Bits rhs ) const
	{ return mask_ == rhs; }

	bool operator != ( Bits rhs ) const
	{ return mask_ == rhs; }

	Bits bits() const
	{ return mask_; }


private:
	Bits mask_; //< Mask value
};

} //END: SubzeroECS
