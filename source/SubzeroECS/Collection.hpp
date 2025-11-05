#pragma once

#include <map>
#include <vector>

#include "CollectionRegistry.hpp"
#include "EntityId.hpp"

namespace SubzeroECS {

	template< typename... Components >
	class Collection;

	template< typename Component>
	class Collection<Component>
	{
	public: 
		typedef Component Component;
		
		typedef std::vector<EntityId> EntityIdVector;
		typedef std::vector<Component> ComponentVector;
		typedef typename EntityIdVector::iterator Iterator;

	public:
		Collection( CollectionRegistry& registry )
			: registry_(registry)
		{ 
			registry_.set(this); 
		}

		~Collection()
		{ 
			registry_.clear(this);
		}

		Component* create(EntityId entityId, Component&& component)
		{
			//TODO: THrow if already exists!
			//TODO: Very temp!
			return &components_.emplace( entityId, std::forward(component) ).first->second;
		}

		bool has(EntityId entityId)
		{
			return components_.find(entityId) != components_.end();			
		}

		/** Get pointer to a component of the specified entityId
		@return Component instance of nullptr if no component exists for the entity
		*/
		Component* find(EntityId entityId)
		{
			const auto iFind = components_.find(entityId);
			return iFind != components_.end() ? &iFind->second : nullptr;			
		}

		/** Get reference to a component of the specified entityId
		@warning Will throw exception if the entityId was not found, use find() if component existance is unknown
		*/
		Component& get(EntityId entityId)
		{
			const auto iFind = components_.find(entityId);
			if ( iFind == components_.end() )
				throw std::invalid_argument( "EntityId does not have this component type for call to Collection::get()");
			return iFind->second; //< Will throw if not valid itertor!
		}

		Component& at( const Iterator& iEntity )
		{
			return iEntity->second;
		}

		/** TODO */
		Iterator begin() 
		{ return components_.begin(); }

		/** TODO */
		Iterator end() 
		{ return components_.end(); }

	private:
		CollectionRegistry& registry_; //< Registry the collection is attached to
		
		EntityIdVector ids_; //< SubzeroECSty ids for lookup
		ComponentVector components_; //< Comoonent data
	};


	/** Multiple Component collections with lifetime maintained by a single object
	 * @TODO This could be specialised to provide better combined-lifetime for related components?  
	 * @tparam Components  List of Component types, a Collection<Compnent[X]> will be created for each typename
	 */
	template< typename... Components >
	class Collection
	{
	public:
		//NOTE: C++11 std::make_tuple
		Collection(CollectionRegistry& registry)
			: collections_((sizeof(Components), registry)...)
		{
		}

		//NOTE: C++14 std::get<>
		template< typename Component>
		Collection<Component>& get()
		{
			return std::get<Collection<Component>>(collections_);
		}

	private:
		typedef std::tuple< Collection<Components>... > CollectionTuple;
		CollectionTuple collections_;
	};

} //END: SubzeroECS
