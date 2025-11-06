#pragma once

#include <cstdint>
#include <limits>
#include <compare>

namespace SubzeroECS
{
	/* Entity unique world Id
	@remarks May be uint16_t to reduce memory usage with trade off for Entity entity count
	*/
	struct EntityId
	{
		std::uint32_t value{};

		constexpr EntityId(std::uint32_t v = 0) : value(v) {}

		// Explicit conversion to underlying type
		[[nodiscard]] constexpr std::uint32_t get() const { return value; }
		
		// Explicit conversion operator
		[[nodiscard]] constexpr explicit operator std::uint32_t() const { return value; }

		// C++20 three-way comparison (spaceship operator) - generates all comparison operators
		constexpr auto operator<=>(const EntityId&) const = default;
		constexpr bool operator==(const EntityId&) const = default;

		// Increment/Decrement operators
		constexpr EntityId& operator++() { ++value; return *this; }
		constexpr EntityId operator++(int) { EntityId tmp = *this; ++value; return tmp; }
		constexpr EntityId& operator--() { --value; return *this; }
		constexpr EntityId operator--(int) { EntityId tmp = *this; --value; return tmp; }

		// Arithmetic operators
		constexpr EntityId operator+(std::uint32_t rhs) const { return EntityId{ value + rhs }; }
		constexpr EntityId operator-(std::uint32_t rhs) const { return EntityId{ value - rhs }; }
		constexpr EntityId& operator+=(std::uint32_t rhs) { value += rhs; return *this; }
		constexpr EntityId& operator-=(std::uint32_t rhs) { value -= rhs; return *this; }

		static const EntityId Invalid;
	};

	inline constexpr EntityId EntityId::Invalid{ std::numeric_limits<std::uint32_t>::max() };

	const EntityId cInvalid_EntityId = EntityId::Invalid;

	/** Returns whether the entity id is a null-ent where entityId == Invalid */
	constexpr inline bool isNull( EntityId entityId )
	{ return entityId == EntityId::Invalid; }

} //END: SubzeroECS

