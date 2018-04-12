#include "Enemy.h"
#include "META.h"

Food::Food( const Vec2& pos,const Vec2& size,float hp )
	:
	pos( pos ),
	size( size ),
	hitbox( pos - size / 2.0f,pos + size / 2.0f ),
	health( hp )
{
}

void Food::Draw( Graphics& gfx ) const
{
	gfx.DrawRectSafe( int( pos.x ),int( pos.y ),
		int( size.x ),int( size.y ),Colors::Magenta );
}

void Food::Target( const Vec2& targetPos )
{
	// Usually override this, but no error if you don't.
}

void Food::Damage( float damage,Powerup* pPowerup,Random& rng )
{
	health -= damage;

	if( health < 0.0f && rng.NextInt( 0,100 ) > 85 )
	{
		pPowerup->Reset( pos );
	}
}

const Vec2& Food::GetPos() const
{
	return pos;
}

const Rect& Food::GetRect() const
{
	return hitbox;
}

Food::operator bool() const
{
	return( health > 0.0f );
}

Meatball::Meatball( Random& rng,const Vec2& playerPos )
	:
	Food( Vec2{ rng.NextFloat( -float( Graphics::ScreenWidth ),
		float( Graphics::ScreenWidth ) ),
		rng.NextFloat( -float( Graphics::ScreenHeight ),
		float( Graphics::ScreenHeight ) ) } *2.0f,
		Vec2{ 40.0f,40.0f },16.0f )
{
	hitbox.MoveTo( pos - size / 2.0f );
	while( hitbox.IsContainedBy( Graphics::GetScreenRect() ) )
	{
		pos = Vec2{ rng.NextFloat( -float( Graphics::ScreenWidth ),
			float( Graphics::ScreenWidth ) ),
			rng.NextFloat( -float( Graphics::ScreenHeight ),
			float( Graphics::ScreenHeight ) ) } * 2.0f;

		hitbox.MoveTo( pos - size / 2.0f );
	}

	vel = playerPos - pos;
	vel.Normalize();
}

Meatball::Meatball( const Vec2& pos )
	:
	Food( pos,Vec2{ 40.0f,40.0f },10.0f )
{
}

Meatball::Meatball( const Meatball& other )
	:
	Meatball( pos )
{
	*this = other;
}

Meatball& Meatball::operator=( const Meatball& other )
{
	pos = other.pos;
	hitbox = other.hitbox;
	health = other.health;
	vel = other.vel;
	rotSpeed = other.rotSpeed;
	shape = other.shape;

	return( *this );
}

void Meatball::Update( Random& rng,const Vec2& playerPos,float dt )
{
	pos += vel * speed * dt;

	shape.Rotate( rng.NextFloat( dt,rotSpeed * dt ) );

	hitbox.MoveTo( pos - size / 2.0f );
	shape.MoveTo( pos );
}

void Meatball::Draw( Graphics& gfx ) const
{
	// gfx.DrawCircleSafe( int( pos.x ),int( pos.y ),
	// 	int( size.x / 2.0f ),Colors::Red );
	shape.Draw( gfx );
	shape.Draw( gfx );

#if !releaseMode
	gfx.DrawHitbox( hitbox );
#endif
}

void Meatball::Target( const Vec2& target )
{
	if( !hitbox.IsOverlappingWith( Graphics::GetScreenRect()
		.GetExpanded( 10.0f ) ) )
	{
		const Vec2 diff = pos - target;
		vel = diff.GetNormalized() * -1.0f;
	}
}

Pasta::Pasta( Random& rng )
	:
	Food( Vec2{ rng.NextFloat( -float( Graphics::ScreenWidth ),
	float( Graphics::ScreenWidth ) ),
	rng.NextFloat( -float( Graphics::ScreenHeight ),
	float( Graphics::ScreenHeight ) ) } *2.0f,
	Vec2{ 30.0f,30.0f },10.0f )
{
	hitbox.MoveTo( pos - size / 2.0f );
	while( hitbox.IsContainedBy( Graphics::GetScreenRect() ) )
	{
		pos = Vec2{ rng.NextFloat( -float( Graphics::ScreenWidth ),
			float( Graphics::ScreenWidth ) ),
			rng.NextFloat( -float( Graphics::ScreenHeight ),
			float( Graphics::ScreenHeight ) ) } *2.0f;

		hitbox.MoveTo( pos - size / 2.0f );
	}
}

void Pasta::Update( Random& rng,const Vec2& playerPos,float dt )
{
	vel = Vec2{ playerPos - pos }.GetNormalized();

	pos += vel * speed * dt;

	shape.SetRotation( vel.GetAngle() + 67.5f );

	shape.MoveTo( pos );
	hitbox.MoveTo( pos - size / 2.0f );
}

void Pasta::Draw( Graphics& gfx ) const
{
	shape.Draw( gfx );
	shape.Draw( gfx );

#if !releaseMode
	gfx.DrawHitbox( hitbox );
#endif
}