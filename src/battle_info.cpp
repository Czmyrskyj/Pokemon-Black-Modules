#include "battle_info.hpp"
#include <filesystem>
#include <fstream>
#include <print>

namespace fs = std::filesystem;

std::expected<void, std::string_view> battle_info_t::load_battle_info(){
	if (!fs::exists("BATTLEINFO")){
		return std::unexpected("BATTLEINFO is absent.");
	}
	std::ifstream in("BATTLEINFO", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open BATTLEINFO");
	}
	u32 entries_amount = 0;
	in.read(reinterpret_cast<char*>(&entries_amount), sizeof(u32));
	table.resize(entries_amount);
	in.read(reinterpret_cast<char*>(table.data()), entries_amount * sizeof(lookup_t));
	return {};
}

const battle_entry_t& battle_info_t::get_battle_info(STATE state, u8 index){
	u16 total_index = static_cast<u8>(state) + index;
	std::ifstream in("BATTLEINFO", std::ios::binary);
	if (!in.is_open()){
		std::println("failed to open BATTLEINFO");
		return infos[0];
	}
	
	return infos[total_index];
}
