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

#include <iostream>
#include <boost/program_options.hpp>
#include "sago/SagoDataHolder.hpp"
#include "sago/SagoSpriteHolder.hpp"
#include "sago/GameStateInterface.hpp"
#include "sago/SagoTextField.hpp"
#include "globals.hpp"
#include "PuzzleSingleImageState.hpp"
#include "ImageSelectState.hpp"
#include "sago_common.hpp"
#include "MainGameState.hpp"

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif


GlobalData globalData;

void runSinglePuzzle(const std::string& filename) {
	PuzzleSingleImageState psi;
	InitGame();
	psi.LoadPictureFromFile(filename, globalData.screen);
	RunGameState(psi);
	UninitGame();
}

void runGame() {
	MainGameState mgs;
	InitGame();

	RunGameState(mgs);
	UninitGame();
}

void runCollection(const std::string& collection_name) {
	ImageSelectState iss;
	iss.collection = collection_name;
	InitGame();
	RunGameState(iss);
	UninitGame();
}

void runFolder(const std::string& folder_name) {
	ImageSelectState iss;
	iss.folder = folder_name;
	iss.Init();
	InitGame();
	RunGameState(iss);
	UninitGame();
}


int main(int argc, const char* argv[]) {
	boost::program_options::options_description desc("Options");
	boost::program_options::positional_options_description p;
	p.add("input-file", -1);
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("input-file", boost::program_options::value< std::vector<std::string> >(), "Image to open directly")
	("collection", boost::program_options::value< std::string >(), "Jump straigt to a named collection. Like \"fairy_tales\"")
	("folder", boost::program_options::value< std::string >(), "Open a specific folder.")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
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
	if (vm.count("input-file")) {
		const std::vector<std::string>& input_files = vm["input-file"].as<std::vector<std::string> >();
		runSinglePuzzle(input_files.at(0));
		return 0;
	}
	if (vm.count("collection")) {
		const std::string& collection_name = vm["collection"].as<std::string>();
		runCollection(collection_name);
		return 0;
	}
	if (vm.count("folder")) {
		const std::string& folder = vm["folder"].as<std::string>();
		runFolder(folder);
		return 0;
	}
	runGame();
	return 0;
}
