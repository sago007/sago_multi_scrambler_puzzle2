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

#include "ImageSelectState.hpp"
#include <filesystem>
#include <iostream>
#include "globals.hpp"
#include "PuzzleSingleImageState.hpp"
#include "sago_common.hpp"

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

static void DrawRectWhite(SDL_Renderer* target, int topx, int topy, int height, int width) {
	std::string name = "ui_rect_white_";
	DrawRect(target, topx, topy, height, width, name);
}

static void DrawRectYellow(SDL_Renderer* target, int topx, int topy, int height, int width) {
	std::string name = "ui_rect_yellow_";
	DrawRect(target, topx, topy, height, width, name);
}

ImageSelectState::ImageSelectState() {

}


ImageSelectState::~ImageSelectState() {
}

bool ImageSelectState::IsActive() {
	return true;
}

void ImageSelectState::ProcessInput(const SDL_Event& event, bool &processed) {

}

void ImageSelectState::Draw(SDL_Renderer* target) {
	const int number_of_images_per_page = 6;
	const int frame_size = 300;
	const int frame_spacing = 20;
	const int frame_border = 10;
	const int number_of_columns = 3;
	const int top_x = (globalData.xsize-(3*frame_size+2*frame_spacing))/2;
	const int top_y = (globalData.ysize-(2*frame_size+frame_spacing))/2;
	for (int i = 0; i < number_of_images_per_page; ++i) {
		int x = i % number_of_columns;
		int y = i / number_of_columns;
		DrawRectYellow(target, top_x+x*(frame_size+frame_spacing), top_y+y*(frame_size+frame_spacing), frame_size, frame_size);
		if (i < imageHolders.size()) {
			imageHolders[i].Draw(target, top_x+x*(frame_size+frame_spacing)+frame_border, top_y+y*(frame_size+frame_spacing)+frame_border, frame_size-2*frame_border, frame_size-2*frame_border);
		}
		if (i < imageNameFields.size()) {
			imageNameFields[i].Draw(target, top_x+x*(frame_size+frame_spacing)+frame_size/2, top_y+y*(frame_size+frame_spacing)+frame_size-5, sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::bottom);
		}
	}
}


void ImageSelectState::Update() {
	// If the mouse button is released, make bMouseUp equal true
	if ( !(SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) ) {
		globalData.mouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && globalData.mouseUp) {
		globalData.mouseUp = false;

		if (globalData.mousex > 0 && globalData.mousex < globalData.xsize && globalData.mousey > 0 && globalData.mousey < globalData.ysize) {
			const int number_of_images_per_page = 6;
			const int frame_size = 300;
			const int frame_spacing = 20;
			const int frame_border = 10;
			const int number_of_columns = 3;
			const int top_x = (globalData.xsize-(3*frame_size+2*frame_spacing))/2;
			const int top_y = (globalData.ysize-(2*frame_size+frame_spacing))/2;
			for (int i = 0; i < number_of_images_per_page; ++i) {
				int x = i % number_of_columns;
				int y = i / number_of_columns;
				if (globalData.mousex > top_x+x*(frame_size+frame_spacing) && globalData.mousex < top_x+x*(frame_size+frame_spacing)+frame_size &&
						globalData.mousey > top_y+y*(frame_size+frame_spacing) && globalData.mousey < top_y+y*(frame_size+frame_spacing)+frame_size) {
					if (i < imageList.size()) {
						printf("Clicked on image %s\n", imageList[i].c_str());
						PuzzleSingleImageState psi;
						psi.LoadPictureFromFile(imageList[i], globalData.screen);
						RunGameState(psi);
					}
				}
			}
		}
	}
}

/**
 * @brief Compares two characters ignoring case
 * 
 * @param a 
 * @param b 
 * @return true if the characters are equal ignoring case
 * @return false if the characters are not equal ignoring case
 */
static bool ichar_equals(char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

/**
 * @brief Checks if the filename has the given extension. The comparison is case insensitive.
 * 
 * @param filename The filename to check
 * @param extension The extension to check for (example ".jpg")
 * @return true if the filename has the given extension
 * @return false if the filename does not have the given extension
 */
static bool HasExtension(const std::string& filename, const std::string& extension) {
	if (filename.size() < extension.size()) {
		return false;
	}
	return std::equal(extension.rbegin(), extension.rend(), filename.rbegin(), ichar_equals);
}

static void setFontText(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
        field.SetHolder(holder);
        field.SetFont("freeserif");
        field.SetColor({255,255,255,255});
        field.SetFontSize(24);
        field.SetOutline(1, {0,0,0,255});
        field.SetText(text);
}


void ImageSelectState::Init() {
	if (!folder.empty()) {
		for (const auto& entry : std::filesystem::directory_iterator(folder)) {
			if (entry.is_regular_file() && (HasExtension(entry.path().string(), ".jpg") || HasExtension(entry.path().string(), ".jpeg") || HasExtension(entry.path().string(), ".png") ) ) {
				std::cout << entry.path() << std::endl;
				imageList.push_back(entry.path().string());
				sago::SagoTextField field;
				setFontText(globalData.dataHolder, field, entry.path().filename().string().c_str());
				imageNameFields.push_back(std::move(field));
				imageHolders.emplace_back();
				imageHolders.back().LoadPictureFromFile(entry.path().string(), globalData.screen);
			}
		}
	}
}