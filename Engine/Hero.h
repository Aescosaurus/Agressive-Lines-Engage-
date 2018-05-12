#pragma once

#include "Vec2.h"
#include "Graphics.h"
#include "Poly.h"
#include "Mouse.h"
#include <vector>
#include "Random.h"

class Hero
{
public:
	class Bullet
	{
	public:
		Bullet( const Vec2& pos,const Vec2& vel,Color c );
		Bullet( const Vec2& pos,float angle,Color c );

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
private:
	enum class PowerupType
	{
		None,
		FasterFireRate,
		DoubleShot,
		TripleShot
	};
public:
	Hero( const Vec2& pos );
	Hero( const Hero& other ) = delete;
	Hero& operator=( const Hero& other ) = delete;

	void operator+=( const Vec2& moveAmount );
	void operator>>( const Vec2& target );
	bool operator<=( const Rect& hitRect );

	void Update( const Mouse& ms,float dt );
	void Draw( Graphics& gfx );

	void PowerUp();
	void Heal();
	void Reset();
	void Attack();

	const Vec2& GetPos() const;
	const Rect& GetRect() const;
	const float& GetRange() const;
	const float& GetDamage() const;
	int GetHP() const;
	bool IsMaxHP() const;
private:
	// Default
	static constexpr float refireTime = 0.15f;
	// Faster
	static constexpr float refireTime2 = refireTime / 2.0f;
	// Slower
	static constexpr float refireTime3 = refireTime * 1.5f;
	static constexpr float powerCooldown = 12.5f;
	PowerupType pt;
	float powerdownTimer = 0.0f;
	static constexpr float range = 250.0f;
	static constexpr float damage = 3.5f;
	static constexpr float speed = 60.5f;
	float shotTimer = 0.0f;
	bool powerupActive = false;
	static constexpr int healAmount = 5;
	static constexpr int maxHP = 10;
	int hp = maxHP;
	Random rng;
	Vec2 pos;
	const Vec2 size;
	Vec2 vel = { 0.0f,0.0f };
	std::vector<Bullet> bullets;
	static constexpr float s = 12.0f; // For vertices.
	Poly shape =
	{
		{
			{ s,0 },{ -s,-s },{ 0.0f,0.0f },{ -s,s }
		},Colors::Cyan
	};
	static constexpr float ss = s * 1.7f;
	static constexpr float sh = ss / 1.3f;
	Poly shield =
	{
		{
			Vec2( -sh,-sh ),Vec2( 0.0f,-ss ),
			Vec2( sh,-sh ),Vec2( ss,0.0f ),
			Vec2( sh,sh ),Vec2( 0.0f,ss ),
			Vec2( -sh,sh ),Vec2( -ss,0.0f )
		},Colors::LightBlue
	};
	Rect hitbox;
};