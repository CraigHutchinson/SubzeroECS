#pragma once

#include <cassert>
#include <stdexcept>
#include "UniqueIndex32.hpp"

namespace SubzeroECS {

template<typename... Component>
class Collection;


/**  Holds registrations for Collection instances which can store a Component type
*/
class CollectionRegistry
{
public:
	/** Defines the limit for the number of RegistryCollections that can be instantiated
	@remark A table of this size is created for every component that is registred in any CollectionRegistry instance
	*/
	static const size_t cMaximumRegistryInstances = 4; 

	CollectionRegistry();

	~CollectionRegistry();
	
	/** Find the collection instance for the specified component
	@return Collection instance or nullptr if no collection has been created for the Component type
	*/
	template< typename Component>
	Collection<Component>* find() /*const*/
	{ 
		return getCollection<Component>().instances[registeryId_];
	}

	/** Get the collection instance for the specified component
	@warning Exception will be thrown if a collection instance does not exist
	@return Collection instance
	*/
	template< typename Component>
	Collection<Component>& get()
	{ 
		Collection<Component>* collection = getCollection<Component>().instances[registeryId_];
		if ( collection == nullptr )
		{
			throw std::invalid_argument( 
				std::string("Collection not registered for Component of type ") + typeid(Component).name() );
		}

		return *collection;
	}
	
	/** Set the collection for a component 
	*/
	template< typename Component>
	void set( Collection<Component>* collection )
	{ 
		CollectionInstances<Component>& collections = getCollection<Component>();
		if ( collections.instances[registeryId_] != nullptr )
		{
			throw std::invalid_argument( 
				std::string("Collection already registered for Component of type ") + typeid(Component).name() );
		}

		///Update a list of collection-buffers so we can release instances at desrruction
		collections.next  = bufferListHead_;
		bufferListHead_ = &collections;

		collections.instances[registeryId_] = collection;
	}

	/** Clear the collection for a component 
	*/
	template< typename Component>
	void clear( Collection<Component>* collection )
	{ 
		CollectionInstances<Component>& collections = getCollection<Component>();

		assert( collections.instances[registeryId_] == collection ); //, Sanity check - to remove

		if ( bufferListHead_ == &collections )
		{
			bufferListHead_ = collections.next;
		}
		else
		{
			CollectionInstancesBase* buffer = bufferListHead_;
			while( buffer->next != nullptr && buffer->next != &collections )
			{
				buffer = buffer->next;
			}

			if ( buffer->next == &collections )
			{
				buffer->next = collections.next;
			}
		}

		collections.instances[registeryId_] = nullptr;
	}
private:

	/** Each collection derives from an interface to allow clean up of references at destruction of RegistryCollection 
	*/
	struct CollectionInstancesBase
	{
		CollectionInstancesBase* next; //< Next collection

		CollectionInstancesBase()
		: next(nullptr) 
		{}

		virtual void clear( const size_t index ) = 0;
	};

	template < typename Component >
	struct CollectionInstances : CollectionInstancesBase
	{
		Collection<Component>* instances[cMaximumRegistryInstances];

		/** Make unpopulated instances buffer 
		*/
		CollectionInstances() 
		: instances()
		{
		}

		/** Remove the instance of a registry
		*/
		virtual void clear( const size_t registryId )
		{
			if ( registryId > cMaximumRegistryInstances )
				throw std::exception();
			
			instances[registryId] = nullptr;
		}
	};
	
	/** Collection instance buffer store for all CollectionRegistry instances within the system
	@todo Document why this is static and try and remove?
	*/
	template < typename Component >
	CollectionInstances<Component>& getCollection()
	{
		static CollectionInstances<Component> collections;
		return collections;
	}

private:
	CollectionInstancesBase* bufferListHead_;
	const UniqueIndex32 registeryId_; //< Registry instance index
};


} //END: SubzeroECS
