#include "Rect.h"

Rect::Rect( float left_in,float right_in,float top_in,float bottom_in )
	:
	left( left_in ),
	right( right_in ),
	top( top_in ),
	bottom( bottom_in )
{
}

Rect::Rect( const Vec2& topLeft,const Vec2& bottomRight )
	:
	Rect( topLeft.x,bottomRight.x,topLeft.y,bottomRight.y )
{
}

Rect::Rect( const Vec2& topLeft,float width,float height )
	:
	Rect( topLeft,topLeft + Vec2( width,height ) )
{
}

bool Rect::IsOverlappingWith( const Rect& other ) const
{
	return right > other.left && left < other.right
		&& bottom > other.top && top < other.bottom;
}

bool Rect::IsContainedBy( const Rect& other ) const
{
	return left >= other.left && right <= other.right &&
		top >= other.top && bottom <= other.bottom;
}

bool Rect::Contains( const Vec2& pos ) const
{
	return( Rect{ pos,1.0f,1.0f }.IsContainedBy( *this ) );
}

void Rect::MoveTo( const Vec2& point )
{
	right += point.x - left;
	bottom += point.y - top;
	left = point.x;
	top = point.y;
}

void Rect::MoveBy( const Vec2& amount )
{
	left += amount.x;
	right += amount.x;
	top += amount.y;
	bottom += amount.y;
}

Rect Rect::FromCenter( const Vec2& center,float halfWidth,float halfHeight )
{
	const Vec2 half( halfWidth,halfHeight );
	return Rect( center - half,center + half );
}

Rect Rect::GetExpanded( float offset ) const
{
	return Rect( left - offset,right + offset,top - offset,bottom + offset );
}

Vec2 Rect::GetCenter() const
{
	return Vec2( ( left + right ) / 2.0f,( top + bottom ) / 2.0f );
}

float Rect::GetWidth() const
{
	return right - left;
}

float Rect::GetHeight() const
{
	return bottom - top;
}
