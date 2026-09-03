#pragma once

#include "common.hpp"

#include <expected>
#include <string_view>
#include <unordered_map>

class music_offset_t{
	std::unordered_map<u32, u32> offsets{}; 
	public:
		std::expected<void, std::string_view> load();
		u32 get(std::string_view key);
};
