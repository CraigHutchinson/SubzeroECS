#pragma once

#include <map>
#include <utility> //< std::forward

#include "Entity.hpp"
#include "CollectionRegistry.hpp"

namespace SubzeroECS
{
	class World : public CollectionRegistry
	{
	public:
		World()
		: lastEntityId_(EntityId::Invalid)
		{}

		Entity create()
		{
			EntityId entityId = newEntityId();
			return Entity( *this, entityId );
		} 

		template<typename... Components>
		Entity create(Components&&... items)
		{
			EntityId entityId = newEntityId();
			std::tuple<Components*...> comps( CollectionRegistry::get<Components>().create(entityId, std::forward<Components>(items))... );
			return Entity( *this, entityId );
		}

		template<typename Component>
		void add( EntityId entityId, const Component& item )
		{
			CollectionRegistry::get<Component>().create(entityId, item );
		}

		template<typename Component>
		void add( EntityId entityId, Component&& item )
		{
			CollectionRegistry::get<Component>().create(entityId, std::forward<Component>(item));
		}

		template<typename Component>
		bool has( EntityId entityId )
		{ 
			Collection<Component>* collection = CollectionRegistry::find<Component>();
			return (collection != nullptr) && collection->has(entityId); 
		}

		template<typename Component>
		Component* find( EntityId entityId )
		{
			Collection<Component>* collection = CollectionRegistry::find<Component>();
			return (collection != nullptr) ? collection->find(entityId) : nullptr; 
		}

		template<typename Component>
		Component& get( EntityId entityId )
		{ return CollectionRegistry::get<Component>().get(entityId); }

	private:

		EntityId newEntityId()
		{ return lastEntityId_ = lastEntityId_.next(); }

	private:
		EntityId lastEntityId_; //< Id of the last created entity where (0 is invalid/null)
	};


	template< typename Component >
	bool add(World& world, const EntityId entityId, const Component& component)
	{
		return world.add(entityId, component);
	}

	template< typename Component >
	bool add(const Entity& entity, const Component& component)
	{
		return add(entity.world(), entity.id(), component);
	}

	template< typename Component >
	void add(World& world, const EntityId entityId, Component&& component)
	{
		world.add(entityId, std::forward<Component>(component));
	}

	template< typename Component >
	void add(const Entity& entity, Component&& component)
	{
		add(entity.world(), entity.id(), std::forward<Component>(component));
	}

} //END: SubzeroECS
