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
#include "PuzzlePieceEditorState.hpp"
#include "SDL_image.h"
#include <iostream>
#include "globals.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>
#include <memory>
#include <filesystem>
#include "SagoImGui.hpp"
#include "sago/SagoMisc.hpp"
#include "rhash.hpp"
#include "config.hpp"
#include "sago_common.hpp"

PuzzleSingleImageState::PuzzleSingleImageState() {
	std::map<std::string, std::string> config = LoadConfigMap();
	flipMode = GetConfigBool(config, "flipMode", false);
	rectangularMode = GetConfigBool(config, "rectangularMode", false);
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

void PuzzleSingleImageState::ProcessInput(const SDL_Event& event, bool& processed) {
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
		// In rectangular mode, draw the original image as background with cutouts
		if (rectangularMode) {
			// Draw a black background first
			SDL_SetRenderDrawColor(target, 0, 0, 0, 255);
			SDL_RenderFillRect(target, &rect);

			// Draw the full background image slightly dimmed
			SDL_SetTextureAlphaMod(this->pictureTex, 128);
			SDL_RenderCopy(target, this->pictureTex, NULL, &rect);
			SDL_SetTextureAlphaMod(this->pictureTex, 255);

			// Draw black rectangles where the pieces should go (cutouts)
			for (size_t i = 0; i < pieces_physical.size(); ++i) {
				const SDL_Rect& piece = pieces_physical[i];
				SDL_Rect cutout;
				cutout.x = rect.x + piece.x;
				cutout.y = rect.y + piece.y;
				cutout.w = piece.w;
				cutout.h = piece.h;
				SDL_SetRenderDrawColor(target, 0, 0, 0, 255);
				SDL_RenderFillRect(target, &cutout);
			}
		}

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

			// Apply fade animation if this piece is animating
			if (i == swapAnimatingPiece1 || i == swapAnimatingPiece2) {
				Uint8 alpha = 255;
				alpha = static_cast<Uint8>(255 * ((swapAnimationTime - swapAnimationDuration) / swapAnimationDuration));
				SDL_SetTextureAlphaMod(this->pictureTex, alpha);
			}
			else {
				SDL_SetTextureAlphaMod(this->pictureTex, 255);
			}

			SDL_RenderCopyEx(target, this->pictureTex, &source, &destination, 0, nullptr, static_cast<SDL_RendererFlip>(flip) );
		}
		// Reset alpha to full after drawing all pieces
		SDL_SetTextureAlphaMod(this->pictureTex, 255);
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
		if (rectangularMode && ImGui::MenuItem("Edit Piece Layout")) {
			shouldLaunchEditor = true;
		}
		if (imageFilePath.length()) {
			bool isFav = IsFavorite(imageFilePath);
			const char* favoriteText = isFav ? "Remove from Favorites" : "Add to Favorites";
			if (ImGui::MenuItem(favoriteText)) {
				if (isFav) {
					RemoveFavorite(imageFilePath);
				} else {
					AddFavorite(imageFilePath);
				}
			}
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Settings")) {
		if (ImGui::MenuItem("Flip Mode", nullptr, flipMode)) {
			flipMode = !flipMode;
			// Save the updated configuration
			std::map<std::string, std::string> config = LoadConfigMap();
			SetConfigBool(config, "flipMode", flipMode);
			SaveConfigMap(config);
			Shuffle();
		}
		if (ImGui::MenuItem("Rectangular Mode", nullptr, rectangularMode)) {
			rectangularMode = !rectangularMode;
			// Save the updated configuration
			std::map<std::string, std::string> config = LoadConfigMap();
			SetConfigBool(config, "rectangularMode", rectangularMode);
			SaveConfigMap(config);
			// Note: Need to reload the image to apply the new piece generation mode
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	confetti.Draw(target);
}


void PuzzleSingleImageState::Update() {
	// Launch editor if requested (must be done outside of Draw/ImGui frame)
	if (shouldLaunchEditor) {
		shouldLaunchEditor = false;
		LaunchPieceEditor();
		return;
	}

	static Uint32 lastTime = SDL_GetTicks();
	Uint32 currentTime = SDL_GetTicks();
	float deltaTime = (currentTime - lastTime) / 1000.0f;
	lastTime = currentTime;
	confetti.Update(deltaTime);

	// Update swap animation
	if (swapAnimatingPiece1 != -1 && swapAnimatingPiece2 != -1) {
		swapAnimationTime += deltaTime;
		if (swapAnimationTime >= swapAnimationDuration) {
			// Animation complete, reset
			swapAnimatingPiece1 = -1;
			swapAnimatingPiece2 = -1;
			swapAnimationTime = 0.0f;
		}
	}

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
						// Start swap animation
						swapAnimatingPiece1 = i;
						swapAnimatingPiece2 = marked_piece;
						swapAnimationTime = 0.0f;
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
	imageFilePath = std::filesystem::absolute(filename).string();
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
	SDL_Surface* bitmapSurface = IMG_Load(filename.c_str());
	if (!bitmapSurface) {
		std::cerr << "Failed to load " << filename << std::endl;
		return;
	}
	RHash h(RHASH_SHA256);
	h.update(filename);
	picture_id = h.hex(RHASH_SHA256);
	source_image_height = bitmapSurface->h;
	source_image_width = bitmapSurface->w;
	ResizeImage();
	this->pictureTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	std::cerr << resized_image_logical_width << ", " << resized_image_logical_height << ", id: " << picture_id << ", file: " << filename << "\n";
	SDL_FreeSurface(bitmapSurface);
	SDL_SetTextureBlendMode(this->pictureTex, SDL_BLENDMODE_BLEND);
	pieces_logical.clear();

	if (rectangularMode) {
		// Try to load custom piece layout first
		if (!LoadCustomPieceLayout(picture_id, pieces_logical)) {
			// Create a 4x4 grid of rectangular pieces if no custom layout exists
			CreateRectangularPieces(4, 4);
		}
		else {
			std::cerr << "Loaded custom piece layout with " << pieces_logical.size() << " pieces\n";
		}
	}
	else {
		// Use the original splitting algorithm
		SDL_Rect piece;
		piece.x = 0;
		piece.y = 0;
		piece.h = resized_image_logical_height;
		piece.w = resized_image_logical_width;
		pieces_logical.push_back(piece);
		for (int i = 0; i<10; ++i) {
			SplitPiece();
		}
	}

	ResizeImagePhysical();
	Shuffle();
}

void PuzzleSingleImageState::LoadPictureFromPhysFS(const std::string& physfsPath, SDL_Renderer* renderer) {
	ClearPicture();
	imageFilePath = physfsPath;
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
	std::unique_ptr<char[]> data;
	unsigned int bytes = 0;
	sago::ReadBytesFromFile(physfsPath.c_str(), data, bytes);
	if (!data || bytes == 0) {
		std::cerr << "Failed to read " << physfsPath << " from PhysFS" << std::endl;
		return;
	}
	SDL_RWops* rw = SDL_RWFromMem(data.get(), bytes);
	SDL_Surface* bitmapSurface = IMG_Load_RW(rw, 0);
	SDL_RWclose(rw);
	if (!bitmapSurface) {
		std::cerr << "Failed to load " << physfsPath << std::endl;
		return;
	}
	RHash h(RHASH_SHA256);
	h.update(physfsPath);
	picture_id = h.hex(RHASH_SHA256);
	source_image_height = bitmapSurface->h;
	source_image_width = bitmapSurface->w;
	ResizeImage();
	this->pictureTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	std::cerr << resized_image_logical_width << ", " << resized_image_logical_height << ", id: " << picture_id << ", file: " << physfsPath << "\n";
	SDL_FreeSurface(bitmapSurface);
	SDL_SetTextureBlendMode(this->pictureTex, SDL_BLENDMODE_BLEND);
	pieces_logical.clear();

	if (rectangularMode) {
		if (!LoadCustomPieceLayout(picture_id, pieces_logical)) {
			CreateRectangularPieces(4, 4);
		}
		else {
			std::cerr << "Loaded custom piece layout with " << pieces_logical.size() << " pieces\n";
		}
	}
	else {
		SDL_Rect piece;
		piece.x = 0;
		piece.y = 0;
		piece.h = resized_image_logical_height;
		piece.w = resized_image_logical_width;
		pieces_logical.push_back(piece);
		for (int i = 0; i<10; ++i) {
			SplitPiece();
		}
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

void PuzzleSingleImageState::CreateRectangularPieces(int rows, int cols) {
	pieces_logical.clear();
	srand(time(NULL));

	int num_pieces = rows * cols;
	int attempts = 0;
	const int max_attempts = num_pieces * 100;

	// Try to place non-overlapping rectangles
	while (pieces_logical.size() < num_pieces && attempts < max_attempts) {
		attempts++;

		// Random size within reasonable bounds
		int piece_width = min_piece_size + rand() % (resized_image_logical_width / 3);
		int piece_height = min_piece_size + rand() % (resized_image_logical_height / 3);

		// Ensure pieces aren't too large
		if (piece_width > resized_image_logical_width / 2) {
			piece_width = resized_image_logical_width / 2;
		}
		if (piece_height > resized_image_logical_height / 2) {
			piece_height = resized_image_logical_height / 2;
		}

		// Random position
		int max_x = resized_image_logical_width - piece_width;
		int max_y = resized_image_logical_height - piece_height;
		if (max_x < 0) {
			max_x = 0;
		}
		if (max_y < 0) {
			max_y = 0;
		}

		int piece_x = rand() % (max_x + 1);
		int piece_y = rand() % (max_y + 1);

		SDL_Rect candidate;
		candidate.x = piece_x;
		candidate.y = piece_y;
		candidate.w = piece_width;
		candidate.h = piece_height;

		// Check for overlaps with existing pieces
		bool overlaps = false;
		for (const SDL_Rect& existing : pieces_logical) {
			if (SDL_HasIntersection(&candidate, &existing)) {
				overlaps = true;
				break;
			}
		}

		if (!overlaps) {
			pieces_logical.push_back(candidate);
		}
	}

	// If we couldn't place enough pieces, fill remaining space with grid pieces
	if (pieces_logical.size() < num_pieces) {
		std::cerr << "Warning: Could only place " << pieces_logical.size() << " non-overlapping pieces out of " << num_pieces << " requested\n";
	}
}

void PuzzleSingleImageState::ClearPicture() {
	if (this->pictureTex) {
		SDL_DestroyTexture(this->pictureTex);
		this->pictureTex = nullptr;
	}
	picture_id = "";
}

void PuzzleSingleImageState::Shuffle() {
	size_t number_of_peices = pieces_logical.size();
	shuffeled_pieces.resize(number_of_peices);
	rotated_pieces.resize(number_of_peices);
	for (size_t i = 0; i< shuffeled_pieces.size(); ++i) {
		shuffeled_pieces.at(i) = i;
	}
	for (size_t i = 0; i< shuffeled_pieces.size(); ++i) {
		if (flipMode) {
			rotated_pieces.at(i) = rand()%4;
		}
		else {
			rotated_pieces.at(i) = 0;
		}
	}
	if (!flipMode) {
		for (int i = 0 ; i < 100; ++i) {
			size_t first = rand()%number_of_peices;
			size_t second = rand()%number_of_peices;
			std::swap(shuffeled_pieces[first], shuffeled_pieces[second]);
		}
	}
	shuffeled = true;
	puzzleSolved = false;  // Reset solved state when shuffling
	confetti.Clear();       // Clear any existing confetti
}


void PuzzleSingleImageState::CheckSolved() {
	bool wasSolved = !shuffeled;
	for (size_t i=0; i < shuffeled_pieces.size(); ++i) {
		if (rotated_pieces[i] != 0) {
			return;
		}
		if (shuffeled_pieces[i] != i) {
			return;
		}
	}
	shuffeled = false;

	// If puzzle just became solved, trigger confetti
	if (!wasSolved && !puzzleSolved) {
		puzzleSolved = true;
		confetti.Burst(globalData.xsize, globalData.ysize);
	}
}

void PuzzleSingleImageState::LaunchPieceEditor() {
	if (!rectangularMode || !pictureTex) {
		return;
	}

	// Create and run the editor state
	PuzzlePieceEditorState editor(imageFilePath, picture_id, pictureTex,
	                              source_image_width, source_image_height,
	                              pieces_logical);
	RunGameState(editor);

	// If the editor saved changes, reload the pieces
	if (editor.WasSaved()) {
		std::vector<SDL_Rect> newPieces = editor.GetEditedPieces();
		if (!newPieces.empty()) {
			pieces_logical = newPieces;
			CreatePhysicalPieces();
			Shuffle();
			std::cout << "Applied edited piece layout\n";
		}
	}
}