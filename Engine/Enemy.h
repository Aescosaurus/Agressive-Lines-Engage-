#pragma once

#include "Vec2.h"
#include "Rect.h"
#include "Graphics.h"
#include "Random.h"
#include "Poly.h"
#include "Powerup.h"
#include <vector>
#include <memory>
#include "Recharger.h"

struct DuoVec2
{
	DuoVec2( const Vec2& pos,const Vec2& vel )
		:
		pos( pos ),
		vel( vel )
	{}
	Vec2 pos;
	Vec2 vel;
};

class Food
{
public:
	Food( const Vec2& pos,const Vec2& size,float hp );

	virtual void Update( const Vec2& playerPos,float dt ) = 0;
	virtual void Draw( Graphics& gfx ) const;

	virtual void Target( const Vec2& targetPos );
	void Damage( float damage,Powerup* pPowerup,
		Recharger* pRecharger );
	void Destroy( Powerup* pPowerup );
	virtual void EndRoutine( std::vector<DuoVec2>& foodVec );
	virtual void Reset( const Vec2& playerPos ) = 0;

	const Vec2& GetPos() const;
	const Rect& GetRect() const;
	operator bool() const;
protected:
	static Random rng;
	Vec2 pos;
	const Vec2 size;
	Rect hitbox;
	float health;
private:
};

class Meatball
	:
	public Food
{
public:
	Meatball( const Vec2& playerPos );

	Meatball( const Meatball& other );
	Meatball& operator=( const Meatball& other );

	void Update( const Vec2& playerPos,float dt );
	void Draw( Graphics& gfx ) const override;

	void Target( const Vec2& target ) override;
	void Reset( const Vec2& playerPos );
private:
	Meatball( const Vec2& pos,const Vec2& size );
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
	public Food
{
public:
	Pasta();
	Pasta( const Pasta& other );
	Pasta& operator=( const Pasta& other );

	void Update( const Vec2& playerPos,float dt );
	void Draw( Graphics& gfx ) const override;

	void Reset( const Vec2& playerPos );
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
		},Colors::Magenta
	};
};

class Orange
	:
	public Food
{
public:
	Orange();
	Orange( const Orange& other );
	Orange& operator=( const Orange& other );

	void Update( const Vec2& playerPos,float dt );
	virtual void Draw( Graphics& gfx ) const override;

	void EndRoutine( std::vector<DuoVec2>& foodVec ) override;
	void Target( const Vec2& playerPos ) override;
	virtual void Reset( const Vec2& playerPos );
protected:
	Orange( const Vec2& pos,const Vec2& size,float hp );
protected:
	Vec2 vel;
	static constexpr float sc = 22.0f;
	static constexpr float ss = sc / 1.5f;
private:
	static constexpr float speed = 60.0f;
	Poly shape =
	{
		{
			Vec2{ -ss,-ss },Vec2{ 0.0f,-sc },
			Vec2{ ss,-ss },Vec2{ sc,0.0f },
			Vec2{ ss,ss },Vec2{ 0.0f,sc },
			Vec2{ -ss,ss },Vec2{ -sc,0.0f }
		},Colors::RedOrange
	};
};

class OrangeSlice
	:
	public Orange
{
public:
	OrangeSlice( const Vec2& startPos,const Vec2& vel );
	OrangeSlice( const DuoVec2& posAndVel );

	void Update( const Vec2& playerPos,float dt ) override;
	void Draw( Graphics& gfx ) const override;

	void EndRoutine( std::vector<DuoVec2>& foodVec ) override;
	void Reset( const Vec2& playerPos ) override;
private:
	static constexpr float speed = 50.0f;
	static constexpr float mc = sc / 2.0f;
	static constexpr float ms = mc / 1.5f;
	Poly shape =
	{
		{
			Vec2{ -ms,-ms },Vec2{ 0.0f,-mc },
			Vec2{ ms,-ms },Vec2{ mc,0.0f },
			Vec2{ ms,ms },Vec2{ 0.0f,mc },
			Vec2{ -ms,ms },Vec2{ -mc,0.0f }
		},Colors::RedOrange
	};
};

class Lime
	:
	public Food
{
public:
	Lime();
	Lime( const Lime& other );
	Lime& operator=( const Lime& other );

	void Update( const Vec2& playerPos,float dt );
	
	void Reset( const Vec2& playerPos );
private:
	static constexpr float speed = 50.0f;
	Vec2 vel = { 0.0f,0.0f };
	float angle = 90.0f;
};