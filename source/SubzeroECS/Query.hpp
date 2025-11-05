#pragma once

#include <cassert>

#include "Entity.hpp"

namespace SubzeroECS
{
	/* Queriable object base type for operator overload resolution
	*/
	struct Query
	{};

	/* Is a type queryable */
	template<typename T>
	struct IsQuery
		: std::is_base_of<Query, T> 
	{};
	
	/* Query operator function
	*/
	template<class QueryObject >
	bool query( const Entity& entity, const QueryObject& queryObject = QueryObject() )
	{ 
		static_assert(IsQuery<QueryObject>::value == true, "Not a Query type" );
		return queryObject(entity); 
	}

	/* Query operator '%'
	*/
	template<class QueryObject>
	bool operator % (const Entity& entity, const QueryObject& rhs)
	{
		return query( entity, rhs );
	}

} //END: SubzeroECS