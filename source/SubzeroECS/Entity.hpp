#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>

#include "EntityId.hpp"
#define CPP11 1

namespace SubzeroECS
{
	//Forward declaration
	class World;

	class Entity
	{
	public:
		/* Default consturctor initialises a null hentity reference */
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
		
		template< typename Component >
		constexpr bool has() const { 
			return world.has<Component>(id()); 
		}

		template< typename Component >
		constexpr Component& get() const {
			return world.get<Component>(id()); 
		}

		template< typename Component >
		constexpr Component* find() const {
            return world.find<Component>(id()); 
		}

		template< typename Component >
		constexpr void add( const Component& component ) const {
            world.add(id(), component ); 
		}

#if CPP11
		template< typename Component >
		constexpr void add( Component&& component ) const { 
            world.add(id(), std::forward(component) ); 
		}
#endif

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
