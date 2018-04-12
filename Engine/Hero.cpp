#include "Hero.h"
#include "META.h"

Hero::Bullet::Bullet( const Vec2& pos,const Vec2& vel )
	:
	pos( pos ),
	size( 15.0f,15.0f ),
	vel( vel ),
	hitbox( pos - size / 2.0f,pos + size / 2.0f )
{
	const float angle = atan2( vel.y,vel.x );
	shape.Rotate( angle + 1.5f );
	shape.MoveTo( pos );
}

Hero::Bullet::Bullet( const Bullet& other )
	:
	size( other.size )
{
	*this = other;
}

Hero::Bullet& Hero::Bullet::operator=( const Bullet& other )
{
	pos = other.pos;
	vel = other.vel;
	shape = other.shape;
	alive = other.alive;
	hitbox = other.hitbox;

	return( *this );
}

void Hero::Bullet::Update( float dt )
{
	pos += vel * speed * dt;

	shape.MoveTo( pos );

	if( !Rect{ pos,Vec2{ s,s } *2.0f }
		.IsContainedBy( Graphics::GetScreenRect() ) )
	{
		alive = false;
	}

	hitbox.MoveTo( pos - size / 2.0f );
}

void Hero::Bullet::Draw( Graphics& gfx ) const
{
	// Makes it darker color.
	shape.Draw( gfx );
	shape.Draw( gfx );

#if !releaseMode
	gfx.DrawHitbox( hitbox );
#endif
}

void Hero::Bullet::Kill()
{
	alive = false;
}

Hero::Bullet::operator bool() const
{
	return alive;
}

const Rect& Hero::Bullet::GetRect() const
{
	return( hitbox );
}

Hero::Hero( const Vec2& pos )
	:
	pos( pos ),
	size( Vec2{ 30.0f,30.0f } ),
	hitbox( pos - size / 2.0f,pos + size / 2.0f )
{
}

void Hero::operator+=( const Vec2& moveAmount )
{
	// pos += moveAmount;
	vel = moveAmount;
	shape.SetRotation( vel.GetAngle() );
}

void Hero::operator>>( const Vec2& target )
{
	if( !powerupActive )
	{
		if( shotTimer > refireTime )
		{
			shotTimer = 0.0f;
			if( pt == PowerupType::DoubleShot )
			{
				// Shoot 2 bullets instead of just 1.
				bullets.emplace_back( Bullet{ pos,Vec2( target - pos )
					.Normalize() } );
			}
			else
			{
				bullets.emplace_back( Bullet{ pos,Vec2( target - pos )
					.Normalize() } );
			}
		}
	}
	else // Powerup is active.
	{
		if( pt == PowerupType::FasterFireRate )
		{
			if( shotTimer > refireTime2 )
			{
				shotTimer = 0.0f;
				bullets.emplace_back( Bullet{ pos,Vec2{ target - pos }.Normalize() } );
			}
		}
		else if( pt == PowerupType::DoubleShot )
		{
		}
	}
}

bool Hero::operator<=( const Rect& hitRect )
{
	for( Bullet& b : bullets )
	{
		if( b.GetRect().IsOverlappingWith( hitRect ) )
		{
			b.Kill();
			return true;
		}
	}
	return false;
}

void Hero::Update( const Mouse& ms,float dt )
{
	shotTimer += dt;

	for( size_t i = 0; i < bullets.size(); ++i )
	{
		if( bullets[i] )
		{
			bullets[i].Update( dt );
		}
		else
		{
			bullets.erase( bullets.begin() + i );
			--i; // Really important!
		}
	}

	if( powerupActive )
	{
		powerdownTimer += dt;
		if( powerdownTimer > powerCooldown )
		{
			powerupActive = false;
			shape.SetColor( Colors::Cyan );
		}
	}

	pos += vel * speed * dt;

	const float incAmount = 0.01f;
	while( pos.x - size.x < 0.0f ) pos.x += incAmount;
	while( int( pos.x + size.x ) > Graphics::ScreenWidth ) pos.x -= incAmount;
	while( pos.y - size.y < 0.0f ) pos.y += incAmount;
	while( int( pos.y + size.y ) > Graphics::ScreenHeight ) pos.y -= incAmount;

	shape.MoveTo( pos );
	hitbox.MoveTo( pos - size / 2.0f );
}

void Hero::Draw( Graphics& gfx )
{
	// const Vec2 dPos = pos - size / 2.0f;
	// gfx.DrawRect( int( dPos.x ),int( dPos.y ),
	// 	int( size.x ),int( size.y ),Colors::Cyan );
	shape.Draw( gfx );
	shape.Draw( gfx );

	for( const Bullet& b : bullets )
	{
		b.Draw( gfx );
	}

#if !releaseMode
	gfx.DrawHitbox( hitbox );
#endif
}

void Hero::PowerUp()
{
	powerupActive = true;
	powerdownTimer = 0.0f;
	shape.SetColor( Colors::Yellow );

	pt = PowerupType( rng.NextInt( int( PowerupType::FasterFireRate ),
		int( PowerupType::DoubleShot ) ) );
}

void Hero::Reset()
{
	powerupActive = false;
	shotTimer = 0.0f;
	powerdownTimer = 0.0f;
	pos = Vec2{ float( Graphics::ScreenWidth / 2 ),
		float( Graphics::ScreenHeight / 2 ) };
	vel = Vec2{ 0.0f,0.0f };
	hitbox.MoveTo( pos );
	shape.MoveTo( pos );
	shape.SetColor( Colors::Cyan );
}

const Vec2& Hero::GetPos() const
{
	return pos;
}

const Rect& Hero::GetRect() const
{
	return hitbox;
}

const float& Hero::GetRange() const
{
	return range;
}

const float& Hero::GetDamage() const
{
	return damage;
}
