#include "pokedex.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::expected<void, std::string_view> pokedex_t::load_pokedex(){
    if (!fs::exists("POKEDEX")){
        return std::unexpected("POKEDEX is absent.");
    }
    std::ifstream in("POKEDEX", std::ios::binary);
    if (!in.is_open()){
        return std::unexpected("failed to open POKEDEX");
    }
    pokedex_header_t header;
    while (in.read(reinterpret_cast<char*>(&header), sizeof(pokedex_header_t))){
	    pokedex_entry_t entry{
		    		.id = header.id,
				.base_hp = header.base_hp,
		    		.base_attack = header.base_attack,
		    		.base_defense = header.base_defense,
				.base_speed = header.base_speed,
				.base_sp_att = header.base_sp_att,
				.base_sp_def = header.base_sp_def,
				.type1 = header.type1,
				.type2 = header.type2,
				.catch_rate = header.catch_rate,
				.base_friendship = header.base_friendship,
				.ability1 = header.ability1,
				.ability2 = header.ability2,
				.hidden_ability = header.hidden_ability,
				.gender_rate = header.gender_rate,
	    };
	    pokemon_registry.push_back(entry);
    }
    return {};
}

pokedex_entry_t pokedex_t::get_pokedex_entry(POKEDEX id){
	return pokemon_registry[id];
}
