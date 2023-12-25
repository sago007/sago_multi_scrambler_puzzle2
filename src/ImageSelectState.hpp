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
#include <string>
#include <vector>


#pragma once

class ImageSelectState : public sago::GameStateInterface {
public:
	ImageSelectState();
	ImageSelectState(const ImageSelectState& orig) = delete;
	virtual ~ImageSelectState();
	
	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;
	void Init();

	//Set to a filesystem folder to use that folder. Set to empty string to use collections.
	std::string folder = "";
	std::string collection = "";
	std::vector<std::string> imageList;

private:
	bool isActive = true;
};

