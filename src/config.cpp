/*
Copyright (c) 2025 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "config.hpp"
#include "os.hpp"
#include "sago/SagoMisc.hpp"
#include <vector>
#include <set>
#include <sstream>

std::map<std::string, std::string> LoadConfigMap() {
	std::map<std::string, std::string> config;
	std::string configPath = "config.txt";

	if (sago::FileExists(configPath.c_str())) {
		std::string content = sago::GetFileContent(configPath);
		std::istringstream iss(content);
		std::string line;

		while (std::getline(iss, line)) {
			// Skip empty lines and comments
			if (line.empty() || line[0] == '#') {
				continue;
			}

			// Find the '=' separator
			size_t separatorPos = line.find('=');
			if (separatorPos != std::string::npos) {
				std::string key = line.substr(0, separatorPos);
				std::string value = line.substr(separatorPos + 1);
				config[key] = value;
			}
		}
	}

	return config;
}

void SaveConfigMap(const std::map<std::string, std::string>& config) {
	std::string configPath = "config.txt";
	std::ostringstream oss;

	oss << "# Sago Multi Scrambler Puzzle II Configuration\n";
	for (const auto& pair : config) {
		oss << pair.first << "=" << pair.second << "\n";
	}

	sago::WriteFileContent(configPath.c_str(), oss.str());
}

bool GetConfigBool(const std::map<std::string, std::string>& config, const std::string& key, bool defaultValue) {
	auto it = config.find(key);
	if (it != config.end()) {
		const std::string& value = it->second;
		return (value == "1" || value == "true");
	}
	return defaultValue;
}

void SetConfigBool(std::map<std::string, std::string>& config, const std::string& key, bool value) {
	config[key] = value ? "1" : "0";
}

int GetConfigInt(const std::map<std::string, std::string>& config, const std::string& key, int defaultValue) {
	auto it = config.find(key);
	if (it != config.end()) {
		try {
			return std::stoi(it->second);
		}
		catch (...) {
			return defaultValue;
		}
	}
	return defaultValue;
}

void SetConfigInt(std::map<std::string, std::string>& config, const std::string& key, int value) {
	config[key] = std::to_string(value);
}

std::string GetConfigString(const std::map<std::string, std::string>& config, const std::string& key, const std::string& defaultValue) {
	auto it = config.find(key);
	if (it != config.end()) {
		return it->second;
	}
	return defaultValue;
}

void SetConfigString(std::map<std::string, std::string>& config, const std::string& key, const std::string& value) {
	config[key] = value;
}

std::vector<std::string> LoadFavorites() {
	std::vector<std::string> favorites;
	std::string favoritesPath = "favorites.txt";
	if (sago::FileExists(favoritesPath.c_str())) {
		std::string content = sago::GetFileContent(favoritesPath);
		std::istringstream iss(content);
		std::string line;
		while (std::getline(iss, line)) {
			if (!line.empty() && line[0] != '#') {
				favorites.push_back(line);
			}
		}
	}

	return favorites;
}

void SaveFavorites(const std::vector<std::string>& favorites) {
	std::string favoritesPath = "favorites.txt";
	std::ostringstream oss;

	oss << "# Favorite images - one absolute path per line\n";
	for (const auto& favorite : favorites) {
		oss << favorite << "\n";
	}

	sago::WriteFileContent(favoritesPath.c_str(), oss.str());
}

void AddFavorite(const std::string& imagePath) {
	std::vector<std::string> favorites = LoadFavorites();

	// Check if already in favorites
	for (const auto& fav : favorites) {
		if (fav == imagePath) {
			return;
		}
	}

	favorites.push_back(imagePath);
	SaveFavorites(favorites);
}

void RemoveFavorite(const std::string& imagePath) {
	std::vector<std::string> favorites = LoadFavorites();
	std::vector<std::string> newFavorites;

	for (const auto& fav : favorites) {
		if (fav != imagePath) {
			newFavorites.push_back(fav);
		}
	}

	SaveFavorites(newFavorites);
}

bool IsFavorite(const std::string& imagePath) {
	std::vector<std::string> favorites = LoadFavorites();

	for (const auto& fav : favorites) {
		if (fav == imagePath) {
			return true;
		}
	}

	return false;
}

bool LoadCustomPieceLayout(const std::string& pictureId, std::vector<SDL_Rect>& pieces) {
	std::string layoutPath = "piece_layouts/" + pictureId + ".layout";

	if (!sago::FileExists(layoutPath.c_str())) {
		return false;
	}

	pieces.clear();
	std::string content = sago::GetFileContent(layoutPath);
	std::istringstream iss(content);

	int numPieces;
	iss >> numPieces;
	for (int i = 0; i < numPieces; ++i) {
		SDL_Rect piece;
		iss >> piece.x >> piece.y >> piece.w >> piece.h;
		pieces.push_back(piece);
	}

	return !pieces.empty();
}

bool SaveCustomPieceLayout(const std::string& pictureId, const std::vector<SDL_Rect>& pieces) {
	std::string layoutPath = "piece_layouts/" + pictureId + ".layout";
	std::ostringstream oss;

	oss << pieces.size() << "\n";
	for (const SDL_Rect& piece : pieces) {
		oss << piece.x << " " << piece.y << " " << piece.w << " " << piece.h << "\n";
	}

	sago::WriteFileContent(layoutPath.c_str(), oss.str());
	return true;
}

bool HasCustomPieceLayout(const std::string& pictureId) {
	std::string layoutPath = "piece_layouts/" + pictureId + ".layout";
	return sago::FileExists(layoutPath.c_str());
}

std::set<int> LoadCollectionProgress(const std::string& collectionName) {
	std::set<int> solved;
	std::string progressPath = "collection_progress/" + collectionName + ".txt";
	if (sago::FileExists(progressPath.c_str())) {
		std::string content = sago::GetFileContent(progressPath);
		std::istringstream iss(content);
		int index;
		while (iss >> index) {
			solved.insert(index);
		}
	}
	return solved;
}

void MarkCollectionPuzzleSolved(const std::string& collectionName, int puzzleIndex) {
	std::set<int> solved = LoadCollectionProgress(collectionName);
	solved.insert(puzzleIndex);
	std::string progressPath = "collection_progress/" + collectionName + ".txt";
	std::ostringstream oss;
	for (int idx : solved) {
		oss << idx << "\n";
	}
	sago::WriteFileContent(progressPath.c_str(), oss.str());
}

bool IsCollectionPuzzleSolved(const std::string& collectionName, int puzzleIndex) {
	std::set<int> solved = LoadCollectionProgress(collectionName);
	return solved.count(puzzleIndex) > 0;
}
