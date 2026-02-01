/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2026 Poul Sander

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
https://github.com/sago007/sago_multi_scrambler_puzzle2
===========================================================================
*/

#ifndef PUZZLEPIECEEDITORSTATE_HPP
#define PUZZLEPIECEEDITORSTATE_HPP

#include "sago/GameStateInterface.hpp"
#include "SDL.h"
#include <string>
#include <vector>

class PuzzlePieceEditorState : public sago::GameStateInterface {
public:
	PuzzlePieceEditorState();
	PuzzlePieceEditorState(const std::string& imageFilePath, const std::string& pictureId, SDL_Texture* texture, int sourceWidth, int sourceHeight, const std::vector<SDL_Rect>& initialPieces);
	PuzzlePieceEditorState(const PuzzlePieceEditorState& orig) = delete;
	virtual ~PuzzlePieceEditorState();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

	const std::vector<SDL_Rect>& GetEditedPieces() const { return pieces_logical; }
	bool WasSaved() const { return saved; }

private:
	enum class DragMode {
		NONE,
		MOVE,
		RESIZE_N,
		RESIZE_S,
		RESIZE_E,
		RESIZE_W,
		RESIZE_NE,
		RESIZE_NW,
		RESIZE_SE,
		RESIZE_SW
	};

	void ResizeImagePhysical();
	void CreatePhysicalPieces();
	void SavePieces();
	void DeleteSelectedPiece();
	void AddNewPiece();
	DragMode GetDragModeForPosition(int x, int y, int pieceIndex);
	void UpdateDrag();

	bool isActive = true;
	bool saved = false;
	SDL_Texture* pictureTex = nullptr;
	std::string imageFilePath;
	std::string pictureId;

	int source_image_width = 1;
	int source_image_height = 1;
	
	int resized_image_logical_width = 1100;
	int resized_image_logical_height = 700;
	const int resized_image_logical_width_max = 1100;
	const int resized_image_logical_height_max = 700;

	int resized_image_physical_width = 1100;
	int resized_image_physical_height = 700;

	std::vector<SDL_Rect> pieces_logical;
	std::vector<SDL_Rect> pieces_physical;

	int selected_piece = -1;
	DragMode dragMode = DragMode::NONE;
	int dragStartX = 0;
	int dragStartY = 0;
	SDL_Rect dragStartRect;

	const int HANDLE_SIZE = 8;
	const int MIN_PIECE_SIZE = 50;
};

#endif // PUZZLEPIECEEDITORSTATE_HPP
