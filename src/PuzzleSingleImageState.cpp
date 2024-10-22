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
#include "SagoImGui.hpp"

PuzzleSingleImageState::PuzzleSingleImageState() {
	
}

PuzzleSingleImageState::~PuzzleSingleImageState() {
	ClearPicture();
}

bool PuzzleSingleImageState::IsActive() {
	return isActive;
}

bool isEscapeEvent(const SDL_Event& event) {
        if ( event.type == SDL_KEYDOWN ) {
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                        return true;
                }
        }
        if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B || event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK ) {
                        return true;
                }
        }
        return false;
}

void PuzzleSingleImageState::ProcessInput(const SDL_Event& event, bool &processed) {
	if (isEscapeEvent(event)) {
		isActive = false;
		processed = true;
	}
	if (event.type == SDL_WINDOWEVENT) {
		if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			ResizeImagePhysical();
		}
	}
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void PuzzleSingleImageState::Draw(SDL_Renderer* target) {
	SDL_Rect rect;
	rect.x = globalData.xsize/2-resized_image_physical_width/2;
	rect.y = globalData.ysize/2-resized_image_physical_height/2;
	rect.h = resized_image_physical_height;
	rect.w = resized_image_physical_width;
	if (!shuffeled) {
		SDL_RenderCopy(target, this->pictureTex, NULL, &rect);
	}
	else {
		for (size_t i = 0; i < pieces_physical.size(); ++i) {
			SDL_Rect destination = pieces_physical[i];
			destination.x += rect.x;
			destination.y += rect.y;
			SDL_Rect source = pieces_logical.at(shuffeled_pieces[i]);
			double scale = double(source_image_height)/double(resized_image_logical_height);
			source.x = double(source.x)*scale;
			source.y = double(source.y)*scale;
			source.w = double(source.w)*scale;
			source.h = double(source.h)*scale;
			int flip = static_cast<int>(SDL_FLIP_NONE);
			if (rotated_pieces[i] == 1 || rotated_pieces[i] == 3) {
				flip |= SDL_FLIP_HORIZONTAL;
			}
			if (rotated_pieces[i] == 2 || rotated_pieces[i] == 3) {
				flip |= SDL_FLIP_VERTICAL;
			}
			SDL_RenderCopyEx(target, this->pictureTex, &source, &destination, 0, nullptr, static_cast<SDL_RendererFlip>(flip) );
		}
		for (size_t i = 0; i < pieces_physical.size(); ++i) {
			const SDL_Rect& piece = pieces_physical[i];
			if (i == marked_piece) {
				continue;
			}
			rectangleRGBA(target, rect.x+piece.x, rect.y+piece.y,
								rect.x+piece.x + piece.w, rect.y+piece.y + piece.h, 255, 255, 0, 255);
		}
		if (marked_piece > -1 && marked_piece < pieces_physical.size()) {
			const SDL_Rect& piece = pieces_physical.at(marked_piece);
			rectangleRGBA(target, rect.x+piece.x, rect.y+piece.y,
								rect.x+piece.x + piece.w, rect.y+piece.y + piece.h, 255, 0, 0, 255);
		}
	}


	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Close")) {
			isActive = false;
		}
		if (ImGui::MenuItem("Shuffle")) {
			Shuffle();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}


void PuzzleSingleImageState::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		globalData.mouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && globalData.mouseUp) {
		globalData.mouseUp = false;
		
		SDL_Rect rect;
		rect.x = globalData.xsize/2-resized_image_physical_width/2;
		rect.y = globalData.ysize/2-resized_image_physical_height/2;;
		rect.h = resized_image_physical_height;
		rect.w = resized_image_physical_width;
		for (size_t i = 0; i< pieces_physical.size(); ++i) {
			const SDL_Rect& piece = pieces_physical[i];
			bool clicked = (globalData.mousex > rect.x+piece.x && globalData.mousex < rect.x+piece.x + piece.w && globalData.mousey > rect.y+piece.y && globalData.mousey < rect.y+piece.y + piece.h);
			if (clicked) {
				if (flipMode) {
					rotated_pieces[i] = (rotated_pieces[i]+1)%4;
				}
				else {
					if (i != marked_piece && marked_piece > -1 && marked_piece < pieces_physical.size()) {
						std::swap(shuffeled_pieces[i], shuffeled_pieces[marked_piece]);
						marked_piece = -1;
					}
					else if (i == marked_piece) {
						marked_piece = -1;
					}
					else {
						marked_piece = i;
					}
				}
			}
		}
		CheckSolved();
	}
}

void PuzzleSingleImageState::CreatePhysicalPieces() {
	pieces_physical.clear();
	for (size_t i = 0; i < pieces_logical.size(); ++i) {
		SDL_Rect piece = pieces_logical.at(i);
		piece.x = (piece.x) * resized_image_physical_width / resized_image_logical_width;
		piece.y = (piece.y) * resized_image_physical_height / resized_image_logical_height;
		piece.w = piece.w * resized_image_physical_width / resized_image_logical_width;
		piece.h = piece.h * resized_image_physical_height / resized_image_logical_height;
		pieces_physical.push_back(piece);
	}
}

void PuzzleSingleImageState::ResizeImage() {
	resized_image_logical_height = resized_image_logical_height_max;
	resized_image_logical_width = double(resized_image_logical_height) * (double(source_image_width)/double(source_image_height));
	if (resized_image_logical_width > resized_image_logical_width_max) {
		resized_image_logical_width = resized_image_logical_width_max;
		resized_image_logical_height = double(resized_image_logical_width) * (double(source_image_height)/double(source_image_width));
	}
	// Set default physical size to logical size
	resized_image_physical_width = resized_image_logical_width;
	resized_image_physical_height = resized_image_logical_height;
}

void PuzzleSingleImageState::ResizeImagePhysical() {
	int resized_image_physical_height_max = globalData.ysize-70;
	int resized_image_physical_width_max = globalData.xsize-70;
	resized_image_physical_height = resized_image_physical_height_max;
	resized_image_physical_width = double(resized_image_physical_height) * (double(source_image_width)/double(source_image_height));
	if (resized_image_physical_width > resized_image_physical_width_max) {
		resized_image_physical_width = resized_image_physical_width_max;
		resized_image_physical_height = double(resized_image_physical_width) * (double(source_image_height)/double(source_image_width));
	}
	CreatePhysicalPieces();
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
	ResizeImage();
	this->pictureTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	std::cerr << resized_image_logical_width << ", " << resized_image_logical_height << "\n";
	SDL_FreeSurface(bitmapSurface);
	pieces_logical.clear();
	SDL_Rect piece;
	piece.x = 0;
	piece.y = 0;
	piece.h = resized_image_logical_height;
	piece.w = resized_image_logical_width;
	pieces_logical.push_back(piece);
	for (int i = 0; i<10; ++i) {
		SplitPiece();
	}
	ResizeImagePhysical();
	Shuffle();
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
	SDL_Rect piece1 = pieces_logical.at(piece_number);
	if (piece1.w > 2*min_piece_size) {
		int new_piece_width = min_piece_size + (rand()%(piece1.w-2*min_piece_size));
		SDL_Rect piece2 = piece1;
		piece2.w = new_piece_width;
		piece1.w -= new_piece_width;
		piece2.x = piece2.x + piece1.w;
		pieces_logical.at(piece_number) = piece1;
		pieces_logical.push_back(piece2);
		return;
	}
}

void PuzzleSingleImageState::SplitPieceHorisontal(size_t piece_number) {
	SDL_Rect piece1 = pieces_logical.at(piece_number);
	if (piece1.h > 2*min_piece_size) {
		int new_piece_height = min_piece_size + (rand()%(piece1.h-2*min_piece_size));
		SDL_Rect piece2 = piece1;
		piece2.h = new_piece_height;
		piece1.h -= new_piece_height;
		piece2.y = piece2.y + piece1.h;
		pieces_logical.at(piece_number) = piece1;
		pieces_logical.push_back(piece2);
	}
}


void PuzzleSingleImageState::SplitPiece() {
	size_t number_of_pieces = pieces_logical.size();
	for (size_t i= 0; i < number_of_pieces; ++i ) {
		const SDL_Rect& piece = pieces_logical.at(i);
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

void PuzzleSingleImageState::Shuffle() {
	size_t number_of_peices = pieces_logical.size();
	shuffeled_pieces.resize(number_of_peices);
	rotated_pieces.resize(number_of_peices);
	for (size_t i = 0; i< shuffeled_pieces.size(); ++i) {
		shuffeled_pieces.at(i) = i;
	}
	if (flipMode) {
		for (size_t i = 0; i< shuffeled_pieces.size(); ++i) {
			rotated_pieces.at(i) = rand()%4;
		}
	}
	else {
		for (int i = 0 ; i < 100; ++i) {
			size_t first = rand()%number_of_peices;
			size_t second = rand()%number_of_peices;
			std::swap(shuffeled_pieces[first], shuffeled_pieces[second]);
		}
	}
	shuffeled = true;
}


void PuzzleSingleImageState::CheckSolved() {
	for (size_t i=0; i < shuffeled_pieces.size(); ++i) {
		if (rotated_pieces[i] != 0) {
			return;
		}
		if (shuffeled_pieces[i] != i) {
			return;
		}
	}
	shuffeled = false;
}