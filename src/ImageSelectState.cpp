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

#include "ImageSelectState.hpp"

ImageSelectState::ImageSelectState() {
}


ImageSelectState::~ImageSelectState() {
}

bool ImageSelectState::IsActive() {
	return true;
}

void ImageSelectState::ProcessInput(const SDL_Event& event, bool &processed) {

}

void ImageSelectState::Draw(SDL_Renderer* target) {

}


void ImageSelectState::Update() {

}