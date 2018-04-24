#include "Enemy.h"
#include "META.h"

Random Food::rng = Random();

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

void Food::Damage( float damage,Powerup* pPowerup,
	Recharger* pRecharger )
{
	health -= damage;

	if( health < 0.0f )
	{
		if( rng.NextInt( 0,100 ) > 85 &&
			!pPowerup->GetRect().IsContainedBy( Graphics
				::GetScreenRect() ) )
		{
			pPowerup->Reset( pos );
		}
		else if( rng.NextInt( 0,100 ) > 93 &&
			!pRecharger->GetRect().IsContainedBy( Graphics
				::GetScreenRect() ) )
		{
			pRecharger->Reset( pos );
		}
	}
}

void Food::Destroy( Powerup* pPowerup )
{
	health = -1.0f;
	if( rng.NextInt( 0,100 ) > 85 &&
		!pPowerup->GetRect().IsContainedBy( Graphics
			::GetScreenRect() ) )
	{
		pPowerup->Reset( pos );
	}
}

void Food::EndRoutine( std::vector<DuoVec2>& foodVec )
{
	// Do nothing, for most foods.
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

Meatball::Meatball( const Vec2& playerPos )
	:
	Food( Vec2( rng.NextFloat( -float( Graphics::ScreenWidth ),
		float( Graphics::ScreenWidth ) ),
		rng.NextFloat( -float( Graphics::ScreenHeight ),
			float( Graphics::ScreenHeight ) ) ) * 2.0f,
		Vec2{ 40.0f,40.0f },17.0f )
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

	vel = playerPos - pos;
	vel.Normalize();
}

Meatball::Meatball( const Vec2& pos,const Vec2& size )
	:
	Food( pos,size,10.0f )
{
}

Meatball::Meatball( const Meatball& other )
	:
	Meatball( pos,Vec2{ 40.0f,40.0f } )
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

void Meatball::Update( const Vec2& playerPos,float dt )
{
	pos += vel * speed * dt;

	shape.Rotate( rng.NextFloat( dt,rotSpeed * dt ) );

	hitbox.MoveTo( pos - size / 2.0f );
	shape.MoveTo( pos );
}

void Meatball::Draw( Graphics& gfx ) const
{
	shape.Draw( gfx );
	shape.Draw( gfx );

#if !DRAW_RELEASE_STUFF
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

void Meatball::Reset( const Vec2& playerPos )
{
	vel = ( pos - playerPos ).GetNormalized();
}

Pasta::Pasta()
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

Pasta::Pasta( const Pasta& other )
	:
	Pasta()
{
	*this = other;
}

Pasta & Pasta::operator=( const Pasta& other )
{
	pos = other.pos;
	hitbox = other.hitbox;
	health = other.health;
	vel = other.vel;
	rotSpeed = other.rotSpeed;
	shape = other.shape;

	return( *this );
}

void Pasta::Update( const Vec2& playerPos,float dt )
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

#if !DRAW_RELEASE_STUFF
	gfx.DrawHitbox( hitbox );
#endif
}

void Pasta::Reset( const Vec2& playerPos )
{
	*this = Pasta();
}

Orange::Orange()
	:
	Food( Vec2{ 0.0f,0.0f },Vec2( sc,sc ) * 2.0f,24.0f )
{
	const int chance = rng.NextInt( 0,100 );
	if( chance > 75 )
	{ // Starts above screen going down.
		pos.x = rng.NextFloat( size.x,float( Graphics
			::ScreenWidth ) - size.x );
		pos.y = rng.NextFloat( -size.y,-size.y * 4.0f );
		vel = { 0.0f,1.0f };
	}
	else if( chance > 50 )
	{ // Starts below screen going up.
		pos.x = rng.NextFloat( size.x,float( Graphics
			::ScreenWidth ) - size.x );
		pos.y = rng.NextFloat( float( Graphics
			::ScreenHeight ) + size.y,float( Graphics
				::ScreenHeight ) + size.y * 4.0f );
		vel = { 0.0f,-1.0f };
	}
	else if( chance > 25 )
	{ // Starts left of screen going right.
		pos.y = rng.NextFloat( size.y,float( Graphics
			::ScreenHeight ) - size.y );
		pos.x = rng.NextFloat( -size.x,-size.x * 4.0f );
		vel = { 1.0f,0.0f };
	}
	else
	{ // Starts right of screen going left.
		pos.y = rng.NextFloat( size.y,float( Graphics
			::ScreenHeight ) - size.y );
		pos.x = rng.NextFloat( float( Graphics
			::ScreenWidth ) + size.x,float( Graphics
				::ScreenWidth ) + size.x * 4.0f );
		vel = { -1.0f,0.0f };
	}
}

Orange::Orange( const Orange& other )
	:
	Orange()
{
	*this = other;
}

Orange & Orange::operator=( const Orange& other )
{
	pos = other.pos;
	hitbox = other.hitbox;
	health = other.health;
	vel = other.vel;
	shape = other.shape;

	return( *this );
}

void Orange::Update( const Vec2& playerPos,float dt )
{
	pos += vel * speed * dt;

	hitbox.MoveTo( pos - size / 2.0f );
	shape.MoveTo( pos );
}

void Orange::Draw( Graphics& gfx ) const
{
	shape.Draw( gfx );
	shape.Draw( gfx );

#if !DRAW_RELEASE_STUFF
	gfx.DrawHitbox( hitbox );
#endif
}

void Orange::EndRoutine( std::vector<DuoVec2>& foodVec )
{
	foodVec.emplace_back( DuoVec2( pos,Vec2( 0.0f,-1.0f ) ) );
	foodVec.emplace_back( DuoVec2( pos,Vec2( 0.0f,1.0f ) ) );
	foodVec.emplace_back( DuoVec2( pos,Vec2( -1.0f,0.0f ) ) );
	foodVec.emplace_back( DuoVec2( pos,Vec2( 1.0f,0.0f ) ) );
}

void Orange::Target( const Vec2& playerPos )
{
	if( !hitbox.IsOverlappingWith( Graphics::GetScreenRect()
		.GetExpanded( 10.0f ) ) )
	{
		const Vec2 diff = pos - playerPos;
		vel = diff.GetNormalized() * -1.0f;
	}
}

void Orange::Reset( const Vec2& playerPos )
{
	vel = ( pos - playerPos ).GetNormalized();
}

Orange::Orange( const Vec2& pos,const Vec2& size,float hp )
	:
	Food( pos,size,hp )
{
}

OrangeSlice::OrangeSlice( const Vec2& startPos,const Vec2& vel )
	:
	Orange( startPos,Vec2( sc,sc ),6.0f )
{
	this->vel = vel;
	hitbox = Rect{ pos - size / 2.0f,pos + size / 2.0f };
}

OrangeSlice::OrangeSlice( const DuoVec2& posAndVel )
	:
	OrangeSlice( posAndVel.pos,posAndVel.vel )
{
}

void OrangeSlice::Update( const Vec2& playerPos,float dt )
{
	// Moves slightly faster than big orange.
	pos += vel * speed * 1.5f * dt;

	hitbox.MoveTo( pos - size / 2.0f );
	shape.MoveTo( pos );
}

void OrangeSlice::Draw( Graphics& gfx ) const
{
	shape.Draw( gfx );
	shape.Draw( gfx );

#if !DRAW_RELEASE_STUFF
	gfx.DrawHitbox( hitbox );
#endif
}

void OrangeSlice::EndRoutine( std::vector<DuoVec2>& foodVec )
{
	// Don't spawn infinitely more oranges!
}

void OrangeSlice::Reset( const Vec2& playerPos )
{
	vel = ( pos - playerPos ).GetNormalized();
}

Lime::Lime()
	:
	Food( Vec2( rng.NextFloat( -float( Graphics::ScreenWidth ),
		float( Graphics::ScreenWidth ) ),
		rng.NextFloat( -float( Graphics::ScreenHeight ),
			float( Graphics::ScreenHeight ) ) ) * 2.0f,
		Vec2{ 40.0f,40.0f },15.0f )
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

Lime::Lime( const Lime& other )
	:
	Lime()
{
	*this = other;
}

Lime& Lime::operator=( const Lime& other )
{
	pos = other.pos;
	hitbox = other.hitbox;
	health = other.health;
	vel = other.vel;
	angle = other.angle;

	return( *this );
}

void Lime::Update( const Vec2& playerPos,float dt )
{
}

void Lime::Reset( const Vec2& playerPos )
{
}
