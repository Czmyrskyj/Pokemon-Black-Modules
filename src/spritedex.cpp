#include "spritedex.hpp"
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


std::expected<void, std::string_view> spritedex_t::load_spritedex(){
	if (!fs::exists("SPRITEDEX")){
		return std::unexpected("SPRITEDEX is absent.");
	}
	std::ifstream in("SPRITEDEX", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open SPRITEDEX");
	}

	spritedex_entry_t entry;
	while (in.read(reinterpret_cast<char*>(&entry), sizeof(entry))){
		spritedex_registry.push_back(entry);
	}

	return {};
}


spritedex_entry_t spritedex_t::get_spritedex_entry(POKEDEX id){
	return spritedex_registry[id];
}
