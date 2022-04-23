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
#include "globals.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>

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
	globalData.spriteHolder->GetSprite("background_sixteen_nine").Draw(target, SDL_GetTicks(), 0, 0);
	SDL_Rect rect;
	rect.x = globalData.xsize/2-resized_image_width/2;
	rect.y = globalData.ysize/2-resized_image_height/2;;
	rect.h = resized_image_height;
	rect.w = resized_image_width;
	SDL_RenderCopy(target, this->pictureTex, NULL, &rect);
	for (const SDL_Rect& piece : pieces) {
		rectangleRGBA(target, rect.x+piece.x, rect.y+piece.y,
							rect.x+piece.x + piece.w, rect.y+piece.y + piece.h, 255, 255, 0, 255);
	}
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
	pieces.clear();
	SDL_Rect piece;
	piece.x = 0;
	piece.y = 0;
	piece.h = resized_image_height;
	piece.w = resized_image_width;
	pieces.push_back(piece);
	for (int i = 0; i<10; ++i) {
		SplitPiece();
	}
}

void PuzzleSingleImageState::SplitPiece(size_t piece_number) {
	srand(time(NULL));
	if (rand()%2==1) {
		SplitPieceVertical(piece_number);
		SplitPieceHorisontal(piece_number);
		return;
	}
	SplitPieceHorisontal(piece_number);
	SplitPieceVertical(piece_number);
}

void PuzzleSingleImageState::SplitPieceVertical(size_t piece_number) {
	SDL_Rect piece1 = pieces.at(piece_number);
	if (piece1.w > 2*min_piece_size) {
		int new_piece_width = min_piece_size + (rand()%(piece1.w-2*min_piece_size));
		SDL_Rect piece2 = piece1;
		piece2.w = new_piece_width;
		piece1.w -= new_piece_width;
		piece2.x = piece2.x + piece1.w;
		pieces.at(piece_number) = piece1;
		pieces.push_back(piece2);
		return;
	}
}

void PuzzleSingleImageState::SplitPieceHorisontal(size_t piece_number) {
	SDL_Rect piece1 = pieces.at(piece_number);
	if (piece1.h > 2*min_piece_size) {
		int new_piece_height = min_piece_size + (rand()%(piece1.h-2*min_piece_size));
		SDL_Rect piece2 = piece1;
		piece2.h = new_piece_height;
		piece1.h -= new_piece_height;
		piece2.y = piece2.y + piece1.h;
		pieces.at(piece_number) = piece1;
		pieces.push_back(piece2);
	}
}


void PuzzleSingleImageState::SplitPiece() {
	size_t number_of_pieces = pieces.size();
	for (size_t i= 0; i < number_of_pieces; ++i ) {
		const SDL_Rect& piece = pieces.at(i);
		if (piece.w > 2*min_piece_size || piece.h > 2*min_piece_size) {
			SplitPiece(i);
		}
	}
}

void PuzzleSingleImageState::ClearPicture() {
	if (this->pictureTex) {
		SDL_DestroyTexture(this->pictureTex);
		this->pictureTex = nullptr;
	}
}