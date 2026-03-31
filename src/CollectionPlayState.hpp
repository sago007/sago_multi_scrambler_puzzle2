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

#pragma once

#include "sago/GameStateInterface.hpp"
#include "ImageHolder.hpp"
#include "sago/SagoTextField.hpp"
#include <string>
#include <vector>
#include <set>

struct CollectionPuzzleInfo {
	std::string image;
	std::string title;
	std::string description;
	bool flip_mode = false;
	bool rectangular_mode = false;
};

class CollectionPlayState : public sago::GameStateInterface {
public:
	CollectionPlayState(const std::string& collectionDir);
	CollectionPlayState(const CollectionPlayState& orig) = delete;
	virtual ~CollectionPlayState();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

private:
	void LoadCollection();
	void LoadCurrentImage();
	std::string GetImageFilesystemPath(const std::string& imageFilename) const;
	int FindFirstUnsolved() const;

	bool isActive = true;
	std::string collectionDir;
	std::string collectionName;
	std::string collectionDescription;
	std::vector<CollectionPuzzleInfo> puzzles;
	std::set<int> solvedPuzzles;
	int currentPuzzleIndex = 0;
	bool shouldPlayPuzzle = false;

	ImageHolder currentImageHolder;
	sago::SagoTextField titleField;
	sago::SagoTextField descriptionField;
	sago::SagoTextField progressField;
};
