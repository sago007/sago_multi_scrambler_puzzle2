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

#ifndef SAGO_COMMON_HPP
#define SAGO_COMMON_HPP


void InitSagoFS(int argc, const char* argv[]);
void RunGameState(sago::GameStateInterface& state );
void UpdateMouseCoordinates(const SDL_Event& event, int& mousex, int& mousey);
void InitGame();
void UninitGame();
void DrawBackground(SDL_Renderer* target);

#endif /* SAGO_COMMON_HPP */

