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

#include "MainGameState.hpp"
#include <string>
#include <random>
#include <iostream>
#include <cmath>
#include "globals.hpp"
#include "SagoImGui.hpp"
#include "config.hpp"
#include "PuzzleSingleImageState.hpp"
#include "CollectionListState.hpp"
#include "sago_common.hpp"
#include "sago/SagoMisc.hpp"
#include "SDL_image.h"

MainGameState::MainGameState() {
}


MainGameState::~MainGameState() {
	if (backgroundTex) {
		SDL_DestroyTexture(backgroundTex);
		backgroundTex = nullptr;
	}
}

static void SplitPieceVertical(std::vector<SDL_Rect>& pieces, size_t piece_number, int min_piece_size) {
	SDL_Rect piece1 = pieces.at(piece_number);
	if (piece1.w > 2 * min_piece_size) {
		int new_piece_width = min_piece_size + (rand() % (piece1.w - 2 * min_piece_size));
		SDL_Rect piece2 = piece1;
		piece2.w = new_piece_width;
		piece1.w -= new_piece_width;
		piece2.x = piece2.x + piece1.w;
		pieces.at(piece_number) = piece1;
		pieces.push_back(piece2);
	}
}

static void SplitPieceHorisontal(std::vector<SDL_Rect>& pieces, size_t piece_number, int min_piece_size) {
	SDL_Rect piece1 = pieces.at(piece_number);
	if (piece1.h > 2 * min_piece_size) {
		int new_piece_height = min_piece_size + (rand() % (piece1.h - 2 * min_piece_size));
		SDL_Rect piece2 = piece1;
		piece2.h = new_piece_height;
		piece1.h -= new_piece_height;
		piece2.y = piece2.y + piece1.h;
		pieces.at(piece_number) = piece1;
		pieces.push_back(piece2);
	}
}

void MainGameState::InitBackgroundPieces() {
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	std::unique_ptr<char[]> data;
	unsigned int bytes = 0;
	sago::ReadBytesFromFile("collections/fairy_tales/Little Red Riding Hood (1919).jpg", data, bytes);
	if (!data || bytes == 0) {
		std::cerr << "Failed to load background image from PhysFS" << std::endl;
		return;
	}
	SDL_RWops* rw = SDL_RWFromMem(data.get(), bytes);
	SDL_Surface* surface = IMG_Load_RW(rw, 0);
	SDL_RWclose(rw);
	if (!surface) {
		std::cerr << "Failed to decode background image" << std::endl;
		return;
	}
	bg_source_width = surface->w;
	bg_source_height = surface->h;
	backgroundTex = SDL_CreateTextureFromSurface(globalData.screen, surface);
	SDL_FreeSurface(surface);
	SDL_SetTextureBlendMode(backgroundTex, SDL_BLENDMODE_BLEND);

	// Compute logical dimensions (max 700h x 1100w, maintaining aspect ratio)
	const int max_logical_height = 700;
	const int max_logical_width = 1100;
	bg_logical_height = max_logical_height;
	bg_logical_width = static_cast<int>(double(bg_logical_height) * (double(bg_source_width) / double(bg_source_height)));
	if (bg_logical_width > max_logical_width) {
		bg_logical_width = max_logical_width;
		bg_logical_height = static_cast<int>(double(bg_logical_width) * (double(bg_source_height) / double(bg_source_width)));
	}

	// Start with single piece covering the whole image
	SDL_Rect piece;
	piece.x = 0;
	piece.y = 0;
	piece.w = bg_logical_width;
	piece.h = bg_logical_height;
	bg_pieces_logical.push_back(piece);

	// Split into ~8-12 pieces
	srand(static_cast<unsigned>(time(nullptr)));
	const int min_piece_size = 120;
	for (int iter = 0; iter < 6; ++iter) {
		size_t count = bg_pieces_logical.size();
		for (size_t i = 0; i < count; ++i) {
			const SDL_Rect& p = bg_pieces_logical[i];
			if (p.w > 2 * min_piece_size || p.h > 2 * min_piece_size) {
				if (rand() % 2 == 1) {
					SplitPieceVertical(bg_pieces_logical, i, min_piece_size);
					SplitPieceHorisontal(bg_pieces_logical, i, min_piece_size);
				} else {
					SplitPieceHorisontal(bg_pieces_logical, i, min_piece_size);
					SplitPieceVertical(bg_pieces_logical, i, min_piece_size);
				}
			}
		}
	}
}

bool MainGameState::IsActive() {
	return isActive;
}

void MainGameState::ProcessInput(const SDL_Event& event, bool& processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void MainGameState::Draw(SDL_Renderer* target) {
	if (!bg_initialized) {
		bg_initialized = true;
		InitBackgroundPieces();
	}

	// Draw rotating image pieces as background decoration
	if (backgroundTex && !bg_pieces_logical.empty()) {
		size_t N = bg_pieces_logical.size();
		double base_angle = SDL_GetTicks() * 0.0005;
		int centerX = globalData.xsize / 2;
		int centerY = globalData.ysize / 2;
		int radius = std::min(globalData.xsize, globalData.ysize) / 4;

		// Compute a display scale so pieces fit nicely on screen
		double display_scale = double(std::min(globalData.xsize, globalData.ysize)) / double(bg_logical_height) * 0.25;
		double source_scale = double(bg_source_height) / double(bg_logical_height);

		SDL_SetTextureAlphaMod(backgroundTex, 180);
		for (size_t i = 0; i < N; ++i) {
			double angle = base_angle + (2.0 * M_PI * i / N);
			int px = centerX + static_cast<int>(radius * cos(angle));
			int py = centerY + static_cast<int>(radius * sin(angle));

			const SDL_Rect& lp = bg_pieces_logical[i];

			// Source rect in original image coordinates
			SDL_Rect src;
			src.x = static_cast<int>(lp.x * source_scale);
			src.y = static_cast<int>(lp.y * source_scale);
			src.w = static_cast<int>(lp.w * source_scale);
			src.h = static_cast<int>(lp.h * source_scale);

			// Destination rect centered at orbit position
			SDL_Rect dst;
			dst.w = static_cast<int>(lp.w * display_scale);
			dst.h = static_cast<int>(lp.h * display_scale);
			dst.x = px - dst.w / 2;
			dst.y = py - dst.h / 2;

			SDL_RenderCopy(target, backgroundTex, &src, &dst);
		}
		SDL_SetTextureAlphaMod(backgroundTex, 255);
	}

	DrawRectYellow(target, 5, 5, 200, 200);

	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Random image from favorites")) {
			shouldLoadRandomFavorite = true;
		}
		if (ImGui::MenuItem("Quit")) {
			isActive = false;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	// Center a window with the button
	ImGui::SetNextWindowPos(ImVec2(globalData.xsize / 2.0f, globalData.ysize / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

	if (ImGui::Button("Collections", ImVec2(300, 50))) {
		shouldOpenCollections = true;
	}

	if (ImGui::Button("Random image from favorites", ImVec2(300, 50))) {
		shouldLoadRandomFavorite = true;
	}

	if (ImGui::Button("Quit", ImVec2(300, 50))) {
		isActive = false;
	}

	ImGui::End();
}


void MainGameState::Update() {
	if (shouldLoadRandomFavorite) {
		shouldLoadRandomFavorite = false;
		LoadRandomFavorite();
	}
	if (shouldOpenCollections) {
		shouldOpenCollections = false;
		CollectionListState cls;
		RunGameState(cls);
		globalData.mouseUp = true;
	}
}

void MainGameState::LoadRandomFavorite() {
	std::vector<std::string> favorites = LoadFavorites();

	if (favorites.empty()) {
		std::cerr << "No favorites found!" << std::endl;
		return;
	}

	// Pick a random favorite
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, favorites.size() - 1);
	int randomIndex = dis(gen);

	std::string randomFavorite = favorites[randomIndex];
	std::cout << "Loading random favorite: " << randomFavorite << std::endl;

	// Load the puzzle
	PuzzleSingleImageState psi;
	psi.LoadPictureFromFile(randomFavorite, globalData.screen);
	RunGameState(psi);
}


static void DrawRect(SDL_Renderer* target, int topx, int topy, int height, int width, const std::string& name) {
	const int size = 32;
	SDL_Rect bounds_ns = {topx, topy+size, width, height-2*size};  //bounds for south
	SDL_Rect bounds_e = {topx, topy, width-size, height};
	const sago::SagoSprite& n = globalData.spriteHolder->GetSprite(name+"n");
	const sago::SagoSprite& s = globalData.spriteHolder->GetSprite(name+"s");
	const sago::SagoSprite& e = globalData.spriteHolder->GetSprite(name+"e");
	const sago::SagoSprite& w = globalData.spriteHolder->GetSprite(name+"w");
	const sago::SagoSprite& fill = globalData.spriteHolder->GetSprite(name+"fill");
	for (int i = 1; i < width/size; ++i) {
		n.DrawBounded(target, SDL_GetTicks(), topx+i*size, topy, bounds_e);
		for (int j = 1; j < height/size; ++j) {
			w.DrawBounded(target, SDL_GetTicks(), topx, topy+j*size, bounds_ns);
			fill.Draw(target, SDL_GetTicks(),topx+i*size, topy+j*size);
			e.DrawBounded(target, SDL_GetTicks(), topx+width-size, topy+j*size, bounds_ns);
		}
		s.DrawBounded(target, SDL_GetTicks(), topx+i*size, topy+height-size, bounds_e);
	}
	//Corners
	const sago::SagoSprite& nw = globalData.spriteHolder->GetSprite(name+"nw");
	const sago::SagoSprite& ne = globalData.spriteHolder->GetSprite(name+"ne");
	const sago::SagoSprite& se = globalData.spriteHolder->GetSprite(name+"se");
	const sago::SagoSprite& sw = globalData.spriteHolder->GetSprite(name+"sw");
	nw.Draw(target, SDL_GetTicks(), topx, topy);
	ne.Draw(target, SDL_GetTicks(), topx+width-size, topy);
	se.Draw(target, SDL_GetTicks(), topx+width-size, topy+height-size);
	sw.Draw(target, SDL_GetTicks(), topx, topy+height-size);
}

void DrawRectWhite(SDL_Renderer* target, int topx, int topy, int height, int width) {
	std::string name = "ui_rect_white_";
	DrawRect(target, topx, topy, height, width, name);
}

void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width) {
	std::string name = "ui_rect_yellow_";
	DrawRect(target, topx, topy, height, width, name);
}