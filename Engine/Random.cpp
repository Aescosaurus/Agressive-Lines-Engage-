#include "Random.h"

Random::Random()
	:
	rng( std::random_device{}( ) )
{
}

int Random::NextInt( int min,int max )
{
	if( min > max ) std::swap( min,max );
	std::uniform_int_distribution<int> dist( min,max );
	return dist( rng );
}

float Random::NextFloat( float min,float max )
{
	if( min > max ) std::swap( min,max );
	std::uniform_real_distribution<float> dist( min,max );
	return dist( rng );
}
