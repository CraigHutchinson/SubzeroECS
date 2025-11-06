#pragma once

#include <compare>

struct Human 
{
};

struct Age
{
	uint32_t age;

	constexpr auto operator<=>(const Age& rhs) const = default;
};

struct Health
{
	float percent;

	constexpr auto operator<=>(const Health& rhs) const = default;
};
struct Glasses
{
};

struct Hat
{
};

struct Shoes
{
	float size;

	constexpr auto operator<=>(const Shoes& rhs) const = default;
};

