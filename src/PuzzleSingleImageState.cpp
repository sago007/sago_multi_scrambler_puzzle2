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

#include "PuzzleSingleImageState.hpp"
#include "SDL_image.h"
#include <iostream>

PuzzleSingleImageState::PuzzleSingleImageState() {
	
}

PuzzleSingleImageState::~PuzzleSingleImageState() {
	ClearPicture();
}

bool PuzzleSingleImageState::IsActive() {
    return true;
}

void PuzzleSingleImageState::ProcessInput(const SDL_Event& event, bool &processed) {

}

void PuzzleSingleImageState::Draw(SDL_Renderer* target) {
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.h = resized_image_height;
	rect.w = resized_image_width;
	SDL_RenderCopy(target, this->pictureTex, &rect, NULL);
}


void PuzzleSingleImageState::Update() {

}

void PuzzleSingleImageState::LoadPictureFromFile(const std::string& filename, SDL_Renderer* renderer) {
	ClearPicture();
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
	SDL_Surface* bitmapSurface = IMG_Load(filename.c_str());
	if (!bitmapSurface) {
		std::cerr << "Failed to load " << filename << std::endl;
		return;
	}
	source_image_height = bitmapSurface->h;
	source_image_width = bitmapSurface->w;
	resized_image_width = double(resized_image_height) * (double(source_image_width)/double(source_image_height));
	this->pictureTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	std::cerr << resized_image_width << ", " << resized_image_height << "\n";
	SDL_FreeSurface(bitmapSurface);
}

void PuzzleSingleImageState::ClearPicture() {
	if (this->pictureTex) {
		SDL_DestroyTexture(this->pictureTex);
		this->pictureTex = nullptr;
	}
}