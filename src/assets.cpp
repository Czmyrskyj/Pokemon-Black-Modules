#include "assets.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
const fs::path assets_pakg{"assets.PAKG"};

std::expected<void, std::string_view> assets_t::load_assets(){
	if (!fs::exists(assets_pakg)){
		return std::unexpected("assets.PAKG is absent");
	}
	std::ifstream in(assets_pakg, std::ios::binary | std::ios::ate);
	if (!in.is_open()){
		return std::unexpected("failed to open assets.PAKG");
	}
	std::streamsize file_size = in.tellg();
	if (file_size == std::streamsize(-1)){
		return std::unexpected("failed to determine assets.PAKG size");
	}
	in.seekg(0, std::ios::beg);
	memory_pool.resize(file_size);
	in.read(reinterpret_cast<char*>(memory_pool.data()), memory_pool.size());
	in.close();
	usz cursor = 0;
	usz assets_count = *reinterpret_cast<const u32*>(&memory_pool[cursor]);
	cursor += sizeof(u32);
	asset_registry.resize(assets_count);
	for (usz i = 0; i < assets_count; i++){
		u32 path_len = *reinterpret_cast<const u32*>(&memory_pool[cursor]);
		cursor += sizeof(u32);
		std::string path(reinterpret_cast<const char*>(&memory_pool[cursor]), path_len);
		cursor += path_len;
		u32 size = *reinterpret_cast<const u32*>(&memory_pool[cursor]);
		cursor += sizeof(u32);
		u32 offset = *reinterpret_cast<const u32*>(&memory_pool[cursor]);
		cursor += sizeof(u32);
		if (offset + size > memory_pool.size()){
			return std::unexpected("asset offset out of bounds");
		}
		const u8* raw_data_ptr = &memory_pool[offset];
		asset_registry[i] = {
			.name = path,
			.size = size,
			.offset = offset,
		};
	}
	return {};
}

std::span<const u8> assets_t::get(std::string_view name) const{
	for (const auto& asset : asset_registry){
		if (name == asset.name){
			return std::span<const u8>(memory_pool.data() + asset.offset, asset.size);
		}
	}
	return {};
}
