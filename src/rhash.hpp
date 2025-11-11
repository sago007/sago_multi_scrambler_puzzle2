/*
===========================================================================
 * RHash - wrapper for C++
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

#pragma once

#include <rhash.h>
#include <string>
#include <vector>

class RHash {
public:
	RHash(unsigned hash_mask) {
		hash_ids.push_back(hash_mask);
	}

	RHash(std::initializer_list<unsigned> hash_list) : RHash(0) {
		for (unsigned hash_id : hash_list) {
			add_hash(hash_id);
		}
	}

	~RHash() {
		if (ctx_) {
			rhash_free(ctx_);
			ctx_ = nullptr;
		}
	}

	void reinit() {
		if (ctx_) {
			rhash_free(ctx_);
			ctx_ = nullptr;
		}
		unsigned hash_mask = 0;
		for (unsigned hash_id : hash_ids) {
			hash_mask |= hash_id;
		}
		ctx_ = rhash_init(hash_mask);
		if (ctx_ == nullptr) {
			throw std::runtime_error("Failed to initialize RHash context");
		}
		done = false;
	}

	void add_hash(unsigned hash_id) {
		hash_ids.push_back(hash_id);
	}

	void update(const std::string& data) {
		if (!ctx_) {
			reinit();
		}
		if (done) {
			throw std::runtime_error("\"update\" called on a finalized rhash context");
		}
		rhash_update(ctx_, data.c_str(), data.length());
		length += data.length();
	}

	void finish() {
		rhash_final(ctx_, nullptr);
		done = true;
	}

	std::string hash_as_string(unsigned hash_id, int flags) {
		if (!done) {
			finish();
		}
		size_t length = rhash_get_hash_length(hash_id);
		std::vector<char> buffer(length);
		rhash_print(buffer.data(), ctx_, hash_id, flags);
		return std::string(buffer.data(), buffer.size());
	}

	std::string hex(unsigned hash_id) {
		return hash_as_string(hash_id, RHPR_HEX);
	}

	std::string HEX(unsigned hash_id) {
		return hash_as_string(hash_id, RHPR_HEX | RHPR_UPPERCASE);
	}

	std::string base32(unsigned hash_id) {
		return hash_as_string(hash_id, RHPR_BASE32);
	}

	std::string base64(unsigned hash_id) {
		return hash_as_string(hash_id, RHPR_BASE64);
	}

	size_t size() {
		return length;
	}

	void hash_file(const std::string& filename) {
		FILE* file = fopen(filename.c_str(), "rb");
		if (file == nullptr) {
			throw std::runtime_error("Failed to open file for hashing: " + filename);
		}
		reinit();  //Unconditionally reinit when loading a new file
		char buffer[1024];
		size_t bytes_read;
		while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
			rhash_update(ctx_, buffer, bytes_read);
			length += bytes_read;
		}

		fclose(file);
		rhash_final(ctx_, nullptr);
	}

private:
	struct rhash_context* ctx_ = nullptr;
	std::vector<unsigned> hash_ids;
	size_t length = 0;
	bool done = false;
};