#pragma once
#include "common.hpp"
#include <expected>
#include <vector>
#include "battle.hpp"
#include "states.hpp"

struct battle_entry_t{
	foe_t foe;
	std::string meet_msg{};
	std::string defeat_msg{};
	u16 reward;
	std::optional<dialog_t> dialog;
};

struct lookup_t{
	u32 size;
	u32 offset;
};

class battle_info_t{
	std::vector<lookup_t> table;
	std::vector<battle_entry_t> infos;
	public:
		std::expected<void, std::string_view> load_battle_info();
		const battle_entry_t& get_battle_info(STATE state, u8 index);
};
