#include "moves.hpp"
#include "common.hpp"
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::expected<void, std::string_view> movedex_t::load_movedex(){
	if (!fs::exists("MOVEDEX")){
		return std::unexpected("MOVEDEX is absent.");
	}
	std::ifstream in("MOVEDEX", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open MOVEDEX");
	}

	move_header_t move_h;
	while (in.read(reinterpret_cast<char*>(&move_h), sizeof(move_header_t))){
		std::string name(move_h.name_len, '\0');
		in.read(&name[0], move_h.name_len);
		move_registry.push_back(move_t{
			move_h.id,
			move_h.type,
			move_h.power,
			move_h.pp,
			move_h.accuracy,
			static_cast<i8>(move_h.priority - 128),
			move_h.damage_class,
			move_h.crit_rate,
			std::move(name),
		});
	}

	return {};
}

