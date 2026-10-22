#pragma once
#include "common.hpp"
#include <expected>
#include <vector>
#include "battle.hpp"
#include "regions.hpp"

struct battle_entry_t{
	foe_t foe;
	std::string name{};
	std::string meet_msg{};
	std::string defeat_msg{};
	u16 reward;
	std::vector<std::string> dialog;
	TRAINER_TYPE trainer_type;
	party_t party;
	std::vector<u8> wild_rates;
};

struct lookup_t{
	u32 size;
	u32 offset;
};


class battle_info_t{
	std::vector<lookup_t> table;
	u8 wild_rate_idx = 0;
	public:
		std::expected<void, std::string_view> load_battle_info();
		battle_entry_t get_battle_info(REGION region, usz index, pokedex_t& pokedex);
};
