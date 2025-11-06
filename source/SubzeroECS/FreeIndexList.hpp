#pragma once

#include <bit>
#include <bitset>
#include <cstdint>
#include <stdexcept>

namespace SubzeroECS {


/** Handles a list of free indices as a bit-mask
@remark Uses a 32bit mask and will throw exception when the limit is reached
*/
class FreeIndexList
{
public:
	using Index = std::uint_fast16_t; //< Freelist return value (Index type of at least 16 bits)
	using AllocFailed = std::overflow_error;
	
	static constexpr size_t Capacity = 32; //< Number of available indices
	
public:

	FreeIndexList();

	/** Alocate a new entry from the free-store 
	@notice If the free-store is empty then throws AllocFailed 
	*/
	Index alloc();

	/** Adds an entry to the free-store 
	*/
	void free( Index index );

	/** Check if there are no allocated entries
	@return True if there are no allocated indices and false if any exist 
	*/
	bool isEmpty() const;

	/** Check if the free-list instance has been reached 
	@notice The next call to alloc() will throw std::overflow_error(...) 
	@return True if the internal limit is reached (i.e. 8/16/32) and true if less
	than the limit and/or 0
	*/
	bool isFull() const;

	/** Get the number of currently allocated indices
	*/
	size_t count() const;

private:
	std::bitset<Capacity> freeMask_; //< Bitset where 0 = free, 1 = allocated
};

} //END: SubzeroECS
