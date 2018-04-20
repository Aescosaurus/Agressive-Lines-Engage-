#pragma once

#include "Colors.h"
#include "Vec2.h"
#include <vector>
#include "Graphics.h"
#include <string>

class Poly
{
public:
	Poly() = default;
	Poly( std::initializer_list<Vec2> vList,Color c = Colors::Magenta );
	Poly( std::string filePath,Color c = Colors::Magenta );

	void Draw( Graphics& gfx ) const;
	void DrawTransparent( float alpha,Graphics& gfx ) const;

	void MoveTo( const Vec2& pos );
	void MoveBy( const Vec2& offset );

	void Rotate( float amount );
	void SetRotation( float amount );
	void SetColor( Color c );

	Color GetColor() const;
private:
	Vec2 pos = Vec2{ 200.0f,200.0f };
	float angle = 0.0f;
	Color c = Colors::Magenta;
	std::vector<Vec2> vertices;
};