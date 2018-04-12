#include "Animation.h"

Animation::Animation( int x,int y,int width,int height,int count,
	const Surface& sheet,float holdTime,Color chroma )
	:
	sprite( sheet ),
	holdTime( holdTime ),
	chroma( chroma )
{
	for( int i = 0; i < count; ++i )
	{
		frames.emplace_back( float( x + i * width ),
			float( x + ( i + 1 ) * width ),float( y ),float( y + height ) );
	}
}

void Animation::Update( float dt )
{
	curFrameTime += dt;
	while( curFrameTime >= holdTime )
	{
		Advance();
		curFrameTime -= holdTime;
	}
}

void Animation::Draw( const Vec2& pos,Graphics& gfx ) const
{
	gfx.DrawSprite( int( pos.x ),int( pos.y ),frames[iCurFrame],sprite,chroma );
}

void Animation::Draw( const Vec2& pos,Graphics& gfx,const Rect& clip ) const
{
	gfx.DrawSprite( int( pos.x ),int( pos.y ),frames[iCurFrame],clip,sprite,chroma );
}

void Animation::Advance()
{
	if( ++iCurFrame >= frames.size() )
	{
		iCurFrame = 0;
	}
}
