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

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "sago/SagoSpriteHolder.hpp"
#include <memory>


struct GlobalData {
	bool isShuttingDown = false;
	bool fullscreen = false;
	bool resetVideo = false;
	bool SoundEnabled = true;
	bool NoSound = false;
	SDL_Renderer* screen = nullptr;
	std::unique_ptr<sago::SagoSpriteHolder> spriteHolder;
	sago::SagoDataHolder* dataHolder;
	sago::SagoSprite mouse;
	int verboseLevel = 0;
	int mousex = 0;
	int mousey = 0;
	bool mouseUp = false;
	int xsize = 1366;
	int ysize = 768;
};

const char* const GAMENAME = "Sago's Multi Scrambler Puzzle II";

extern GlobalData globalData;

#endif /* GLOBALS_HPP */

