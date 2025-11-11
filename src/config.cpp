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
#include <fstream>

std::map<std::string, std::string> LoadConfigMap() {
	std::map<std::string, std::string> config;
	std::string configPath = getPathToSaveFiles() + "/config.txt";
	std::ifstream configFile(configPath);

	if (configFile.is_open()) {
		std::string line;
		while (std::getline(configFile, line)) {
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
		configFile.close();
	}

	return config;
}

void SaveConfigMap(const std::map<std::string, std::string>& config) {
	std::string configPath = getPathToSaveFiles() + "/config.txt";
	std::ofstream configFile(configPath);

	if (configFile.is_open()) {
		configFile << "# Sago Multi Scrambler Puzzle II Configuration" << std::endl;
		for (const auto& pair : config) {
			configFile << pair.first << "=" << pair.second << std::endl;
		}
		configFile.close();
	}
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
