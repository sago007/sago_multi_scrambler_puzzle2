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

#include "sago/SagoDataHolder.hpp"
#include "sago/SagoSpriteHolder.hpp"
#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextField.hpp"
#include "globals.hpp"
#include "sago_common.hpp"
#include "SagoImGui.hpp"

void InitSagoFS(int argc, const char* argv[]) {
	PHYSFS_init(argv[0]);
	PHYSFS_mount((std::string(PHYSFS_getBaseDir())+"/data").c_str(), nullptr, 0);
}

void RunGameState(sago::GameStateInterface& state ) {
	bool done = false;  //We are done!
	while (!done && !globalData.isShuttingDown) {
		SDL_RenderClear(globalData.screen);
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		state.Draw(globalData.screen);

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		float x, y;
		SDL_RenderGetScale(globalData.screen, &x, &y);
		SDL_RenderSetScale(globalData.screen, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		ImGui_ImplSDLRenderer2_RenderDrawData( ImGui::GetDrawData(), globalData.screen );
		SDL_RenderSetScale(globalData.screen, x, y);

		//While using Dear ImGui we do not draw the mouse ourself. This is gone: globalData.mouse.Draw(globalData.screen, SDL_GetTicks(), globalData.mousex, globalData.mousey);
		SDL_RenderPresent(globalData.screen);


		SDL_Delay(5);
		SDL_Event event;

		bool mustWriteScreenshot = false;

		while ( SDL_PollEvent(&event) ) {
				UpdateMouseCoordinates(event, globalData.mousex, globalData.mousey);
				if ( event.type == SDL_QUIT ) {
						globalData.isShuttingDown = true;
						done = true;
				}

				if ( event.key.keysym.sym == SDLK_F9 ) {
						mustWriteScreenshot = true;
				}

				bool processed = false;
				state.ProcessInput(event, processed);

		}

		state.Update();



		if (!state.IsActive()) {
			done = true;
		}
	}
}

/**
 * This function reads the mouse coordinates from a relevant event.
 * Unlike SDL_GetMouseState this works even if SDL_RenderSetLogicalSize is used
 * @param event
 * @param mousex
 * @param mousey
 */
void UpdateMouseCoordinates(const SDL_Event& event, int& mousex, int& mousey) {
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		mousex = event.button.x;
		mousey = event.button.y;
		break;
	case SDL_MOUSEMOTION:
		mousex = event.motion.x;
		mousey = event.motion.y;
		break;
	default:
		break;
	}
}

sago::SagoDataHolder dataHolder;


static SDL_Window* win = nullptr;

void InitGame() {
	int width = globalData.xsize, height = globalData.ysize;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);

	int rendererFlags = 0;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SCALING, "1");
	win = SDL_CreateWindow(GAMENAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	globalData.screen = SDL_CreateRenderer(win, -1, rendererFlags);
	SDL_RenderSetLogicalSize(globalData.screen, width, height);
	InitImGui(win, globalData.screen, width, height);
	
	dataHolder.invalidateAll(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder(dataHolder));
	globalData.dataHolder = &dataHolder;
	globalData.mouse = globalData.spriteHolder->GetSprite("mouse");
	SDL_ShowCursor(SDL_DISABLE);
}

void UninitGame() {
	SDL_DestroyRenderer(globalData.screen);
	SDL_DestroyWindow(win);

	SDL_Quit();
}