#pragma once

#include "Vec2.h"
#include "Rect.h"
#include "Graphics.h"
#include "Random.h"
#include "Poly.h"
#include "Powerup.h"
#include <vector>

class Enemy
{
public:
	Enemy( const Vec2& pos,const Vec2& size,float hp );

	virtual void Update( Random& rng,const Vec2& playerPos,float dt ) = 0;
	virtual void Draw( Graphics& gfx ) const;

	virtual void Target( const Vec2& targetPos );
	void Damage( float damage,Powerup* pPowerup,Random& rng );

	const Vec2& GetPos() const;
	const Rect& GetRect() const;
	operator bool() const;
protected:
	Vec2 pos;
	const Vec2 size;
	Rect hitbox;
	float health;
private:
};

class Meatball
	:
	public Enemy
{
public:
	Meatball( Random& rng,const Vec2& playerPos );

	Meatball( const Meatball& other );
	Meatball& operator=( const Meatball& other );

	void Update( Random& rng,const Vec2& playerPos,float dt );
	void Draw( Graphics& gfx ) const override;

	void Target( const Vec2& target );
private:
	Meatball( const Vec2& pos );
private:
	static constexpr float speed = 65.2f;
	Vec2 vel = { 0.0f,0.0f };
	float rotSpeed = 10.0f;
	static constexpr float s = 10.0f; // For vertices.
	Poly shape =
	{
		{
			Vec2( -s,-s * 1.5f ),Vec2( s,-s * 1.5f ),
			Vec2( s * 1.5f,0.0f ),Vec2( s,s ),
			Vec2( -s,s ),Vec2( 0.0f,0.0f ),
			Vec2( -s * 1.5f,0.0f )
		},Colors::Red
	};
};

class Pasta
	:
	public Enemy
{
public:
	Pasta( Random& rng );

	void Update( Random& rng,const Vec2& playerPos,float dt );
	void Draw( Graphics& gfx ) const override;
private:
	static constexpr float speed = 105.3f;
	Vec2 vel = { 0.0f,0.0f };
	float rotSpeed = 10.0f;
	static constexpr float s = 10.0f; // For vertices.
	Poly shape =
	{
		{
			Vec2( -s,-s * 2.0f ),Vec2( s,-s * 2.0f ),
			Vec2( s,s * 2.0f ),Vec2( -s,s * 2.0f )
		},Colors::Pink
	};
};