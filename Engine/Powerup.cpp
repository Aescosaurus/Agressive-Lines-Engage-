#include "Powerup.h"
#include "META.h"

Powerup::Powerup( const Vec2& pos,bool active )
	:
	pos( pos ),
	hitbox( pos,s1 * 2.0f,s1 * 2.0f ),
	active( active )
{
}

void Powerup::Update( float dt )
{
	if( active )
	{
		p.Rotate( rotSpeed * float( speedMultiplier ) * dt );

		p.MoveTo( pos );
		hitbox.MoveTo( pos - Vec2{ s1,s1 } );
	}
}

void Powerup::Draw( Graphics& gfx ) const
{
	if( active )
	{
		p.Draw( gfx );
		p.Draw( gfx );

#if !releaseMode
		gfx.DrawHitbox( hitbox );
#endif
	}
}

void Powerup::Reset( const Vec2& pos )
{
	this->pos = pos;
	active = true;
	do
	{
		speedMultiplier = rng.NextFloat( -rotSpeed,rotSpeed );
	}
	while( speedMultiplier > -1.0f && speedMultiplier < 1.0f );
}

const Rect& Powerup::GetRect() const
{
	return hitbox;
}
