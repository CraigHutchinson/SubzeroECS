#include "CollectionRegistry.hpp"
#include "Collection.hpp"


namespace SubzeroECS
{	

	CollectionRegistry::CollectionRegistry() 
	: bufferListHead_(nullptr)
	{
	}

	CollectionRegistry::~CollectionRegistry()
	{
		CollectionInstancesBase* buffer = bufferListHead_;
		while( buffer )
		{
			buffer->unregisterCollection( registeryId_ );
			buffer = buffer->next;
		}
	}
} //END: SubzeroECS



