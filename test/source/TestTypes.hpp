#pragma once


struct Human 
{
	Human(){}
};

struct Health
{
	Health(float percent) 
		: percent(percent)
	{}

	float percent;
};
struct Glasses
{
	Glasses() {}
};

struct Hat
{
	Hat() {} 
};

struct Shoes
{
	Shoes(float size)
		: size(size)
	{}

	float size;
};

