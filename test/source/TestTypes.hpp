#pragma once

#include <compare>

struct Human 
{
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

