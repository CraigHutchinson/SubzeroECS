#pragma once

#include <cassert>

#include "Entity.hpp"
#include "World.hpp"

namespace SubzeroECS
{

    template< typename Component >
    constexpr bool has(const Entity& entity)
    { return has<Component>(entity.world(), entity.id()); }

    template< typename Component >
    constexpr bool has( World& world, EntityId entityId)
    { return world.has<Component>(entityId); }

    template< typename Component >
    Component* find( const Entity& entity)
    { return find<Component>(entity.world(), entity.id()); }

    template< typename Component >
    constexpr Component* find( World& world, EntityId entityId)
    { return world.find<Component>(entityId); }

    template< typename Component >
    constexpr Component& get( const Entity& entity)
    { return get<Component>( entity.world(), entity.id() ); }

    template< typename Component >
    constexpr Component& get( World& world, EntityId entityId )
    { return world.get<Component>(entityId); }

} //END: SubzeroECS