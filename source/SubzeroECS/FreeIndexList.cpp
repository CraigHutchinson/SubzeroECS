
#include "FreeIndexList.hpp"

namespace SubzeroECS 
{

	FreeIndexList::FreeIndexList() 
	: freeMask_()
	{
		// Initialize with all bits clear (all indices are free)
		// Default constructor already zeros the bitset
	}

	FreeIndexList::Index FreeIndexList::alloc()
	{ 
		if (freeMask_.all())
			throw AllocFailed("Exceeds limit of UniqueIndex instances");
		
		// Find the first clear bit (first free index) using C++20 countr_one
		// Convert to unsigned long to use with std::countr_one
		auto bits = freeMask_.to_ulong();
		Index index = static_cast<Index>(std::countr_one(bits));
		
		// Mark this index as allocated (set the bit)
		freeMask_.set(index);
		
		return index;
	}

	void FreeIndexList::free(Index index)
	{
		// Mark this index as free (clear the bit)
		freeMask_.reset(index); 
	}

	bool FreeIndexList::isEmpty() const
	{ 
		// Empty means no indices are allocated (no bits set)
		return freeMask_.none(); 
	}

	bool FreeIndexList::isFull() const
	{ 
		// Full means all indices are allocated (all bits set)
		return freeMask_.all(); 
	}

	size_t FreeIndexList::count() const
	{
		// Return the number of allocated indices (direct count)
		return freeMask_.count();
	}

} //END: SubzeroECS