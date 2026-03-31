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
#include "globals.hpp"
#include "SagoImGui.hpp"
#include "config.hpp"
#include "PuzzleSingleImageState.hpp"
#include "CollectionListState.hpp"
#include "sago_common.hpp"

MainGameState::MainGameState() {
}


MainGameState::~MainGameState() {
}

bool MainGameState::IsActive() {
	return isActive;
}

void MainGameState::ProcessInput(const SDL_Event& event, bool& processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void MainGameState::Draw(SDL_Renderer* target) {
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