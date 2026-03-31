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

#include "CollectionListState.hpp"
#include "CollectionPlayState.hpp"
#include "sago/SagoMisc.hpp"
#include "sago_common.hpp"
#include "globals.hpp"
#include "config.hpp"
#include "SagoImGui.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <algorithm>

CollectionListState::CollectionListState() {
	DiscoverCollections();
}

CollectionListState::~CollectionListState() {
}

bool CollectionListState::IsActive() {
	return isActive;
}

void CollectionListState::ProcessInput(const SDL_Event& event, bool& processed) {
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

void CollectionListState::Draw(SDL_Renderer* target) {
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Close")) {
			isActive = false;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	float window_w = 500.0f;
	float window_h = globalData.ysize - 80.0f;
	ImGui::SetNextWindowPos(ImVec2(globalData.xsize / 2.0f, globalData.ysize / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(window_w, window_h));
	ImGui::Begin("Collections", nullptr,
	             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
	             ImGuiWindowFlags_NoCollapse);

	if (collections.empty()) {
		ImGui::TextWrapped("No collections found. Place collection folders with a collection.json file in the data/collections/ directory.");
	} else {
		for (size_t i = 0; i < collections.size(); ++i) {
			const CollectionInfo& col = collections[i];
			ImGui::PushID(static_cast<int>(i));

			std::set<int> progress = LoadCollectionProgress(col.dirName);
			std::string solvedText = std::to_string(progress.size()) + " / " + std::to_string(col.puzzleCount) + " solved";

			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button(col.displayName.c_str(), ImVec2(window_w - 30.0f, 0))) {
				selectedCollection = static_cast<int>(i);
			}

			ImGui::TextWrapped("%s", col.description.c_str());
			ImGui::Text("%s", solvedText.c_str());
			ImGui::Spacing();

			ImGui::PopID();
		}
		ImGui::Separator();
	}

	ImGui::Spacing();
	if (ImGui::Button("Back", ImVec2(window_w - 30.0f, 30))) {
		isActive = false;
	}

	ImGui::End();
}

void CollectionListState::Update() {
	if (selectedCollection >= 0 && selectedCollection < static_cast<int>(collections.size())) {
		std::string dirName = collections[selectedCollection].dirName;
		selectedCollection = -1;

		CollectionPlayState cps(dirName);
		RunGameState(cps);
		globalData.mouseUp = true;

		// Re-discover in case progress changed
		DiscoverCollections();
	}
}

void CollectionListState::DiscoverCollections() {
	collections.clear();
	std::vector<std::string> dirs = sago::GetFileList("collections");

	for (const std::string& dir : dirs) {
		std::string jsonPath = "collections/" + dir + "/collection.json";
		if (!sago::FileExists(jsonPath.c_str())) {
			continue;
		}

		std::string content = sago::GetFileContent(jsonPath);
		try {
			nlohmann::json j = nlohmann::json::parse(content);
			CollectionInfo info;
			info.dirName = dir;
			info.displayName = j.value("name", dir);
			info.description = j.value("description", "");
			if (j.contains("puzzles") && j["puzzles"].is_array()) {
				info.puzzleCount = static_cast<int>(j["puzzles"].size());
			}
			collections.push_back(info);
		} catch (const std::exception& e) {
			std::cerr << "Error parsing " << jsonPath << ": " << e.what() << std::endl;
		}
	}

	std::sort(collections.begin(), collections.end(), [](const CollectionInfo& a, const CollectionInfo& b) {
		return a.displayName < b.displayName;
	});
}
