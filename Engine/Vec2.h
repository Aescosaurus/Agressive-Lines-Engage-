#pragma once

class Vec2
{
public:
	Vec2() = default;
	Vec2( float x_in,float y_in );

	Vec2 operator+( const Vec2& rhs ) const;
	Vec2& operator+=( const Vec2& rhs );
	Vec2 operator*( float rhs ) const;
	Vec2& operator*=( float rhs );
	Vec2 operator-( const Vec2& rhs ) const;
	Vec2& operator-=( const Vec2& rhs );
	Vec2 operator/( float rhs ) const;
	Vec2& operator/=( float rhs );

	bool operator==( const Vec2& rhs ) const;
	bool operator!=( const Vec2& rhs ) const;

	int operator[]( char letter );

	float GetLength() const;
	float GetLengthSq() const;
	float GetAngle() const;

	Vec2& Normalize();
	Vec2 GetNormalized() const;

	Vec2 Rotation( const float angle ) const;

	static Vec2 Up();
	static Vec2 Down();
	static Vec2 Left();
	static Vec2 Right();
public:
	float x;
	float y;
};