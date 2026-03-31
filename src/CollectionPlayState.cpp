/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2026 Poul Sander

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

#include "CollectionPlayState.hpp"
#include "PuzzleSingleImageState.hpp"
#include "sago/SagoMisc.hpp"
#include "sago_common.hpp"
#include "globals.hpp"
#include "config.hpp"
#include "SagoImGui.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <physfs.h>

CollectionPlayState::CollectionPlayState(const std::string& collectionDir)
	: collectionDir(collectionDir) {
	LoadCollection();
	solvedPuzzles = LoadCollectionProgress(collectionDir);
	int firstUnsolved = FindFirstUnsolved();
	currentPuzzleIndex = (firstUnsolved >= 0) ? firstUnsolved : 0;
	LoadCurrentImage();
}

CollectionPlayState::~CollectionPlayState() {
}

bool CollectionPlayState::IsActive() {
	return isActive;
}

void CollectionPlayState::ProcessInput(const SDL_Event& event, bool& processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			isActive = false;
			processed = true;
		}
	}
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B || event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) {
			isActive = false;
			processed = true;
		}
	}
}

void CollectionPlayState::Draw(SDL_Renderer* target) {
	if (puzzles.empty()) {
		return;
	}

	const CollectionPuzzleInfo& puzzle = puzzles[currentPuzzleIndex];

	// Draw the image preview centered in the left portion of the screen
	int preview_x = 40;
	int preview_y = 60;
	int preview_max_w = globalData.xsize / 2 - 60;
	int preview_max_h = globalData.ysize - 120;
	currentImageHolder.Draw(target, preview_x, preview_y, preview_max_w, preview_max_h);

	// Draw solved indicator using sprites
	bool isSolved = solvedPuzzles.count(currentPuzzleIndex) > 0;
	int icon_x = preview_x + preview_max_w - 40;
	int icon_y = preview_y + 10;
	const sago::SagoSprite& checkbox = globalData.spriteHolder->GetSprite("i_level_check_box");
	checkbox.Draw(target, SDL_GetTicks(), icon_x, icon_y);
	if (isSolved) {
		const sago::SagoSprite& check = globalData.spriteHolder->GetSprite("i_level_check");
		check.Draw(target, SDL_GetTicks(), icon_x, icon_y);
	}

	// Right panel with info and controls via ImGui
	float panel_x = globalData.xsize / 2.0f + 20.0f;
	float panel_w = globalData.xsize / 2.0f - 40.0f;

	ImGui::SetNextWindowPos(ImVec2(panel_x, 60));
	ImGui::SetNextWindowSize(ImVec2(panel_w, globalData.ysize - 120.0f));
	ImGui::Begin("Puzzle Info", nullptr,
	             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
	             ImGuiWindowFlags_NoCollapse);

	ImGui::TextWrapped("%s", collectionName.c_str());
	ImGui::Separator();

	ImGui::Spacing();
	std::string progress = "Puzzle " + std::to_string(currentPuzzleIndex + 1) + " of " + std::to_string(puzzles.size());
	ImGui::Text("%s", progress.c_str());

	std::string solvedCount = std::to_string(solvedPuzzles.size()) + " / " + std::to_string(puzzles.size()) + " solved";
	ImGui::Text("%s", solvedCount.c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextWrapped("Title: %s", puzzle.title.c_str());
	ImGui::Spacing();
	ImGui::TextWrapped("%s", puzzle.description.c_str());

	if (isSolved) {
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Solved!");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	float button_width = panel_w - 30.0f;

	if (ImGui::Button("Play", ImVec2(button_width, 40))) {
		shouldPlayPuzzle = true;
	}

	ImGui::Spacing();

	bool hasPrev = currentPuzzleIndex > 0;
	bool hasNext = currentPuzzleIndex < static_cast<int>(puzzles.size()) - 1;

	if (!hasPrev) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Previous", ImVec2(button_width, 30))) {
		currentPuzzleIndex--;
		LoadCurrentImage();
	}
	if (!hasPrev) {
		ImGui::EndDisabled();
	}

	if (!hasNext) {
		ImGui::BeginDisabled();
	}
	if (ImGui::Button("Next", ImVec2(button_width, 30))) {
		currentPuzzleIndex++;
		LoadCurrentImage();
	}
	if (!hasNext) {
		ImGui::EndDisabled();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Back to Collections", ImVec2(button_width, 30))) {
		isActive = false;
	}

	ImGui::End();

	// Menu bar
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Close")) {
			isActive = false;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void CollectionPlayState::Update() {
	if (shouldPlayPuzzle) {
		shouldPlayPuzzle = false;

		if (currentPuzzleIndex >= 0 && currentPuzzleIndex < static_cast<int>(puzzles.size())) {
			const CollectionPuzzleInfo& puzzle = puzzles[currentPuzzleIndex];
			std::string imagePath = GetImageFilesystemPath(puzzle.image);

			if (!imagePath.empty()) {
				PuzzleSingleImageState psi;
				psi.flipMode = puzzle.flip_mode;
				psi.rectangularMode = puzzle.rectangular_mode;
				psi.LoadPictureFromFile(imagePath, globalData.screen);
				RunGameState(psi);
				globalData.mouseUp = true;

				if (psi.IsSolved()) {
					MarkCollectionPuzzleSolved(collectionDir, currentPuzzleIndex);
					solvedPuzzles = LoadCollectionProgress(collectionDir);

					// Advance to next unsolved
					int nextUnsolved = -1;
					for (int i = currentPuzzleIndex + 1; i < static_cast<int>(puzzles.size()); ++i) {
						if (solvedPuzzles.count(i) == 0) {
							nextUnsolved = i;
							break;
						}
					}
					if (nextUnsolved >= 0) {
						currentPuzzleIndex = nextUnsolved;
					} else {
						// All remaining are solved; try from the beginning
						int fromStart = FindFirstUnsolved();
						if (fromStart >= 0) {
							currentPuzzleIndex = fromStart;
						}
						// else all solved, stay on current
					}
					LoadCurrentImage();
				}
			} else {
				std::cerr << "Could not resolve path for collection image: " << puzzle.image << std::endl;
			}
		}
	}
}

void CollectionPlayState::LoadCollection() {
	std::string jsonPath = "collections/" + collectionDir + "/collection.json";
	if (!sago::FileExists(jsonPath.c_str())) {
		std::cerr << "Collection file not found: " << jsonPath << std::endl;
		isActive = false;
		return;
	}

	std::string content = sago::GetFileContent(jsonPath);
	try {
		nlohmann::json j = nlohmann::json::parse(content);
		collectionName = j.value("name", collectionDir);
		collectionDescription = j.value("description", "");

		if (j.contains("puzzles") && j["puzzles"].is_array()) {
			for (const auto& p : j["puzzles"]) {
				CollectionPuzzleInfo info;
				info.image = p.value("image", "");
				info.title = p.value("title", "");
				info.description = p.value("description", "");
				info.flip_mode = p.value("flip_mode", false);
				info.rectangular_mode = p.value("rectangular_mode", false);
				if (!info.image.empty()) {
					puzzles.push_back(info);
				}
			}
		}
	} catch (const std::exception& e) {
		std::cerr << "Error parsing collection.json: " << e.what() << std::endl;
		isActive = false;
	}
}

void CollectionPlayState::LoadCurrentImage() {
	if (currentPuzzleIndex >= 0 && currentPuzzleIndex < static_cast<int>(puzzles.size())) {
		std::string imagePath = GetImageFilesystemPath(puzzles[currentPuzzleIndex].image);
		ImageHolder newHolder;
		if (!imagePath.empty()) {
			newHolder.LoadPictureFromFileLazy(imagePath);
		}
		currentImageHolder = std::move(newHolder);
	}
}

std::string CollectionPlayState::GetImageFilesystemPath(const std::string& imageFilename) const {
	std::string physfsPath = "collections/" + collectionDir + "/" + imageFilename;
	const char* realDir = PHYSFS_getRealDir(physfsPath.c_str());
	if (realDir) {
		return std::string(realDir) + "/" + physfsPath;
	}
	return "";
}

int CollectionPlayState::FindFirstUnsolved() const {
	for (int i = 0; i < static_cast<int>(puzzles.size()); ++i) {
		if (solvedPuzzles.count(i) == 0) {
			return i;
		}
	}
	return -1;
}
