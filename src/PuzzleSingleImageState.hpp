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

#include "sago/GameStateInterface.hpp"
#include "SDL.h"
#include <string>
#include <vector>

class PuzzleSingleImageState : public sago::GameStateInterface {
public:
	PuzzleSingleImageState();
	PuzzleSingleImageState(const PuzzleSingleImageState& orig) = delete;
	virtual ~PuzzleSingleImageState();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

	void LoadPictureFromFile(const std::string& filename, SDL_Renderer* renderer);

	void SplitPiece();
	void SplitPiece(size_t piece_number);
	void SplitPieceVertical(size_t piece_number);
	void SplitPieceHorisontal(size_t piece_number);
	void Shuffle();
	void CheckSolved();

	void ResizeImage();
	void ResizeImagePhysical();
	void CreatePhysicalPieces();

	bool flipMode = false;
private:
	void ClearPicture();

	bool isActive = true;
	SDL_Texture* pictureTex = NULL;
	int source_image_height = 1;
	int source_image_width = 1;
	int resized_image_logical_height = 700;
	int resized_image_logical_width = 1;
	const int resized_image_logical_height_max = 700;
	const int resized_image_logical_width_max = 1100;
	int resized_image_physical_height = 700;
	int resized_image_physical_width = 1100;
	std::vector<SDL_Rect> pieces_logical;
	std::vector<SDL_Rect> pieces_physical;
	int min_piece_size = 120;
	bool shuffeled = false;
	std::vector<size_t> shuffeled_pieces;
	std::vector<size_t> rotated_pieces;
	int marked_piece = -1;
};
