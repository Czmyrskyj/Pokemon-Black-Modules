#include "items.hpp"
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::expected<void, std::string_view> itemdex_t::load_itemdex(){
	if (!fs::exists("ITEMDEX")){
		return std::unexpected("ITEMDEX is absent.");
	}
	std::ifstream in("ITEMDEX", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open ITEMDEX");
	}

	item_header_t item_h;
	while (in.read(reinterpret_cast<char*>(&item_h), sizeof(item_header_t))){
		std::string name(item_h.name_len, '\0');
		in.read(&name[0], item_h.name_len);
		item_registry.push_back(item_t{
			item_h.id,
			item_h.cost,
			item_h.fling_power,
			item_h.category,
			std::move(name),
		});
	}

	return {};
}

