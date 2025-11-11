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

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>

/**
 * @brief Loads configuration from file into a map
 * @return Map of configuration key-value pairs
 */
std::map<std::string, std::string> LoadConfigMap();

/**
 * @brief Saves configuration map to file
 * @param config Map of configuration key-value pairs
 */
void SaveConfigMap(const std::map<std::string, std::string>& config);

/**
 * @brief Gets a boolean value from config map
 * @param config The configuration map
 * @param key The configuration key
 * @param defaultValue Default value if key not found
 * @return The boolean value
 */
bool GetConfigBool(const std::map<std::string, std::string>& config, const std::string& key, bool defaultValue);

/**
 * @brief Sets a boolean value in config map
 * @param config The configuration map
 * @param key The configuration key
 * @param value The boolean value
 */
void SetConfigBool(std::map<std::string, std::string>& config, const std::string& key, bool value);

/**
 * @brief Gets an integer value from config map
 * @param config The configuration map
 * @param key The configuration key
 * @param defaultValue Default value if key not found
 * @return The integer value
 */
int GetConfigInt(const std::map<std::string, std::string>& config, const std::string& key, int defaultValue);

/**
 * @brief Sets an integer value in config map
 * @param config The configuration map
 * @param key The configuration key
 * @param value The integer value
 */
void SetConfigInt(std::map<std::string, std::string>& config, const std::string& key, int value);

/**
 * @brief Gets a string value from config map
 * @param config The configuration map
 * @param key The configuration key
 * @param defaultValue Default value if key not found
 * @return The string value
 */
std::string GetConfigString(const std::map<std::string, std::string>& config, const std::string& key, const std::string& defaultValue);

/**
 * @brief Sets a string value in config map
 * @param config The configuration map
 * @param key The configuration key
 * @param value The string value
 */
void SetConfigString(std::map<std::string, std::string>& config, const std::string& key, const std::string& value);

#endif // CONFIG_HPP
