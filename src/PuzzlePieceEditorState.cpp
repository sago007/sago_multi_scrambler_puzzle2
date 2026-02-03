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

#include "PuzzlePieceEditorState.hpp"
#include "globals.hpp"
#include "SagoImGui.hpp"
#include "config.hpp"
#include "os.hpp"
#include "sago/SagoMisc.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include <sstream>

PuzzlePieceEditorState::PuzzlePieceEditorState() {
}

PuzzlePieceEditorState::PuzzlePieceEditorState(const std::string& imageFilePath, const std::string& pictureId, 
                                               SDL_Texture* texture, int sourceWidth, int sourceHeight, 
                                               const std::vector<SDL_Rect>& initialPieces) 
	: imageFilePath(imageFilePath), pictureId(pictureId), pictureTex(texture), 
	  source_image_width(sourceWidth), source_image_height(sourceHeight), 
	  pieces_logical(initialPieces) {
	
	// Calculate logical size
	resized_image_logical_height = resized_image_logical_height_max;
	resized_image_logical_width = double(resized_image_logical_height) * (double(source_image_width) / double(source_image_height));
	if (resized_image_logical_width > resized_image_logical_width_max) {
		resized_image_logical_width = resized_image_logical_width_max;
		resized_image_logical_height = double(resized_image_logical_width) * (double(source_image_height) / double(source_image_width));
	}
	
	ResizeImagePhysical();
}

PuzzlePieceEditorState::~PuzzlePieceEditorState() {
}

bool PuzzlePieceEditorState::IsActive() {
	return isActive;
}

void PuzzlePieceEditorState::ResizeImagePhysical() {
	int resized_image_physical_height_max = globalData.ysize - 150;
	int resized_image_physical_width_max = globalData.xsize - 70;
	resized_image_physical_height = resized_image_physical_height_max;
	resized_image_physical_width = double(resized_image_physical_height) * (double(source_image_width) / double(source_image_height));
	if (resized_image_physical_width > resized_image_physical_width_max) {
		resized_image_physical_width = resized_image_physical_width_max;
		resized_image_physical_height = double(resized_image_physical_width) * (double(source_image_height) / double(source_image_width));
	}
	CreatePhysicalPieces();
}

void PuzzlePieceEditorState::CreatePhysicalPieces() {
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

PuzzlePieceEditorState::DragMode PuzzlePieceEditorState::GetDragModeForPosition(int x, int y, int pieceIndex) {
	if (pieceIndex < 0 || pieceIndex >= pieces_physical.size()) {
		return DragMode::NONE;
	}

	SDL_Rect rect;
	rect.x = globalData.xsize / 2 - resized_image_physical_width / 2;
	rect.y = globalData.ysize / 2 - resized_image_physical_height / 2 + 40;

	const SDL_Rect& piece = pieces_physical[pieceIndex];
	int px = rect.x + piece.x;
	int py = rect.y + piece.y;

	// Check corners first (higher priority)
	if (abs(x - px) <= HANDLE_SIZE && abs(y - py) <= HANDLE_SIZE) {
		return DragMode::RESIZE_NW;
	}
	if (abs(x - (px + piece.w)) <= HANDLE_SIZE && abs(y - py) <= HANDLE_SIZE) {
		return DragMode::RESIZE_NE;
	}
	if (abs(x - px) <= HANDLE_SIZE && abs(y - (py + piece.h)) <= HANDLE_SIZE) {
		return DragMode::RESIZE_SW;
	}
	if (abs(x - (px + piece.w)) <= HANDLE_SIZE && abs(y - (py + piece.h)) <= HANDLE_SIZE) {
		return DragMode::RESIZE_SE;
	}

	// Check edges
	if (abs(y - py) <= HANDLE_SIZE && x >= px && x <= px + piece.w) {
		return DragMode::RESIZE_N;
	}
	if (abs(y - (py + piece.h)) <= HANDLE_SIZE && x >= px && x <= px + piece.w) {
		return DragMode::RESIZE_S;
	}
	if (abs(x - px) <= HANDLE_SIZE && y >= py && y <= py + piece.h) {
		return DragMode::RESIZE_W;
	}
	if (abs(x - (px + piece.w)) <= HANDLE_SIZE && y >= py && y <= py + piece.h) {
		return DragMode::RESIZE_E;
	}

	// Check if inside piece for move
	if (x >= px && x <= px + piece.w && y >= py && y <= py + piece.h) {
		return DragMode::MOVE;
	}

	return DragMode::NONE;
}

void PuzzlePieceEditorState::UpdateDrag() {
	if (dragMode == DragMode::NONE || selected_piece < 0 || selected_piece >= pieces_logical.size()) {
		return;
	}
	int mouseX = globalData.mousex;
	int mouseY = globalData.mousey;
	int deltaX = mouseX - dragStartX;
	int deltaY = mouseY - dragStartY;
	// Convert delta to logical coordinates
	deltaX = deltaX * resized_image_logical_width / resized_image_physical_width;
	deltaY = deltaY * resized_image_logical_height / resized_image_physical_height;
	SDL_Rect& piece = pieces_logical[selected_piece];
	switch (dragMode) {
		case DragMode::MOVE:
			piece.x = dragStartRect.x + deltaX;
			piece.y = dragStartRect.y + deltaY;
			// Clamp to image bounds
			if (piece.x < 0) piece.x = 0;
			if (piece.y < 0) piece.y = 0;
			if (piece.x + piece.w > resized_image_logical_width) piece.x = resized_image_logical_width - piece.w;
			if (piece.y + piece.h > resized_image_logical_height) piece.y = resized_image_logical_height - piece.h;
			break;
		case DragMode::RESIZE_N:
			piece.y = dragStartRect.y + deltaY;
			piece.h = dragStartRect.h - deltaY;
			if (piece.h < MIN_PIECE_SIZE) {
				piece.y = dragStartRect.y + dragStartRect.h - MIN_PIECE_SIZE;
				piece.h = MIN_PIECE_SIZE;
			}
			if (piece.y < 0) {
				piece.h = dragStartRect.y + dragStartRect.h;
				piece.y = 0;
			}
			break;
		case DragMode::RESIZE_S:
			piece.h = dragStartRect.h + deltaY;
			if (piece.h < MIN_PIECE_SIZE) piece.h = MIN_PIECE_SIZE;
			if (piece.y + piece.h > resized_image_logical_height) piece.h = resized_image_logical_height - piece.y;
			break;
		case DragMode::RESIZE_W:
			piece.x = dragStartRect.x + deltaX;
			piece.w = dragStartRect.w - deltaX;
			if (piece.w < MIN_PIECE_SIZE) {
				piece.x = dragStartRect.x + dragStartRect.w - MIN_PIECE_SIZE;
				piece.w = MIN_PIECE_SIZE;
			}
			if (piece.x < 0) {
				piece.w = dragStartRect.x + dragStartRect.w;
				piece.x = 0;
			}
			break;
			
		case DragMode::RESIZE_E:
			piece.w = dragStartRect.w + deltaX;
			if (piece.w < MIN_PIECE_SIZE) piece.w = MIN_PIECE_SIZE;
			if (piece.x + piece.w > resized_image_logical_width) piece.w = resized_image_logical_width - piece.x;
			break;
		case DragMode::RESIZE_NW:
			piece.x = dragStartRect.x + deltaX;
			piece.y = dragStartRect.y + deltaY;
			piece.w = dragStartRect.w - deltaX;
			piece.h = dragStartRect.h - deltaY;
			if (piece.w < MIN_PIECE_SIZE) {
				piece.x = dragStartRect.x + dragStartRect.w - MIN_PIECE_SIZE;
				piece.w = MIN_PIECE_SIZE;
			}
			if (piece.h < MIN_PIECE_SIZE) {
				piece.y = dragStartRect.y + dragStartRect.h - MIN_PIECE_SIZE;
				piece.h = MIN_PIECE_SIZE;
			}
			if (piece.x < 0) {
				piece.w = dragStartRect.x + dragStartRect.w;
				piece.x = 0;
			}
			if (piece.y < 0) {
				piece.h = dragStartRect.y + dragStartRect.h;
				piece.y = 0;
			}
			break;
			
		case DragMode::RESIZE_NE:
			piece.y = dragStartRect.y + deltaY;
			piece.w = dragStartRect.w + deltaX;
			piece.h = dragStartRect.h - deltaY;
			if (piece.w < MIN_PIECE_SIZE) piece.w = MIN_PIECE_SIZE;
			if (piece.h < MIN_PIECE_SIZE) {
				piece.y = dragStartRect.y + dragStartRect.h - MIN_PIECE_SIZE;
				piece.h = MIN_PIECE_SIZE;
			}
			if (piece.x + piece.w > resized_image_logical_width) piece.w = resized_image_logical_width - piece.x;
			if (piece.y < 0) {
				piece.h = dragStartRect.y + dragStartRect.h;
				piece.y = 0;
			}
			break;
		case DragMode::RESIZE_SW:
			piece.x = dragStartRect.x + deltaX;
			piece.w = dragStartRect.w - deltaX;
			piece.h = dragStartRect.h + deltaY;
			if (piece.w < MIN_PIECE_SIZE) {
				piece.x = dragStartRect.x + dragStartRect.w - MIN_PIECE_SIZE;
				piece.w = MIN_PIECE_SIZE;
			}
			if (piece.h < MIN_PIECE_SIZE) piece.h = MIN_PIECE_SIZE;
			if (piece.x < 0) {
				piece.w = dragStartRect.x + dragStartRect.w;
				piece.x = 0;
			}
			if (piece.y + piece.h > resized_image_logical_height) piece.h = resized_image_logical_height - piece.y;
			break;
		case DragMode::RESIZE_SE:
			piece.w = dragStartRect.w + deltaX;
			piece.h = dragStartRect.h + deltaY;
			if (piece.w < MIN_PIECE_SIZE) piece.w = MIN_PIECE_SIZE;
			if (piece.h < MIN_PIECE_SIZE) piece.h = MIN_PIECE_SIZE;
			if (piece.x + piece.w > resized_image_logical_width) piece.w = resized_image_logical_width - piece.x;
			if (piece.y + piece.h > resized_image_logical_height) piece.h = resized_image_logical_height - piece.y;
			break;
		default:
			break;
	}
	CreatePhysicalPieces();
}

void PuzzlePieceEditorState::SavePieces() {
	std::string layoutPath = "piece_layouts/" + pictureId + ".layout";
	std::ostringstream oss;
	oss << pieces_logical.size() << "\n";
	for (const SDL_Rect& piece : pieces_logical) {
		oss << piece.x << " " << piece.y << " " << piece.w << " " << piece.h << "\n";
	}
	sago::WriteFileContent(layoutPath.c_str(), oss.str());
	saved = true;
	std::cout << "Saved piece layout to " << layoutPath << std::endl;
}

void PuzzlePieceEditorState::DeleteSelectedPiece() {
	if (selected_piece >= 0 && selected_piece < pieces_logical.size()) {
		pieces_logical.erase(pieces_logical.begin() + selected_piece);
		selected_piece = -1;
		CreatePhysicalPieces();
	}
}

void PuzzlePieceEditorState::AddNewPiece() {
	// Add a new piece in the center
	SDL_Rect newPiece;
	newPiece.w = 150;
	newPiece.h = 150;
	newPiece.x = resized_image_logical_width / 2 - newPiece.w / 2;
	newPiece.y = resized_image_logical_height / 2 - newPiece.h / 2;
	
	pieces_logical.push_back(newPiece);
	CreatePhysicalPieces();
	selected_piece = pieces_logical.size() - 1;
}

void PuzzlePieceEditorState::ProcessInput(const SDL_Event& event, bool& processed) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			isActive = false;
			processed = true;
		}
		else if (event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_BACKSPACE) {
			DeleteSelectedPiece();
			processed = true;
		}
		else if (event.key.keysym.sym == SDLK_n) {
			AddNewPiece();
			processed = true;
		}
		else if (event.key.keysym.sym == SDLK_s && (SDL_GetModState() & KMOD_CTRL)) {
			SavePieces();
			processed = true;
		}
	}
	if (event.type == SDL_WINDOWEVENT) {
		if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			ResizeImagePhysical();
		}
	}
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		int mouseX = event.button.x;
		int mouseY = event.button.y;
		// Check if clicking on a piece
		bool foundPiece = false;
		for (int i = pieces_physical.size() - 1; i >= 0; --i) {
			DragMode mode = GetDragModeForPosition(mouseX, mouseY, i);
			if (mode != DragMode::NONE) {
				selected_piece = i;
				dragMode = mode;
				dragStartX = mouseX;
				dragStartY = mouseY;
				dragStartRect = pieces_logical[i];
				foundPiece = true;
				processed = true;
				break;
			}
		}
		if (!foundPiece) {
			selected_piece = -1;
		}
	}
	if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
		dragMode = DragMode::NONE;
	}
	ImGui_ImplSDL2_ProcessEvent(&event);
}

void PuzzlePieceEditorState::Draw(SDL_Renderer* target) {
	SDL_SetRenderDrawColor(target, 40, 40, 40, 255);
	SDL_RenderClear(target);
	// Draw title
	SDL_SetRenderDrawColor(target, 60, 60, 60, 255);
	SDL_Rect titleBar = {0, 0, globalData.xsize, 35};
	SDL_RenderFillRect(target, &titleBar);
	// Draw the image
	SDL_Rect imageRect;
	imageRect.x = globalData.xsize / 2 - resized_image_physical_width / 2;
	imageRect.y = globalData.ysize / 2 - resized_image_physical_height / 2 + 40;
	imageRect.h = resized_image_physical_height;
	imageRect.w = resized_image_physical_width;
	if (pictureTex) {
		SDL_RenderCopy(target, pictureTex, NULL, &imageRect);
	}
	// Draw piece outlines and handles
	for (size_t i = 0; i < pieces_physical.size(); ++i) {
		const SDL_Rect& piece = pieces_physical[i];
		SDL_Rect pieceRect;
		pieceRect.x = imageRect.x + piece.x;
		pieceRect.y = imageRect.y + piece.y;
		pieceRect.w = piece.w;
		pieceRect.h = piece.h;
		// Draw piece outline
		if (i == selected_piece) {
			rectangleRGBA(target, pieceRect.x, pieceRect.y, pieceRect.x + pieceRect.w, pieceRect.y + pieceRect.h, 255, 255, 0, 255);
			rectangleRGBA(target, pieceRect.x + 1, pieceRect.y + 1, pieceRect.x + pieceRect.w - 1, pieceRect.y + pieceRect.h - 1, 255, 255, 0, 255);

			// Draw resize handles for selected piece
			SDL_SetRenderDrawColor(target, 255, 255, 0, 255);

			// Corner handles
			SDL_Rect handle;
			handle.w = HANDLE_SIZE * 2;
			handle.h = HANDLE_SIZE * 2;

			// NW
			handle.x = pieceRect.x - HANDLE_SIZE;
			handle.y = pieceRect.y - HANDLE_SIZE;
			SDL_RenderFillRect(target, &handle);

			// NE
			handle.x = pieceRect.x + pieceRect.w - HANDLE_SIZE;
			handle.y = pieceRect.y - HANDLE_SIZE;
			SDL_RenderFillRect(target, &handle);

			// SW
			handle.x = pieceRect.x - HANDLE_SIZE;
			handle.y = pieceRect.y + pieceRect.h - HANDLE_SIZE;
			SDL_RenderFillRect(target, &handle);

			// SE
			handle.x = pieceRect.x + pieceRect.w - HANDLE_SIZE;
			handle.y = pieceRect.y + pieceRect.h - HANDLE_SIZE;
			SDL_RenderFillRect(target, &handle);
		} else {
			rectangleRGBA(target, pieceRect.x, pieceRect.y, pieceRect.x + pieceRect.w, pieceRect.y + pieceRect.h, 100, 200, 255, 200);
		}
	}
	
	// Draw UI with ImGui
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save (Ctrl+S)", nullptr, false, true)) {
				SavePieces();
			}
			if (ImGui::MenuItem("Close", "ESC")) {
				isActive = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Add Piece", "N")) {
				AddNewPiece();
			}
			if (ImGui::MenuItem("Delete Selected", "Del", false, selected_piece >= 0)) {
				DeleteSelectedPiece();
			}
			ImGui::EndMenu();
		}
		ImGui::Text(" | Pieces: %zu", pieces_logical.size());
		if (selected_piece >= 0) {
			ImGui::Text(" | Selected: #%d", selected_piece + 1);
		}
		ImGui::EndMainMenuBar();
	}

	// Instructions window
	ImGui::SetNextWindowPos(ImVec2(10, 45), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Instructions");
	ImGui::Text("Piece Editor");
	ImGui::Separator();
	ImGui::BulletText("Click to select a piece");
	ImGui::BulletText("Drag to move selected piece");
	ImGui::BulletText("Drag corners/edges to resize");
	ImGui::BulletText("Press N to add new piece");
	ImGui::BulletText("Press Delete to remove selected");
	ImGui::BulletText("Press Ctrl+S to save");
	ImGui::BulletText("Press ESC to close");
	ImGui::End();
}

void PuzzlePieceEditorState::Update() {
	if (dragMode != DragMode::NONE && (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(1))) {
		UpdateDrag();
	}
}
