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
#if DRAW_RELEASE_STUFF
#include "SpriteCodex.h"
#endif

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	js( Vec2{ float( Graphics::ScreenWidth - 70 ),70.0f } ),
	player( Vec2{ float( Graphics::ScreenWidth ),
		float( Graphics::ScreenHeight ) } / 2.0f )
{
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
	if( wnd.kbd.KeyIsPressed( VK_CONTROL ) &&
		wnd.kbd.KeyIsPressed( 'W' ) )
	{
		// Bop!
		wnd.Kill();
	}
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
		float speedupFactor = 1.0f;
		// float speedupFactor = 3.0f;
		// if( wnd.kbd.KeyIsPressed( 'A' ) ) speedupFactor = 1.0f;
		// if( wnd.kbd.KeyIsPressed( 'D' ) ) speedupFactor = 5.0f;
		// if( wnd.kbd.KeyIsPressed( 'W' ) ) pPowerup->Reset( player.GetPos() );
		// if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
		// {
		// 	int* pTest = new int[500000];
		// }
		if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
		{
			const auto ff = level;
			const auto fb = nEnemies;

			const int fff = 2;
		}

		const float dt = ft.Mark() * speedupFactor;

		if( wnd.mouse.RightIsPressed() )
		{
			foods.emplace_back( std
				::make_unique<Orange>( rng ) );
		}
		js.Update( wnd.mouse );
		player += js.GetDir();
		player.Update( wnd.mouse,dt );

		// Always initialize pointer to nullptr!
		Food* playerTarget = nullptr;
		bool targetSet = false;
		float minDist = 99999999.0f;
		std::vector<DuoVec2> tempVec;
		for( auto it = foods.begin();
			it < foods.end();
			++it )
		{
			auto& e = *it;

			e->Target( player.GetPos() );

			e->Update( rng,player.GetPos(),dt );

			if( player.GetRect()
				.IsOverlappingWith( e->GetRect() ) )
			{
				// ResetGame();
				player.Attack();
				// Don't get (possibly) infinite health
				//  by running into enemies and getting
				//  them to drop health.
				e->Destroy( pPowerup.get(),rng );
				if( player.GetHP() < 1 )
				{
					ResetGame();
					return;
				}
			}

			if( player <= ( e->GetRect() ) )
			{
				e->Damage( player.GetDamage(),
					pPowerup.get(),
					pRecharger.get(),rng );
			}

			if( ( *e ) && Vec2{ e->GetPos() - player.GetPos() }
				.GetLengthSq() < minDist &&
				Rect{ e->GetPos(),1.0f,1.0f }
				.IsContainedBy( Graphics::GetScreenRect() ) )
			{ // Target closest within range and screen.
				minDist = Vec2{ e->GetPos() -
					player.GetPos() }.GetLengthSq();
				playerTarget = e.get();
				targetSet = true;
			}

			if( !( *e ) )
			{
				e->EndRoutine( tempVec );
				it = foods.erase( it );
				if( it == foods.end() ) return;
			}
		}

		for( const auto& x : tempVec )
		{
			foods.emplace_back( std
				::make_unique<OrangeSlice>( x ) );
		}

		if( targetSet && minDist < player.GetRange() *
			player.GetRange() )
		{
			player >> playerTarget->GetPos();
		}

		pPowerup->Update( dt );
		pRecharger->Update( pPowerup.get(),
			!player.IsMaxHP(),dt );

		bool enemiesLeft = false;
		for( const auto& e : foods )
		{
			enemiesLeft = true;
			break;
		}
		if( !enemiesLeft ) AdvanceLevel();

		if( player.GetRect().IsOverlappingWith( pPowerup
			->GetRect() ) )
		{
			player.PowerUp();
			pPowerup->Reset( { 9999.0f,9999.0f } );
		}

		if( player.GetRect().IsOverlappingWith( pRecharger
			->GetRect() ) )
		{
			player.Heal();
			pRecharger->Reset( { 9999.0f,9999.0f } );
		}
	}
}

void Game::AdvanceLevel()
{
	++level;
	int dist1 = 0;
	int dist2 = 0;
	int dist3 = 0;
	{
		if( level > 0 )
		{
			dist1 = int( nEnemies * 1.0f );
			dist2 = int( nEnemies * 0.0f );
			dist3 = int( nEnemies * 0.0f );
		}
		if( level > 3 )
		{
			dist1 = int( nEnemies * 0.35f );
			dist2 = int( nEnemies * 0.15f );
			dist3 = int( nEnemies * 0.0f );
		}
		if( level > 5 )
		{
			dist1 = int( nEnemies * 0.05f );
			dist2 = int( nEnemies * 0.07f );
			dist3 = int( nEnemies * 0.03f );
			nEnemies = int( float( nEnemies ) * 0.85f );
		}
		if( level > 8 )
		{
			dist1 = int( nEnemies * 0.3f );
			dist2 = int( nEnemies * 0.4f );
			dist3 = int( nEnemies * 0.3f );
		}
	}

	{
		for( int i = 0; i < dist1; ++i )
		{
			foods.emplace_back( std::make_unique<Meatball>( rng,
				player.GetPos() ) );
		}
		for( int i = 0; i < dist2; ++i )
		{
			foods.emplace_back( std::make_unique<Pasta>( rng ) );
		}
		for( int i = 0; i < dist3; ++i )
		{
			foods.emplace_back( std::make_unique<Orange>( rng ) );
		}
	}

	if( level <= 5 )
	{
		nEnemies += int( float( nEnemies ) * 1.1f );
	}
	else
	{
		nEnemies += int( float( nEnemies ) * 1.01f );
	}
}

void Game::ResetGame()
{
	pPowerup->Reset( { 9999.0f,9999.0f } );
	pRecharger->Reset( { 9999.0f,9999.0f } );
	player.Reset();

	level = 1;
	nEnemies = 10;

	started = false;
	canStart = false;

	foods.clear();
	js.Reset();
}

void Game::ComposeFrame()
{
	if( !started )
	{
#if DRAW_RELEASE_STUFF
		SpriteCodex::DrawTitleScreen( gfx );
#endif
}
	else
	{
		for( const auto& e : foods )
		{
			e->Draw( gfx );
		}

		pPowerup->Draw( gfx );
		pRecharger->Draw( gfx );

		player.Draw( gfx );
		js.Draw( gfx );
	}
}
