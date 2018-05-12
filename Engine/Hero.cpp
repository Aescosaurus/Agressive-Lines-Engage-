#include "Hero.h"
#include "META.h"

Hero::Bullet::Bullet( const Vec2& pos,const Vec2& vel,Color c )
	:
	pos( pos ),
	size( 15.0f,15.0f ),
	vel( vel ),
	hitbox( pos - size / 2.0f,pos + size / 2.0f )
{
	const float angle = atan2( vel.y,vel.x );
	shape.Rotate( angle + 1.5f );
	shape.MoveTo( pos );
	shape.SetColor( c );
}

Hero::Bullet::Bullet( const Vec2& pos,float angle,Color c )
	:
	Bullet( pos,Vec2( std::cos( angle ),std::sin( angle ) ),c )
{
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

#if !DRAW_RELEASE_STUFF
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
	vel = moveAmount;
	shape.SetRotation( vel.GetAngle() );
}

void Hero::operator>>( const Vec2& target )
{
	if( !powerupActive || pt != PowerupType::FasterFireRate )
	{
		if( pt == PowerupType::TripleShot )
		{
			if( shotTimer > refireTime3 )
			{
				shotTimer = 0.0f;
				// Shoot 3 bullets instead of just 1.
				bullets.emplace_back( Bullet{ pos,Vec2( target - pos )
					.GetAngle() + 0.0f,shape.GetColor() } );
				bullets.emplace_back( Bullet{ pos,Vec2( pos - target )
					.GetAngle() + 3.4f,shape.GetColor() } );
				bullets.emplace_back( Bullet{ pos,Vec2( pos - target )
					.GetAngle() - 3.4f,shape.GetColor() } );
			}
		}
		else if( shotTimer > refireTime )
		{
			shotTimer = 0.0f;
			if( pt == PowerupType::DoubleShot )
			{
				// Shoot 2 bullets instead of just 1.
				bullets.emplace_back( Bullet{ pos,
					Vec2( pos - target )
					.GetAngle() + 3.0f,shape.GetColor() } );
				bullets.emplace_back( Bullet{ pos,
					Vec2( pos - target )
					.GetAngle() - 3.0f,shape.GetColor() } );
			}
			else
			{
				bullets.emplace_back( Bullet{ pos,
					Vec2( target - pos )
					.Normalize(),shape.GetColor() } );
			}
		}
	}
	else if( pt == PowerupType::FasterFireRate )
	{
		if( shotTimer > refireTime2 )
		{
			shotTimer = 0.0f;
			bullets.emplace_back( Bullet{ pos,
				Vec2( target - pos ).GetNormalized(),
				shape.GetColor() } );
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

	for( auto& it = bullets.begin(); it < bullets.end(); ++it )
	{
		auto& b = *it;
		if( b )
		{
			b.Update( dt );
		}
		else
		{
			it = bullets.erase( it );
			if( it == bullets.end() ) return;
		}
	}

	if( powerupActive )
	{
		powerdownTimer += dt;
		if( powerdownTimer > powerCooldown )
		{
			powerupActive = false;
			pt = PowerupType::None;
			powerdownTimer = 0.0f;
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
	shield.MoveTo( pos );
	hitbox.MoveTo( pos - size / 2.0f );
}

void Hero::Draw( Graphics& gfx )
{
	shape.Draw( gfx );
	shape.Draw( gfx );

	{
		shield.MoveBy( Vec2( -1.0f,-1.0f ) );
		shield.DrawTransparent( float( hp ) / float( maxHP ),gfx );
		shield.MoveBy( Vec2( 1.0f,1.0f ) );
		shield.DrawTransparent( float( hp ) / float( maxHP ),gfx );
		shield.MoveBy( Vec2( 1.0f,1.0f ) );
		shield.DrawTransparent( float( hp ) / float( maxHP ),gfx );
	}


	for( const Bullet& b : bullets )
	{
		b.Draw( gfx );
	}

#if !DRAW_RELEASE_STUFF
	gfx.DrawHitbox( hitbox );
#endif
}

void Hero::PowerUp()
{
	powerupActive = true;
	powerdownTimer = 0.0f;

	pt = PowerupType( rng.NextInt( int( PowerupType
		::FasterFireRate ),int( PowerupType
			::TripleShot ) ) );

	if( pt == PowerupType::FasterFireRate )
	{
		shape.SetColor( Colors::Yellow );
	}
	if( pt == PowerupType::DoubleShot )
	{
		shape.SetColor( Colors::LightGray );
	}
	if( pt == PowerupType::TripleShot )
	{
		shape.SetColor( Colors::Gray );
	}
}

void Hero::Heal()
{
	hp = hp + healAmount + rng.NextInt( -1,1 );

	if( hp > maxHP ) hp = maxHP;
}

void Hero::Reset()
{
	hp = maxHP;
	powerupActive = false;
	shotTimer = 0.0f;
	powerdownTimer = 0.0f;
	pos = Vec2{ float( Graphics::ScreenWidth / 2 ),
		float( Graphics::ScreenHeight / 2 ) };
	pt = PowerupType::None;
	vel = Vec2{ 0.0f,0.0f };
	hitbox.MoveTo( pos );
	shape.MoveTo( pos );
	shape.SetColor( Colors::Cyan );
}

void Hero::Attack()
{
	--hp;
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

int Hero::GetHP() const
{
	return hp;
}

bool Hero::IsMaxHP() const
{
	return !( hp < maxHP );
}
