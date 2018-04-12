#include "Joystick.h"

Joystick::Joystick( const Vec2& pos )
	:
	pos( pos ),
	basePos( pos ),
	hitbox( pos - Vec2{ size,size },pos + Vec2{ size,size } )
{
}

void Joystick::Update( const Mouse& ms )
{
	float minimizeAmount = 1.0f;
	if( ms.LeftIsPressed() )
	{
		const Vec2 msPos = Vec2{ float( ms.GetPosX() ),
			float( ms.GetPosY() ) };
		const Rect msRect = { msPos,msPos + Vec2{ 2.0f,2.0f } };
		if( msRect.IsContainedBy( hitbox ) || selected )
		{
			selected = true;

			pos = msPos;
			while( !hitbox.Contains( pos ) )
			{
				const Vec2 diff = basePos - pos;
				pos += diff / 10.0f;
			}
		}
	}
	else
	{
		selected = false;
		const Vec2 diff = basePos - pos;
		pos += diff / 3.0f;
		minimizeAmount = 0.0f;
	}

	// This needs to be outside of any ms.LeftIsPressed stuff.
	dir = ( basePos - pos ) * -1.0f * 0.1f;
}

void Joystick::Draw( Graphics& gfx ) const
{
	// gfx.DrawRectDim( hitbox.left,hitbox.top,hitbox.right,hitbox.bottom,Colors::Red );
	gfx.DrawCircle( int( basePos.x ),int( basePos.y ),int( size * 1.5 ),Colors::Blue );
	gfx.DrawCircle( int( pos.x ),int( pos.y ),int( size ) - 5,Colors::Gray );
}

const Vec2& Joystick::GetDir() const
{
	return dir;
}
