#pragma once
#include "common.hpp"
#include <expected>
#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>
#include <raylib.h>
#include <span>

struct asset_entry_t{
	std::string name;
	usz size;
	usz offset;
};

struct assets_t{
	std::vector<u8> memory_pool;
	std::vector<asset_entry_t> asset_registry;
	public:
	std::expected<void, std::string_view> load_assets();
	std::span<const u8> get(std::string_view name) const;
};
