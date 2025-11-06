#pragma once

#include <stdexcept>
#include "FreeIndexList32.hpp"

namespace SubzeroECS {

	/** Stores a new incremental counter value when instantiated. 
	@remark When a counter is released its index will be reused
	@notice This implementation is limited to 16 or 32 indices
	*/
	class UniqueIndex32
	{ 
	public:
		using Index = FreeIndexList32::Index; //< Index/FreeList storage type of atleast 16bits
	
		static constexpr size_t Capacity = FreeIndexList32::Capacity; //< Number of available indices
		
	public:
		/** Aquires the next available lowest-value index
		@warning Indexes are reused by subsequent instantiations when they are freed
		@throw FreeIndexList32::AllocFailed if internal limit is reached
		@todo Debug: feature not to reuse indices for subsequent instantiations instead
		*/
		UniqueIndex32()
		: index_( freeList_s.alloc() )
		{}

		~UniqueIndex32() 
		{ 
			freeList_s.free(index_);
		}

		/** Access the internal index */
		operator Index () const
		{ return index_; };

		bool operator == ( Index index ) const 
		{ return index_ == index; }
	
	private:
		const Index index_; ///< Index value
		static FreeIndexList32 freeList_s; ///< Mask of available indices
	};

} //END: SubzeroECS

