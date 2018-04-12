/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Game.cpp																			  *
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
 // Change to true for final build, false for building(it lags you a lot).
#include "META.h"

#include "MainWindow.h"
#include "Game.h"
#if releaseMode
#include "SpriteCodex.h"
#endif

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	js( Vec2{ float( Graphics::ScreenWidth - 70 ),70.0f } ),
	player( Vec2{ float( Graphics::ScreenWidth ),float( Graphics::ScreenHeight ) } / 2.0f ),
	pPowerup( new Powerup{ Vec2{ -9999.0f,-9999.0f },false } )
{
}

Game::~Game()
{
	// for( const Meatball* m : pMeatballs )
	// {
	// 	delete m;
	// }
	// 
	// delete pPowerup;
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if( !started )
	{
		if( wnd.mouse.LeftIsPressed() )
		{
			if( canStart )
			{
				started = true;
				canStart = false;
			}
		}
		else canStart = true;
	}
	else
	{
		const float dt = ft.Mark();

		if( wnd.mouse.RightIsPressed() )
		{
			// meatballs.emplace_back( Meatball{ rng,player.GetPos() } );
			// pastas.emplace_back( Pasta{ rng } );
			foods.emplace_back( std::make_unique<Pasta>( rng ) );
		}
		js.Update( wnd.mouse );
		player += js.GetDir();
		player.Update( wnd.mouse,dt );

		Food* playerTarget;
		bool targetSet = false;
		float minDist = 99999999.0f;
		// for( size_t i = 0; i < meatballs.size(); ++i )
		for( auto it = foods.begin(); it < foods.end(); ++it )
		{
			// Meatball& m = meatballs[i];
			auto& e = *it;

			// m.Target( player.GetPos() );
			e->Target( player.GetPos() );
			// m.Update( rng,dt );
			e->Update( rng,player.GetPos(),dt );

			// if( player.GetRect().IsOverlappingWith( m.GetRect() ) )
			if( player.GetRect().IsOverlappingWith( e->GetRect() ) )
			{
				ResetGame();
				return;
			}

			// if( player <= ( m.GetRect() ) )
			if( player <= ( e->GetRect() ) )
			{
				// m.Damage( player.GetDamage(),pPowerup.get(),rng );
				e->Damage( player.GetDamage(),pPowerup.get(),rng );
			}

			if( ( *e ) && Vec2{ e->GetPos() - player.GetPos() }
				.GetLengthSq() < minDist &&
				Rect{ e->GetPos(),1.0f,1.0f }
				.IsContainedBy( Graphics::GetScreenRect() ) )
			{ // Target closest within range and screen.
				minDist = Vec2{ e->GetPos() - player.GetPos() }.GetLengthSq();
				playerTarget = e.get();
				targetSet = true;
			}

			if( !( *e ) )
			{
				// delete m;
				// meatballs.erase( meatballs.begin() + i );
				it = foods.erase( it );
				if( it == foods.end() ) return;
			}
		}

		if( targetSet && minDist < player.GetRange() * player.GetRange() )
		{
			player >> playerTarget->GetPos();
		}

		pPowerup->Update( dt );

		bool enemiesLeft = false;
		// for( const Meatball& m : meatballs )
		for( const auto& e : foods )
		{
			enemiesLeft = true;
			break;
		}
		if( !enemiesLeft )
		{
			AdvanceLevel();
		}

		if( player.GetRect().IsOverlappingWith( pPowerup->GetRect() ) )
		{
			player.PowerUp();
			pPowerup->Reset( { 9999.0f,9999.0f } );
		}

		// if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
		// {
		// 	int* pTest = new int[500000];
		// }
	}
}

void Game::AdvanceLevel()
{
	++level;
	for( int i = 0; i < int( float( nEnemies ) * 0.6f ); ++i )
	{
		foods.emplace_back( std::make_unique<Meatball>( rng,player.GetPos() ) );
	}
	for( int i = 0; i < int( float( nEnemies ) * 0.4f ); ++i )
	{
		foods.emplace_back( std::make_unique<Pasta>( rng ) );
	}
	nEnemies += int( float( nEnemies ) * 1.1f );
}

void Game::ResetGame()
{
	pPowerup->Reset( { 9999.0f,9999.0f } );
	player.Reset();

	level = 1;
	nEnemies = 10;

	started = false;
	canStart = false;

	// meatballs.clear();
	// pastas.clear();
	foods.clear();
}

void Game::ComposeFrame()
{
	if( !started )
	{
#if releaseMode
		SpriteCodex::DrawTitleScreen( gfx );
#endif
	}
	else
	{
		// for( const Meatball& m : meatballs )
		for( const auto& e : foods )
		{
			e->Draw( gfx );
		}

		pPowerup->Draw( gfx );

		player.Draw( gfx );
		js.Draw( gfx );
	}
}
