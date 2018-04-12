#pragma once

#include "Vec2.h"
#include "Graphics.h"
#include "Poly.h"
#include "Mouse.h"
#include <vector>
#include "Random.h"

class Hero
{
private:
	enum class PowerupType
	{
		FasterFireRate,
		DoubleShot
	};
public:
	class Bullet
	{
	public:
		Bullet( const Vec2& pos,const Vec2& vel );
		Bullet( const Bullet& other );
		Bullet& operator=( const Bullet& other );

		void Update( float dt );
		void Draw( Graphics& gfx ) const;

		void Kill();

		operator bool() const;
		const Rect& GetRect() const;
	private:
		static constexpr float speed = 350.0f;
		static constexpr float s = 7.0f; // For vertex stuff.
		const Vec2 size;
		Vec2 pos;
		Vec2 vel;
		Poly shape =
		{
			{
				{ -s,s },{ 0.0f,-s * 1.5f },
				{ 0.0f,0.0f },{ 0.0f,-s * 1.5f },
				{ s,s },{ 0.0f,0.0f }
			},Colors::Cyan
		};
		bool alive = true;
		Rect hitbox;
	};
public:
	// Construct at position pos;
	Hero( const Vec2& pos );
	// NO COPY!!
	Hero( const Hero& other ) = delete;
	// NO COPY!!
	Hero& operator=( const Hero& other ) = delete;

	// Move by moveAmount.
	void operator+=( const Vec2& moveAmount );
	// Shoot at target.
	void operator>>( const Vec2& target );
	// Check for bullet hit.
	bool operator<=( const Rect& hitRect );

	// Check for mouse press, etc.
	void Update( const Mouse& ms,float dt );
	// Draw my dude to the screen.
	void Draw( Graphics& gfx );

	// Call this when hero collects powerup.
	void PowerUp();
	// Reset player to initial state.
	void Reset();

	// Returns position of center.
	const Vec2& GetPos() const;
	// Returns collision rectangle.
	const Rect& GetRect() const;
	// Gives max targeting distance.
	const float& GetRange() const;
	// Tells damage of each bullet fired.
	const float& GetDamage() const;
private:
	static constexpr float refireTime = 0.15f;
	static constexpr float refireTime2 = refireTime / 2.0f;
	static constexpr float powerCooldown = 12.5f;
	PowerupType pt;
	float powerdownTimer = 0.0f;
	static constexpr float range = 250.0f;
	static constexpr float damage = 3.5f;
	static constexpr float s = 12.0f; // For vertices.
	static constexpr float speed = 60.5f;
	float shotTimer = 0.0f;
	bool powerupActive = false;
	Random rng;
	Vec2 pos;
	const Vec2 size;
	Vec2 vel = { 0.0f,0.0f };
	std::vector<Bullet> bullets;
	Poly shape =
	{
		{
			{ s,0 },{ -s,-s },{ 0.0f,0.0f },{ -s,s }
		},Colors::Cyan
	};
	Rect hitbox;
};