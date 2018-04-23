/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Game.h																				  *
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
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Joystick.h"
#include "Hero.h"
#include <vector>
#include "Enemy.h"
#include "Random.h"
#include "FrameTimer.h"
#include "Powerup.h"
#include <memory>
#include "Recharger.h"
#include "Font.h"

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	void AdvanceLevel();
	void ResetGame();
	/********************************/
private:
	MainWindow & wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	Random rng;
	FrameTimer ft;
	Font consolas = Font( "Fonts/Consolas.bmp" );
	Joystick js;
	Hero player;

	int level = 1;
	int nEnemies = 10;
	std::vector<std::unique_ptr<Food>> foods;

	std::unique_ptr<Powerup> pPowerup = std
		::make_unique<Powerup>( Vec2( 9999.0f,9999.0f ),
			false );
	std::unique_ptr<Recharger> pRecharger = std
		::make_unique<Recharger>( Vec2( 9999.0f,9999.0f ),
			false );
	bool started = false;
	bool canStart = false;
	/********************************/
};