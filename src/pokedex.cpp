#include "pokedex.hpp"
#include "common.hpp"
#include <fstream>
#include <filesystem>
#include <vector>
#include <print>

namespace fs = std::filesystem;

#define rd_memb(dest, T) in.read(reinterpret_cast<char*>(&dest), sizeof(T))

std::expected<void, std::string_view> pokedex_t::load_pokedex(){
	if (!fs::exists("POKEDEX")){
		return std::unexpected("POKEDEX is absent.");
	}
	std::ifstream in("POKEDEX", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open POKEDEX");
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
		pokemon_registry[id] = offset;
	}
	pokemon_cache.resize(PK_COUNT, std::nullopt);
	return {};
}


std::optional<pokedex_entry_t> pokedex_t::get_pokedex_entry(POKEDEX id){
	if (id >= PK_COUNT){
		return std::nullopt;
	}
	if (pokemon_cache[id].has_value()){
		return pokemon_cache[id].value();
	}
	u32 offset = pokemon_registry[id];
	if (offset == 0 && id != MISSINGNO){
		std::println(stderr, "no offset found for: {}", static_cast<u16>(id));
		return std::nullopt;
	}
	std::ifstream in("POKEDEX", std::ios::binary);
	if (!in.is_open()) {
		std::println(stderr, "POKEDEX is absent");
		return std::nullopt;
	}
	in.seekg(offset, std::ios::beg);

	pokedex_entry_t entry{};

	in.read(reinterpret_cast<char*>(&entry), sizeof(pokedex_entry_t));
	entry.id = id;

	if (!in) {
		std::println("failed to read bytes for pokemon id: {}", static_cast<u16>(id));
		return std::nullopt;
	}	

	pokemon_cache[id] = entry;
	return entry;
}
