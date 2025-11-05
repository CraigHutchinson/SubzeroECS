#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <utility> //< std::forward

#include "EntityId.hpp"

namespace SubzeroECS
{
	//Forward declaration
	class World;

	class Entity
	{
	public:
		/* Default consturctor initialises a null ECS-entity reference */
		Entity();

		/* Create aentity for the specified world and Id */
		Entity(World& world, EntityId id) ;
		
		/** Get entitys world */
		World& world() const;

		/** Get entity handle */
		constexpr EntityId id() const {
			return id_;
		}	

		/** Returns whether the entity id is a null-ent where id()==0 */
		constexpr bool isNull() const { 
			return SubzeroECS::isNull( id_ ); 
		}
		
		template< typename TComponent >
		constexpr bool has() const { 
			return world().template has<TComponent>(id()); 
		}

		template< typename TComponent >
		constexpr TComponent& get() const {
			return world().template get<TComponent>(id()); 
		}

		template< typename TComponent >
		constexpr TComponent* find() const {
            return world().template find<TComponent>(id()); 
		}

		template< typename TComponent >
		constexpr void add( const TComponent& component ) const {
            world().add(id(), component ); 
		}

		template< typename TComponent >
		constexpr void add( TComponent&& component ) const { 
            world().add(id(), std::forward<TComponent>(component) ); 
		}

	private:
		/** Throws if this is null-ent */
		void throwIfIsNull()
		{
			if ( isNull() )
				throw std::runtime_error( "add() on null-ent" );
		}

	protected:
		World* world_;
		EntityId id_;
	};

	/** Equality to test entityId and world are equal
	@returns True if (id()==0 && rhs.id()==0) || id()==rhs.id() && (&world()==&rhs.world())
	*/
	inline bool operator ==( const Entity& lhs, const Entity& rhs)
	{
		return (lhs.id() == rhs.id()) 
			&& ((lhs.id() == 0U) || (&lhs.world() == &rhs.world()));
	}

	/** @see operator==(const Entity&,const Entity&)
	*/
	inline bool operator !=( const Entity& lhs, const Entity& rhs)
	{
		return !(lhs == rhs);
	}

} //END: SubzeroECS
