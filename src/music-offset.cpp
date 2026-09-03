
#include "music-offset.hpp"
#include "hash.hpp"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::expected<void, std::string_view> music_offset_t::load(){
	if (!fs::exists("MUSICOFFSET")){
		return std::unexpected("MUSICOFFSET is absent");
	}
	std::ifstream in("MUSICOFFSET", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open MUSICOFFSET");
	}
	
	u32 offsets_count;
	in.read(reinterpret_cast<char*>(&offsets_count), sizeof(u32));
	offsets.reserve(offsets_count);

	for (usz i = 0; i < offsets_count; i++){
		u32 hash;
		in.read(reinterpret_cast<char*>(&hash), sizeof(u32));

		u32 offset;
		in.read(reinterpret_cast<char*>(&offset), sizeof(u32));
		offsets[hash] = offset;
	}

	return {};
}

u32 music_offset_t::get(std::string_view key){
	u32 hash = get_hash(key);
	auto it = offsets.find(hash);
	if (it != offsets.end()){
		return it->second;
	}
	return 0;
}
