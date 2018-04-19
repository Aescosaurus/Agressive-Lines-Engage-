#pragma once

#include "Vec2.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Rect.h"

class Joystick
{
public:
	Joystick( const Vec2& pos );

	void Update( const Mouse& ms );
	void Draw( Graphics& gfx ) const;

	void Reset();

	const Vec2& GetDir() const;
private:
	static constexpr float size = 35.0f;
	static constexpr float range = size;
	static constexpr float speedupFactor = 3.0f;
	bool selected = false;
	const Vec2 basePos;
	Vec2 pos;
	Rect hitbox;

	Vec2 dir = { 0.0f,0.0f };
};