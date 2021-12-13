#include <iostream>
#include <boost/program_options.hpp>
#include "sago/SagoDataHolder.hpp"
#include "sago/SagoSpriteHolder.hpp"
#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextField.hpp"
#include "globals.hpp"

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif


static SDL_Window* win = nullptr;
GlobalData globalData;

void InitSagoFS(int argc, const char* argv[]) {
	PHYSFS_init(argv[0]);
	PHYSFS_mount((std::string(PHYSFS_getBaseDir())+"/data").c_str(), nullptr, 0);
}


void runGame() {
	int width = 1280, height = 800;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);

	int rendererFlags = 0;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	win = SDL_CreateWindow("Sago's Multi Scrambler Puzzle II", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	globalData.screen = SDL_CreateRenderer(win, -1, rendererFlags);
	sago::SagoDataHolder holder(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder(holder));
	globalData.dataHolder = &holder;

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		SDL_RenderClear(globalData.screen);
		SDL_RenderPresent(globalData.screen);
	}

	SDL_DestroyRenderer(globalData.screen);
	SDL_DestroyWindow(win);

	SDL_Quit();
}


int main(int argc, const char* argv[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("somestring", boost::program_options::value<std::string>(), "A string to print")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << "sago_multi_scrambler_puzzle2 " << VERSIONNUMBER << "\n";
		return 0;
	}
	InitSagoFS(argc, argv);
	runGame();
	return 0;
}
