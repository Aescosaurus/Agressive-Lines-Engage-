#pragma once

#include "Poly.h"
#include "Random.h"
#include "Rect.h"

class Powerup
{
public:
	Powerup( const Vec2& pos,bool active = true );

	void Update( float dt );
	void Draw( Graphics& gfx ) const;

	void Reset( const Vec2& pos );

	const Rect& GetRect() const;
private:
	static constexpr float rotSpeed = 3.1f; // From 7.3f
	Random rng;
	
	Vec2 pos;
	Rect hitbox;
	bool active = false;
	float speedMultiplier = 1.0f;

	static constexpr float s1 = 14.0f;
	Poly p =
	{
		{
			Vec2{ -s1,-s1 },Vec2{ s1,-s1 },
			Vec2{ s1,s1 },Vec2{ -s1,-s1 },
			Vec2{ -s1,s1 },Vec2{ s1,s1 }
		},Colors::Yellow
	};
};