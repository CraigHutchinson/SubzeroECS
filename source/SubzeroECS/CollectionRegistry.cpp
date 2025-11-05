#include "CollectionRegistry.hpp"
#include "Collection.hpp"


namespace SubzeroECS
{	

	CollectionRegistry::CollectionRegistry() 
	: bufferListHead_(nullptr)
	{
		if ( registeryId_ >= cMaximumRegistryInstances )
			throw std::overflow_error( "Too many RegistryCollection instances");
	}

	CollectionRegistry::~CollectionRegistry()
	{
		CollectionInstancesBase* buffer = bufferListHead_;
		while( buffer )
		{
			buffer->clear( registeryId_ );
			buffer = buffer->next;
		}
	}
} //END: SubzeroECS



