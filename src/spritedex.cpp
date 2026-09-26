#include "spritedex.hpp"
#include "common.hpp"
#include <fstream>
#include <filesystem>
#include <vector>
#include <print>

namespace fs = std::filesystem;

#define rd_memb(dest, T) in.read(reinterpret_cast<char*>(&dest), sizeof(T))

std::expected<void, std::string_view> spritedex_t::load_spritedex(){
	if (!fs::exists("SPRITEDEX")){
		return std::unexpected("SPRITEDEX is absent.");
	}
	std::ifstream in("SPRITEDEX", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open SPRITEDEX");
	}

	u32 total_entries;
	rd_memb(total_entries, u32);

	for (u32 i = 0; i < total_entries; i++){
		u32 id;
		u32 offset;
		rd_memb(id, u32);
		rd_memb(offset, u32);
		if (id >= PK_COUNT){
			return std::unexpected("id exceeds PK_COUNT");
		}
		spritedex_registry[id] = offset;
	}
	spritedex_cache.resize(PK_COUNT, std::nullopt);
	return {};
}


std::optional<spritedex_entry_t> spritedex_t::get_spritedex_entry(POKEDEX id){
	if (id >= PK_COUNT){
		return std::nullopt;
	}
	if (spritedex_cache[id].has_value()){
		return spritedex_cache[id].value();
	}
	u32 offset = spritedex_registry[id];
	if (offset == 0 && id != MISSINGNO){
		std::println(stderr, "no offset found for: {}", static_cast<u16>(id));
		return std::nullopt;
	}
	std::ifstream in("SPRITEDEX", std::ios::binary);
	if (!in.is_open()) {
		std::println(stderr, "SPRITEDEX is absent");
		return std::nullopt;
	}
	in.seekg(offset, std::ios::beg);

	spritedex_entry_t entry{};

	in.read(reinterpret_cast<char*>(&entry), sizeof(spritedex_entry_t));

	if (!in) {
		std::println("failed to read spritedex bytes for pokemon id: {}", static_cast<u16>(id));
		return std::nullopt;
	}	

	spritedex_cache[id] = entry;
	return entry;
}
