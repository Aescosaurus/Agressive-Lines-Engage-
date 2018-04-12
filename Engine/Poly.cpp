#include "Poly.h"

Poly::Poly( std::initializer_list<Vec2> vList,Color c )
	:
	vertices( vList ),
	c( c )
{
}

void Poly::Draw( Graphics& gfx ) const
{
	for( auto i = vertices.begin(),end = vertices.end() - 1; i != end; ++i )
	{
		const Vec2 pos1 = i->Rotation( angle ) + pos;
		const Vec2 pos2 = ( i + 1 )->Rotation( angle ) + pos;
		gfx.DrawLine( int( pos1.x ),int( pos1.y ),int( pos2.x ),int( pos2.y ),c );
	}
	const Vec2 pos1 = vertices.back().Rotation( angle ) + pos;
	const Vec2 pos2 = vertices.front().Rotation( angle ) + pos;
	gfx.DrawLine( int( pos1.x ),int( pos1.y ),int( pos2.x ),int( pos2.y ),c );
}

void Poly::MoveTo( const Vec2& pos )
{
	this->pos = pos;
}

void Poly::Rotate( float amount )
{
	angle += amount;
}

void Poly::SetRotation( float amount )
{
	angle = amount;
}

void Poly::SetColor( Color c )
{
	this->c = c;
}

Color Poly::GetColor() const
{
	return( c );
}
