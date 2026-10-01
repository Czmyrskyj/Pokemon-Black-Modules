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
    pokedex_entry_t entry;
    while (in.read(reinterpret_cast<char*>(&entry), sizeof(pokedex_entry_t))){
        pokemon_registry.push_back(entry);
    }
    return {};
}

pokedex_entry_t pokedex_t::get_pokedex_entry(POKEDEX id){
    return pokemon_registry[id];
}
