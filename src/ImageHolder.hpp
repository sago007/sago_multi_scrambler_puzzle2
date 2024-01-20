/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2024 Poul Sander

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

#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include <string>
#include <SDL.h>

class ImageHolder {
public:
	// Constructor
	ImageHolder();

	// Copy constructor
	ImageHolder(const ImageHolder& other) = delete;

	// Assignment operator
	ImageHolder& operator=(const ImageHolder& other) = delete;

	// Move constructor
	ImageHolder(ImageHolder&& other);

	// Destructor
	~ImageHolder();

	// Other member functions and variables
	void LoadPictureFromFile(const std::string& filename, SDL_Renderer* renderer);
	int GetWidth() const;
	int GetHeight() const;
	SDL_Texture* GetTexture() const;

	/**
	 * Draw the image to the target.
	 * Image will be scaled to fit within the max_w and max_h.
	 * Ratio is kept.
	 * @param target The target to draw to.
	 * @param x The x coordinate to draw to.
	 * @param y The y coordinate to draw to.
	 * @param max_w The maximum width to draw. If 0 then the image width is used.
	 * @param max_h The maximum height to draw. If 0 then the image height is used.
	*/
	void Draw(SDL_Renderer* target, int x, int y, int max_w = 0, int max_h = 0) const;

private:
	// Private member variables
	SDL_Texture* pictureTex = NULL;
	int source_image_height = 1;
	int source_image_width = 1;
	std::string source_filename;
};

#endif // IMAGEHOLDER_HPP
