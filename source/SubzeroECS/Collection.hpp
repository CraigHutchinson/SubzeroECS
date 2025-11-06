#pragma once

#include <map>
#include <vector>

#include "CollectionRegistry.hpp"
#include "EntityId.hpp"

namespace SubzeroECS {

	template< typename... Components >
	class Collection;

	template< typename TComponent>
	class Collection<TComponent>
	{
	public: 
		using Component = TComponent;

		using EntityIdVector = std::vector<EntityId>;
		using ComponentVector = std::vector<Component>;
		using Iterator = typename EntityIdVector::iterator;

	public:
		Collection( CollectionRegistry& registry )
			: registry_(registry)
		{ 
			registry_.registerCollection(this); 
		}

		~Collection()
		{ 
			registry_.unregisterCollection(this);
		}

		Component* create(EntityId entityId, Component&& component) noexcept(false)
		{
			auto iFind = std::lower_bound( ids_.begin(), ids_.end(), entityId );
			if ( iFind != ids_.end() && *iFind == entityId )
			{
				throw std::invalid_argument( "EntityId already has this component type for call to Collection::create()" );
			}

			const size_t index = std::distance( ids_.begin(), iFind );
			ids_.insert( iFind, entityId );	
			components_.insert( components_.begin() + index, std::move(component) );
			return &components_.at( index );
		}

		bool has(EntityId entityId)
		{
			auto iFind = std::lower_bound( ids_.begin(), ids_.end(), entityId );
			return iFind != ids_.end() && *iFind == entityId;
		}

		/** Get pointer to a component of the specified entityId
		@return Component instance of nullptr if no component exists for the entity
		*/
		Component* find(EntityId entityId) noexcept(true)
		{
			const auto iFind = std::lower_bound( ids_.begin(), ids_.end(), entityId );
			return (iFind != ids_.end() && *iFind == entityId)
				? &at(iFind)
				: nullptr;
		}

		/** Get reference to a component of the specified entityId
		@warning Will throw exception if the entityId was not found, use find() if component existance is unknown
		*/
		Component& get(EntityId entityId) noexcept(false)
		{
			const auto iFind = std::lower_bound( ids_.begin(), ids_.end(), entityId );
			if ( iFind == ids_.end() )
				throw std::invalid_argument( "EntityId does not have this component type for call to Collection::get()");
			return at(iFind);
		}

		Component& at( const Iterator& iEntity ) noexcept(true)
		{
			return components_.at( std::distance( ids_.begin(), iEntity ) );
		}

		/** TODO */
		Iterator begin() 
		{ return ids_.begin(); }

		/** TODO */
		Iterator end() 
		{ return ids_.end(); }

	private:
		CollectionRegistry& registry_; //< Registry the collection is attached to
		
		EntityIdVector ids_; //< ECS-entity ids for lookup
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
