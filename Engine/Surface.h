#pragma once

#include "Colors.h"
#include <string>
#include "Rect.h"

class Surface
{
public:
	Surface( const std::string& filename );
	Surface( int width,int height );
	Surface( const Surface& );
	~Surface();
	Surface& operator=( const Surface& );
	void PutPixel( int x,int y,Color c );
	void PutPixel( int x,int y,int r,int g,int b );
	void Fill( Color c );
	Color GetPixel( int x,int y ) const;
	int GetWidth() const;
	int GetHeight() const;
	Rect GetRect() const;
private:
	Color * pPixels = nullptr;
	int width;
	int height;
};