/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
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
#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		( LPVOID* )&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}


	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}


	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
	{ 1.0f,1.0f,0.5f,1.0f,0.0f },
	{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
	{ -1.0f,1.0f,0.5f,0.0f,0.0f },
	{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
	{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}


	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast< Color* >(
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

Rect Graphics::GetScreenRect()
{
	return{ 0.0f,float( ScreenWidth ),0.0f,float( ScreenHeight ) };
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast< Color* >( mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[y * dstPitch],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

void Graphics::PutPixel( int x,int y,Color c,unsigned char alpha )
{
	const Color srcPixel = c;
	const Color dstPixel = pSysBuffer[y * ScreenWidth + x];

	const Color blendedPixel = {
		unsigned char( ( dstPixel.GetR() + srcPixel.GetR() ) / 2 ),
		unsigned char( ( dstPixel.GetG() + srcPixel.GetG() ) / 2 ),
		unsigned char( ( dstPixel.GetB() + srcPixel.GetB() ) / 2 )
	};

	pSysBuffer[Graphics::ScreenWidth * y + x] = blendedPixel;
}

void Graphics::PutPixel( int x,int y,Color c,float alpha )
{
	PutPixel( x,y,c,unsigned char( alpha * 255 ) );
}

void Graphics::PutPixelT( int x,int y,Color c,float alpha )
{
	const Color srcPixel = c;
	const Color dstPixel = pSysBuffer[y * ScreenWidth + x];

	const Color blendedPixel =
	{
		unsigned char( ( srcPixel.GetR() * alpha +
		dstPixel.GetR() * ( 1.0f - alpha ) ) / 2.0f ),
		unsigned char( ( srcPixel.GetG() * alpha +
			dstPixel.GetG() * ( 1.0f - alpha ) ) / 2.0f ),
		unsigned char( ( srcPixel.GetB() * alpha +
			dstPixel.GetB() * ( 1.0f - alpha ) ) / 2.0f )
	};

	pSysBuffer[Graphics::ScreenWidth * y + x] = blendedPixel;
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

void Graphics::DrawRect( int x,int y,int width,int height,Color c )
{
	for( int i = y; i < y + height; ++i )
	{
		for( int j = x; j < x + width; ++j )
		{
			PutPixel( j,i,c );
		}
	}
}

void Graphics::DrawRectDim( int x1,int y1,int x2,int y2,Color c )
{
	if( x1 > x2 )
	{
		std::swap( x1,x2 );
	}
	if( y1 > y2 )
	{
		std::swap( y1,y2 );
	}
	for( int y = y1; y < y2; ++y )
	{
		for( int x = x1; x < x2; ++x )
		{
			PutPixel( x,y,c );
		}
	}
}

void Graphics::DrawRectSafe( int x,int y,int width,int height,Color c )
{
	for( int i = y; i < y + height; ++i )
	{
		for( int j = x; j < x + width; ++j )
		{
			if( j > 0 && j < ScreenWidth &&
				i > 0 && i < ScreenHeight )
			{
				PutPixel( j,i,c );
			}
		}
	}
}

void Graphics::DrawCircle( int x,int y,int radius,Color c )
{
	const int radSq = radius * radius;
	for( int i = y - radius; i < y + radius; ++i )
	{
		for( int j = x - radius; j < x + radius; ++j )
		{
			const int xDiff = x - j;
			const int yDiff = y - i;
			if( xDiff * xDiff + yDiff * yDiff < radSq )
			{
				PutPixel( j,i,c );
			}
		}
	}
}

void Graphics::DrawCircleSafe( int x,int y,int radius,Color c )
{
	const int radSq = radius * radius;
	for( int i = y - radius; i < y + radius; ++i )
	{
		for( int j = x - radius; j < x + radius; ++j )
		{
			const int xDiff = x - j;
			const int yDiff = y - i;
			if( xDiff * xDiff + yDiff * yDiff < radSq )
			{
				if( j > 0 && j < ScreenWidth &&
					i > 0 && i < ScreenHeight )
				{
					PutPixel( j,i,c );
				}
			}
		}
	}
}

void Graphics::DrawLine( int x1,int y1,int x2,int y2,Color c )
{
	if( !( x1 > 1 && x1 < ScreenWidth - 1 &&
		x2 > 1 && x2 < ScreenWidth - 1 &&
		y1 > 1 && y1 < ScreenHeight - 1 &&
		y2 > 1 && y2 < ScreenHeight - 1 ) )
	{
		return;
	}
	const float dx = float( x2 - x1 );
	const float dy = float( y2 - y1 );

	if( dy == 0.0f && dx == 0.0f )
	{
		PutPixel( int( x1 ),int( y1 ),c );
	}
	else if( abs( dy ) > abs( dx ) )
	{
		if( dy < 0.0f )
		{
			std::swap( x1,x2 );
			std::swap( y1,y2 );
		}

		const float m = dx / dy;
		float y = float( y1 );
		int lastIntY;
		for( float x = float( x1 );
			y < float( y2 );
			y += 1.0f,x += m )
		{
			lastIntY = int( y );
			PutPixel( int( x ),lastIntY,c );
		}
		if( int( y2 ) > lastIntY )
		{
			PutPixel( int( x2 ),int( y2 ),c );
		}
	}
	else
	{
		if( dx < 0.0f )
		{
			std::swap( x1,x2 );
			std::swap( y1,y2 );
		}

		const float m = dy / dx;
		float x = float( x1 );
		int lastIntX;
		for( float y = float( y1 );
			x < float( x2 );
			x += 1.0f,y += m )
		{
			lastIntX = int( x );
			PutPixel( lastIntX,int( y ),c );
		}
		if( int( x2 ) > lastIntX )
		{
			PutPixel( int( x2 ),int( y2 ),c );
		}
	}
}

void Graphics::DrawTransparentLine( const Vec2& pos1,const Vec2& pos2,float alpha,Color c )
{
	int x0 = int( pos1.x );
	int y0 = int( pos1.y );
	int x1 = int( pos2.x );
	int y1 = int( pos2.y );
	if( !( x0 > 1 && x0 < ScreenWidth - 1 &&
		x1 > 1 && x1 < ScreenWidth - 1 &&
		y0 > 1 && y0 < ScreenHeight - 1 &&
		y1 > 1 && y1 < ScreenHeight - 1 ) )
	{
		return;
	}
	bool steep = ( abs( y1 - y0 ) > abs( x1 - x0 ) );

	if( steep )
	{
		std::swap( x0,y0 );
		std::swap( x1,y1 );
	}
	if( x0 > x1 )
	{
		std::swap( x0,x1 );
		std::swap( y0,y1 );
	}

	float dx = float( x1 - x0 );
	float dy = float( y1 - y0 );
	float gradient = dy / dx;
	if( dx == 0.0 )
	{
		gradient = 1.0;
	}

	// handle first endpoint
	float xend = float( round( x0 ) );
	float yend = y0 + gradient * ( xend - x0 );
	float xgap = 1 - x0 + 0.5f - floor( x0 + 0.5f );
	float xpxl1 = xend; // this will be used in the main loop
	float ypxl1 = floor( yend );
	if( steep )
	{
		PutPixelT( int( ypxl1 ),int( xpxl1 ),c,alpha );
		PutPixelT( int( ypxl1 + 1 ),int( xpxl1 ),c,alpha );
	}
	else
	{
		PutPixelT( int( xpxl1 ),int( ypxl1 ),c,alpha );
		PutPixelT( int( xpxl1 ),int( ypxl1 + 1 ),c,alpha );
	}
	float intery = yend + gradient;
	xend = float( round( x1 ) );
	yend = y1 + gradient * ( xend - x1 );
	xgap = x1 + 0.5f - floor( x1 + 0.5f );
	float xpxl2 = xend; //this will be used in the main loop
	float ypxl2 = floor( yend );
	if( steep )
	{
		PutPixelT( int( ypxl2 ),int( xpxl2 ),c,alpha );
		PutPixelT( int( ypxl2 + 1 ),int( xpxl2 ),c,alpha );
	}
	else
	{
		PutPixelT( int( xpxl2 ),int( ypxl2 ),c,alpha );
		PutPixelT( int( xpxl2 ),int( ypxl2 + 1 ),c,alpha );
	}

	if( steep )
	{
		for( int x = int( xpxl1 + 1 ); x < int( xpxl2 - 1 ); ++x )
		{
			PutPixelT( int( floor( intery ) ),x,c,alpha );
			PutPixelT( int( floor( intery ) + 1 ),x,c,alpha );
			intery = intery + gradient;
		}
	}
	else
	{
		for( int x = int( xpxl1 + 1 ); x < int( xpxl2 - 1 ); ++x )
		{
			PutPixelT( x,int( floor( intery ) ),c,alpha );
			PutPixelT( x,int( floor( intery ) + 1 ),c,alpha );
			intery = intery + gradient;
		}
	}
}

void Graphics::DrawSpriteNonChroma( int x,int y,const Surface& s )
{
	DrawSpriteNonChroma( x,y,s.GetRect(),s );
}

void Graphics::DrawSpriteNonChroma( int x,int y,const Rect& srcRect,const Surface& s )
{
	DrawSpriteNonChroma( x,y,srcRect,GetScreenRect(),s );
}

void Graphics::DrawSpriteNonChroma( int x,int y,Rect srcRect,const Rect& clip,const Surface& s )
{
	assert( srcRect.left >= 0 );
	assert( srcRect.right <= s.GetWidth() );
	assert( srcRect.top >= 0 );
	assert( srcRect.bottom <= s.GetHeight() );
	if( x < clip.left )
	{
		srcRect.left += clip.left - float( x );
		x = int( clip.left );
	}
	if( y < clip.top )
	{
		srcRect.top += clip.top - float( y );
		y = int( clip.top );
	}
	if( x + srcRect.GetWidth() > clip.right )
	{
		srcRect.right -= x + srcRect.GetWidth() - clip.right;
	}
	if( y + srcRect.GetHeight() > clip.bottom )
	{
		srcRect.bottom -= y + srcRect.GetHeight() - clip.bottom;
	}
	for( int sy = int( srcRect.top ); sy < int( srcRect.bottom ); ++sy )
	{
		for( int sx = int( srcRect.left ); sx < int( srcRect.right ); ++sx )
		{
			PutPixel( x + int( sx - srcRect.left ),y + int( sy - srcRect.top ),s.GetPixel( sx,sy ) );
		}
	}
}

void Graphics::DrawSprite( int x,int y,const Surface& s,Color chroma )
{
	DrawSprite( x,y,s.GetRect(),s,chroma );
}

void Graphics::DrawSprite( int x,int y,const Rect& srcRect,const Surface& s,Color chroma )
{
	DrawSprite( x,y,srcRect,GetScreenRect(),s,chroma );
}

void Graphics::DrawSprite( int x,int y,Rect srcRect,const Rect& clip,const Surface& s,Color chroma )
{
	assert( srcRect.left >= 0 );
	assert( srcRect.right <= s.GetWidth() );
	assert( srcRect.top >= 0 );
	assert( srcRect.bottom <= s.GetHeight() );
	if( x < clip.left )
	{
		srcRect.left += clip.left - x;
		x = int( clip.left );
	}
	if( y < clip.top )
	{
		srcRect.top += clip.top - y;
		y = int( clip.top );
	}
	if( x + srcRect.GetWidth() > clip.right )
	{
		srcRect.right -= x + srcRect.GetWidth() - clip.right;
	}
	if( y + srcRect.GetHeight() > clip.bottom )
	{
		srcRect.bottom -= y + srcRect.GetHeight() - clip.bottom;
	}
	for( int sy = int( srcRect.top ); sy < int( srcRect.bottom ); ++sy )
	{
		for( int sx = int( srcRect.left ); sx < int( srcRect.right ); ++sx )
		{
			const Color srcPixel = s.GetPixel( sx,sy );
			if( srcPixel != chroma )
			{
				PutPixel( x + int( sx - srcRect.left ),y + int( sy - srcRect.top ),srcPixel );
			}
		}
	}
}

void Graphics::DrawHitbox( const Rect& hitbox,Color c )
{
	DrawLine( int( hitbox.left ),int( hitbox.top ),
		int( hitbox.right ),int( hitbox.top ),c );
	DrawLine( int( hitbox.right ),int( hitbox.top ),
		int( hitbox.right ),int( hitbox.bottom ),c );
	DrawLine( int( hitbox.right ),int( hitbox.bottom ),
		int( hitbox.left ),int( hitbox.bottom ),c );
	DrawLine( int( hitbox.left ),int( hitbox.bottom ),
		int( hitbox.left ),int( hitbox.top ),c );
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    ( !errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty )
		+ ( !errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty )
		+ ( !note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty )
		+ ( !location.empty() ? std::wstring( L"Location: " ) + location
			: empty );
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}