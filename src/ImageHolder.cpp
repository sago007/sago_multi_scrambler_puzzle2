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

#include "ImageHolder.hpp"
#include <SDL_image.h>

ImageHolder::ImageHolder() : pictureTex(nullptr), source_image_width(1), source_image_height(1) {}

ImageHolder::ImageHolder(ImageHolder&& other) {
	pictureTex = other.pictureTex;
	source_image_width = other.source_image_width;
	source_image_height = other.source_image_height;
	source_filename = other.source_filename;
	do_lazy_load = other.do_lazy_load;
	other.pictureTex = nullptr;
}

ImageHolder::~ImageHolder() {
	if (pictureTex != nullptr) {
		SDL_DestroyTexture(pictureTex);
		pictureTex = nullptr;
	}
}

void ImageHolder::LoadPictureFromFile(const std::string& filename, SDL_Renderer* renderer) {
	source_filename = filename;
	SDL_Surface* pictureSurface = IMG_Load(filename.c_str());
	if (pictureSurface == nullptr) {
		printf("ImageHolder::LoadPictureFromFile() failed to load image. Filename: %s\n", filename.c_str());
		return;
	}

	if (pictureTex != nullptr) {
		SDL_DestroyTexture(pictureTex);
	}

	pictureTex = SDL_CreateTextureFromSurface(renderer, pictureSurface);
	if (pictureTex == nullptr) {
		printf("ImageHolder::LoadPictureFromFile() failed to create texture. Filename: %s\n", filename.c_str());
	}

	source_image_width = pictureSurface->w;
	source_image_height = pictureSurface->h;

	SDL_FreeSurface(pictureSurface);
}

void ImageHolder::LoadPictureFromFileLazy(const std::string& filename) {
	source_filename = filename;
	do_lazy_load = true;
}


int ImageHolder::GetWidth() const {
	return source_image_width;
}

int ImageHolder::GetHeight() const {
	return source_image_height;
}

SDL_Texture* ImageHolder::GetTexture() const {
	return pictureTex;
}

void ImageHolder::Draw(SDL_Renderer* target, int x, int y, int max_w, int max_h) {
	if (do_lazy_load) {
		printf("Lazy loading image: %s\n", source_filename.c_str());
		std::string filename = source_filename;
		LoadPictureFromFile(filename, target);
		do_lazy_load = false;
	}
	if (pictureTex == nullptr) {
		//printf("ImageHolder::Draw() called with no image loaded. Filename: %s\n", source_filename.c_str());
		return;
	}
	int dest_w = max_w;
	int dest_h = max_h;

	int imgWidth = GetWidth();
	int imgHeight = GetHeight();

	if (max_w == 0) {
		max_w = imgWidth;
	}

	if (max_h == 0) {
		max_h = imgHeight;
	}

	// Calculate the scaled dimensions while maintaining the aspect ratio
	double aspect_ratio = static_cast<double>(imgWidth) / imgHeight;
	if (max_w / aspect_ratio < max_h) {
		max_h = static_cast<int>(max_w / aspect_ratio);
	} else {
		max_w = static_cast<int>(max_h * aspect_ratio);
	}

	x = x + (dest_w - max_w) / 2;
	y = y + (dest_h - max_h) / 2;

	SDL_Rect destRect = { x, y, max_w, max_h };
	SDL_RenderCopy(target, pictureTex, nullptr, &destRect);
}
