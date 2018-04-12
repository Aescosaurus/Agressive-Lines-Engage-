#include "Vec2.h"
#include <cmath>

Vec2::Vec2( float x_in,float y_in )
	:
	x( x_in ),
	y( y_in )
{
}

Vec2 Vec2::operator+( const Vec2& rhs ) const
{
	return Vec2( x + rhs.x,y + rhs.y );
}

Vec2& Vec2::operator+=( const Vec2& rhs )
{
	return *this = *this + rhs;
}

Vec2 Vec2::operator*( float rhs ) const
{
	return Vec2( x * rhs,y * rhs );
}

Vec2& Vec2::operator*=( float rhs )
{
	return *this = *this * rhs;
}

Vec2 Vec2::operator-( const Vec2& rhs ) const
{
	return Vec2( x - rhs.x,y - rhs.y );
}

Vec2& Vec2::operator-=( const Vec2& rhs )
{
	return *this = *this - rhs;
}

Vec2 Vec2::operator/( float rhs ) const
{
	return Vec2{ x / rhs,y / rhs };
}

Vec2 & Vec2::operator/=( float rhs )
{
	*this = ( *this ) / rhs;
	return *this;
}

bool Vec2::operator==( const Vec2& rhs ) const
{
	return( x == rhs.x && y == rhs.y );
}

bool Vec2::operator!=( const Vec2& rhs ) const
{
	return !( *this == rhs );
}

int Vec2::operator[]( char letter )
{
	if( letter == 'x' )
	{
		return( int( x ) );
	}
	else if( letter == 'y' )
	{
		return( int( y ) );
	}
	else
	{
		return( int( x + y ) );
	}
}

float Vec2::GetLength() const
{
	return std::sqrt( GetLengthSq() );
}

float Vec2::GetLengthSq() const
{
	return x * x + y * y;
}

float Vec2::GetAngle() const
{
	return( atan2( y,x ) );
}

Vec2& Vec2::Normalize()
{
	return *this = GetNormalized();
}

Vec2 Vec2::GetNormalized() const
{
	const float len = GetLength();
	if( len != 0.0f )
	{
		return *this * ( 1.0f / len );
	}
	return *this;
}

Vec2 Vec2::Rotation( const float angle ) const
{
	Vec2 result;
	result.x = x * cosf( angle ) - y * sinf( angle );
	result.y = x * sinf( angle ) + y * cosf( angle );
	return result;
}

Vec2 Vec2::Up()
{
	return Vec2{ 0.0f,-1.0f };
}

Vec2 Vec2::Down()
{
	return Vec2{ 0.0f,1.0f };
}

Vec2 Vec2::Left()
{
	return Vec2{ -1.0f,0.0f };
}

Vec2 Vec2::Right()
{
	return Vec2{ 1.0f,0.0f };
}
