/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/saland
===========================================================================
*/

#include "sago/GameStateInterface.hpp"

#ifndef MAINGAMESTATE_HPP
#define MAINGAMESTATE_HPP

class MainGameState : public sago::GameStateInterface {
public:
	MainGameState();
	MainGameState(const MainGameState& orig) = delete;
	virtual ~MainGameState();
	
	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

private:
	bool isActive = true;
};

void DrawRectWhite(SDL_Renderer* target, int topx, int topy, int height, int width);
void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width);

#endif /* MAINGAMESTATE_HPP */

