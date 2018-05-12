#include "Recharger.h"

Recharger::Recharger( const Vec2& pos,bool active )
	:
	pos( pos ),
	hitbox( pos,ss * 2.0f,ss * 2.0f ),
	active( active )
{
}

void Recharger::Update( Powerup* pPowerup,
	bool playerLostHealth,float dt )
{
	if( active )
	{
		if( !playerLostHealth )
		{
			if( !pPowerup->GetRect()
				.IsContainedBy( Graphics
				::GetScreenRect() ) )
			{
				pPowerup->Reset( pos );
			}

			// Has to come after pPowerup->Reset.
			Reset( { 9999.0f,9999.0f } );
		}

		shape.Rotate( rotSpeed * float( speedMultiplier ) * dt );

		shape.MoveTo( pos );
		hitbox.MoveTo( pos - Vec2{ ss,ss } );
	}
}

void Recharger::Draw( Graphics& gfx ) const
{
	if( active )
	{
		shape.Draw( gfx );
		shape.Draw( gfx );

// #if !DRAW_RELEASE_STUFF
// 		gfx.DrawHitbox( hitbox );
// #endif
	}
}

void Recharger::Reset( const Vec2& pos )
{
	this->pos = pos;
	active = true;
	do
	{
		speedMultiplier = rng.NextFloat( -rotSpeed,rotSpeed );
	} while( speedMultiplier > -1.0f && speedMultiplier < 1.0f );
}

const Rect& Recharger::GetRect() const
{
	return hitbox;
}
