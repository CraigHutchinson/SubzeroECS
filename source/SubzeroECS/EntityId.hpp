#pragma once

#include <cstdint>
#include <limits>
#include <compare>
#include <stdexcept>

namespace SubzeroECS
{
	/* Entity unique world Id
	@remarks May be uint16_t to reduce memory usage with trade off for Entity entity count
	*/
	struct EntityId
	{
		std::uint32_t value{};

		constexpr auto operator<=>(const EntityId&) const = default;
		constexpr bool operator==(const EntityId&) const = default;

		constexpr EntityId next() const
		{
			auto nextId = value + 1U;
			if ( nextId == std::numeric_limits<std::uint32_t>::max() )
				throw std::overflow_error( "EntityId::next() overflow" );
			return EntityId{ nextId };
		}

		static const EntityId Invalid;
	};

	inline constexpr EntityId EntityId::Invalid{ std::numeric_limits<std::uint32_t>::max() };

	/** Returns whether the entity id is a null-ent where entityId == Invalid */
	constexpr inline bool isNull( EntityId entityId )
	{ return entityId == EntityId::Invalid; }

} //END: SubzeroECS

