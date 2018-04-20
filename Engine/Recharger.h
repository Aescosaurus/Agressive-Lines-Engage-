#pragma once

#include "Poly.h"
#include "Random.h"
#include "Rect.h"
#include "Powerup.h"

class Recharger
{
public:
	Recharger( const Vec2& pos,bool active = true );

	void Update( Powerup* pPowerup,
		bool playerLostHealth,float dt );
	void Draw( Graphics& gfx ) const;

	void Reset( const Vec2& pos );

	const Rect& GetRect() const;
private:
	static constexpr float rotSpeed = 1.6f;
	Random rng;
	
	Vec2 pos;
	Rect hitbox;
	bool active = false;
	float speedMultiplier = 1.0f;

	static constexpr float ss = 14.0f;
	Poly shape =
	{
		{
			Vec2{ -ss,-ss },Vec2{ ss,-ss },
			Vec2{ ss,ss },Vec2{ -ss,-ss },
			Vec2{ -ss,ss },Vec2{ ss,ss }
		},Colors::White
	};
};