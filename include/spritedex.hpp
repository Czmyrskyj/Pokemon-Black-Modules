#pragma once
#include "common.hpp"
#include "pokedex.hpp"

#pragma pack(push, 1)
struct spritedex_entry_t{
	u8 front_frames;
	u16 front_w;
	u16 front_h;
	u8 back_frames;
	u16 back_w;
	u16 back_h;
};
#pragma pack(pop)

class spritedex_t{
	std::vector<spritedex_entry_t> spritedex_registry{};
	public:
	spritedex_t(){
		spritedex_registry.reserve(PK_COUNT);
	};
	std::expected<void, std::string_view> load_spritedex();
	spritedex_entry_t get_spritedex_entry(POKEDEX ID);
};

