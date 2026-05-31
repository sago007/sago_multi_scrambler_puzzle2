/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2024 Poul Sander

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
#include <fstream>
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
#include "CollectionPlayState.hpp"
#include "editor/SagoTextureSelector.hpp"
#include "version.h"
#include "os.hpp"
#include "sago/platform_folders.h"
#include <filesystem>


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
	InitGame();
	CollectionPlayState cps(collection_name);
	RunGameState(cps);
	UninitGame();
}

void runFolder(const std::string& folder_name) {
	InitGame();
	ImageSelectState iss;
	iss.folder = folder_name;
	iss.Init();
	RunGameState(iss);
	UninitGame();
}

void runEditor() {
	InitGame();
	SagoTextureSelector sts;
	sts.Init();
	RunGameState(sts);
	UninitGame();
}

bool installDesktopEntry() {
#if defined(__unix__) && !defined(__APPLE__)
	// Get the executable path
	std::string exe_path;
	try {
		exe_path = std::filesystem::canonical("/proc/self/exe").string();
	} catch (const std::exception& e) {
		std::cerr << "Error: Could not determine executable path: " << e.what() << "\n";
		return false;
	}

	std::string desktop_dir = sago::getDataHome() + "/applications";
	OsCreateFolder(desktop_dir);

	std::string desktop_file_path = desktop_dir + "/sago-multi-scrambler-puzzle2.desktop";

	// Check if file already exists
	if (std::filesystem::exists(desktop_file_path)) {
		std::cout << "Desktop entry already exists at: " << desktop_file_path << "\n";
		std::cout << "Skipping installation.\n";
		return true;
	}

	// Create the desktop entry file
	std::ofstream desktop_file(desktop_file_path);
	if (!desktop_file) {
		std::cerr << "Error: Could not create desktop entry file at: " << desktop_file_path << "\n";
		return false;
	}

	desktop_file << "[Desktop Entry]\n";
	desktop_file << "Version=1.0\n";
	desktop_file << "Type=Application\n";
	desktop_file << "Name=Sago Multi Scrambler Puzzle II\n";
	desktop_file << "Comment=Image scrambling puzzle game\n";
	desktop_file << "Exec=" << exe_path << " %f\n";
	desktop_file << "Icon=sago-multi-scrambler-puzzle2\n";
	desktop_file << "Terminal=false\n";
	desktop_file << "Categories=Game;LogicGame;\n";
	desktop_file << "MimeType=image/jpeg;image/png;image/jpg;\n";

	desktop_file.close();

	// Install SVG icon into the hicolor icon theme
	std::string svg_source;
	try {
		std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
		std::filesystem::path svg_src = exe_dir / "extra" / "sago-multi-scrambler-puzzle2.svg";
		if (std::filesystem::exists(svg_src)) {
			std::string icon_dir = sago::getDataHome() + "/icons/hicolor/scalable/apps";
			OsCreateFolder(icon_dir);
			std::filesystem::path icon_dest = icon_dir + "/sago-multi-scrambler-puzzle2.svg";
			std::filesystem::copy_file(svg_src, icon_dest, std::filesystem::copy_options::overwrite_existing);
			std::cout << "Icon installed at: " << icon_dest.string() << "\n";
			// Update icon cache if possible
			std::string icon_cache_cmd = "gtk-update-icon-cache -f \"" + sago::getDataHome() + "/icons/hicolor\" 2>/dev/null";
			system(icon_cache_cmd.c_str());
		} else {
			std::cout << "Note: SVG icon not found at " << svg_src.string() << ". Desktop entry will use fallback icon.\n";
		}
	} catch (const std::exception& e) {
		std::cout << "Note: Could not install icon: " << e.what() << "\n";
	}

	std::cout << "Desktop entry created successfully at: " << desktop_file_path << "\n";

	// Update the desktop database
	std::string update_cmd = "update-desktop-database \"" + desktop_dir + "\" 2>/dev/null";
	int result = system(update_cmd.c_str());
	if (result == 0) {
		std::cout << "Desktop database updated successfully.\n";
	} else {
		std::cout << "Note: Failed to update desktop database. You may need to run:\n";
		std::cout << "  update-desktop-database \"" << desktop_dir << "\"\n";
	}

	std::cout << "\nYou can now right-click on image files and select\n";
	std::cout << "'Open With → Sago Multi Scrambler Puzzle II' from the context menu.\n";

	return true;
#else
	std::cerr << "Error: Desktop entry installation is only supported on Linux.\n";
	std::cerr << "For Windows, please see the README.md for registry-based installation.\n";
	return false;
#endif
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
	("editor", "Opens a build in editor. Not implemented yet.")
	("install-desktop-entry", "Install desktop entry file for Linux desktop integration (adds 'Open With' menu option)")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << GAMENAME << " " << VERSION_NUMBER << "\n";
		return 0;
	}
	if (vm.count("install-desktop-entry")) {
		return installDesktopEntry() ? 0 : 1;
	}

	InitSagoFS(argc, argv);
	if (vm.count("input-file")) {
		const std::vector<std::string>& input_files = vm["input-file"].as<std::vector<std::string> >();
		std::filesystem::path p = input_files.at(0);
		std::error_code ec;
		std::string filename = std::filesystem::canonical(p, ec).string();
		if (ec) {
			std::cerr << "Failed to open file: " << input_files.at(0) << "\n";
		}
		runSinglePuzzle(filename);
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
	if (vm.count("editor")) {
		runEditor();
		return 0;
	}
	runGame();
	return 0;
}
