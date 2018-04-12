#pragma once

#include <random>

class Random
{
public:
	Random();
	int NextInt( int min,int max );
	float NextFloat( float min,float max );
private:
	std::mt19937 rng;
};