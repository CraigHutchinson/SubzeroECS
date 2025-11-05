#pragma once

#include <stdexcept>
#include "FreeIndexList.hpp"

namespace SubzeroECS {

	/** Stores a new incremental counter value when instantiated. 
	@remark When a counter is released its index will be reused
	@notice This implementation is limited to 16 or 32 indices
	*/
	class GlobalUniqueIndex
	{ 
	public:
		typedef uint_fast16_t Index; //< Index/FreeList storage type of atleast 16bits
	
	public:
		/** Constructor takes the next available unique index reusing any available indices 
		@notice Indexes are reused by subsequent instantiations when they are freed
		@todo Bias/cycle the index for subsequent instantiations instead of always using the lowest index
		@throw FreeIndexList::AllocFailed if internal limit is reached
		*/
		GlobalUniqueIndex()
		: counter_( freeList_s.alloc() )
		{}

		~GlobalUniqueIndex() 
		{ 
			freeList_s.free(counter_);
		}

		/** Access the internal index */
		operator Index () const
		{ return counter_; };

		bool operator == ( Index index ) const 
		{ return counter_ == index; }
	
	private:
		const Index counter_; ///< The counter value
		static FreeIndexList freeList_s; ///< Mask of available indices
	};

} //END: SubzeroECS

