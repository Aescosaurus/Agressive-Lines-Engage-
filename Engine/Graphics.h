/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include "ChiliWin.h"
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Surface.h"
#include "Rect.h"
#include <cassert>

class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( int x,int y,Color c );
	void PutPixel( int x,int y,Color c,unsigned char alpha );
	void PutPixel( int x,int y,Color c,float alpha );
	void PutPixelT( int x,int y,Color c,float alpha );
	void DrawRect( int x,int y,int width,int height,Color c );
	void DrawRectDim( int x1,int y1,int x2,int y2,Color c );
	void DrawRectSafe( int x,int y,int width,int height,Color c );
	void DrawCircle( int x,int y,int radius,Color c );
	void DrawCircleSafe( int x,int y,int radius,Color c );
	void DrawLine( int x1,int y1,int x2,int y2,Color c );
	void DrawTransparentLine( const Vec2& pos1,const Vec2& pos2,float alpha,Color c );
	void DrawSpriteNonChroma( int x,int y,const Surface& s );
	void DrawSpriteNonChroma( int x,int y,const Rect& srcRect,const Surface& s );
	void DrawSpriteNonChroma( int x,int y,Rect srcRect,const Rect& clip,const Surface& s );
	void DrawSprite( int x,int y,const Surface& s,Color chroma = Colors::Magenta );
	void DrawSprite( int x,int y,const Rect& srcRect,const Surface& s,Color chroma = Colors::Magenta );
	void DrawSprite( int x,int y,Rect srcRect,const Rect& clip,const Surface& s,Color chroma = Colors::Magenta );
	template<typename E>
	void DrawSprite( int x,int y,Rect srcRect,const Rect& clip,const Surface& s,E effect,bool reversed = false )
	{
		assert( srcRect.left >= 0 );
		assert( srcRect.right <= s.GetWidth() );
		assert( srcRect.top >= 0 );
		assert( srcRect.bottom <= s.GetHeight() );

		// Mirror in x depending on reversed bool switch.
		if( !reversed )
		{
			// Clipping is different depending on mirroring status.
			if( x < int( clip.left ) )
			{
				srcRect.left += clip.left - float( x );
				x = int( clip.left );
			}
			if( y < int( clip.top ) )
			{
				srcRect.top += clip.top - float( y );
				y = int( clip.top );
			}
			if( float( x ) + srcRect.GetWidth() > clip.right )
			{
				srcRect.right -= float( x ) + srcRect.GetWidth() - clip.right;
			}
			if( float( y ) + srcRect.GetHeight() > clip.bottom )
			{
				srcRect.bottom -= float( y ) + srcRect.GetHeight() - clip.bottom;
			}
			for( int sy = int( srcRect.top ); sy < int( srcRect.bottom ); sy++ )
			{
				for( int sx = int( srcRect.left ); sx < int( srcRect.right ); sx++ )
				{
					effect(
						// No mirroring!
						s.GetPixel( sx,sy ),
						int( float( x + sx ) - srcRect.left ),
						int( float( y + sy ) - srcRect.top ),
						*this
					);
				}
			}
		}
		else
		{
			if( float( x ) < clip.left )
			{
				srcRect.right -= clip.left - float( x );
				x = int( clip.left );
			}
			if( float( y ) < clip.top )
			{
				srcRect.top += clip.top - float( y );
				y = int( clip.top );
			}
			if( float( x ) + srcRect.GetWidth() > clip.right )
			{
				srcRect.left += float( x ) + srcRect.GetWidth() - clip.right;
			}
			if( float( y ) + srcRect.GetHeight() > clip.bottom )
			{
				srcRect.bottom -= float( y ) + srcRect.GetHeight() - clip.bottom;
			}
			const int xOffset = int( srcRect.left + srcRect.right - 1.0f );
			for( int sy = int( srcRect.top ); sy < int( srcRect.bottom ); sy++ )
			{
				for( int sx = int( srcRect.left ); int( sx < srcRect.right ); sx++ )
				{
					effect(
						// Mirror in x.
						s.GetPixel( xOffset - sx,sy ),
						int( float( x + sx ) - srcRect.left ),
						int( float( y + sy ) - srcRect.top ),
						*this
					);
				}
			}
		}
	}
	void DrawHitbox( const Rect& hitbox,Color c = Colors::MakeRGB( 255,160,0 ) );
	~Graphics();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr int ScreenWidth = 800;
	static constexpr int ScreenHeight = 600;
	static Rect GetScreenRect();
};