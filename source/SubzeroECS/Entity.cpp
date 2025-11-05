
#include "Entity.hpp"

#include "World.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "Collection.hpp" //<NOTE: Needed for majority of tests for component storage etc!
#include "Has.hpp"


namespace SubzeroECS 
{

	Entity::Entity() 
		: world_(nullptr)
		, id_(0)
	{}

	Entity::Entity(World& world, EntityId id) 
		: world_(&world)
		, id_(id)
	{}

	
	World& Entity::world() const
	{
		if ( world_ == nullptr )
			throw std::runtime_error( "Entity::world_ == nullptr" );
		return *world_;
	}


} //END: SubzeroECS