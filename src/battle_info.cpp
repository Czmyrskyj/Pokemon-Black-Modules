#include "battle_info.hpp"
#include "random.hpp"
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

battle_entry_t battle_info_t::get_battle_info(REGION region, u8 index, pokedex_t& pokedex){
	u16 total_index = static_cast<u8>(region) + index;
	std::ifstream in("BATTLEINFO", std::ios::binary);
	if (!in.is_open()){
		std::println("failed to open BATTLEINFO");
		return {};
	}

	in.seekg(table.at(total_index).offset);

	OPPONENT battle_type;
	in.read(reinterpret_cast<char*>(&battle_type), sizeof(OPPONENT));

	opp_entry_t opp = [&]() -> opp_entry_t{
		switch (battle_type){
			case OPPONENT::TRAINER: return trainer_t{};			
			case OPPONENT::WILD: return pokemon_t{};
			case OPPONENT::LEGENDARY: return legendary_t{};
			case OPPONENT::GYM_LEADER: return trainer_t{};
		}
		return {};
	}();

	u32 name_len;
	in.read(reinterpret_cast<char*>(&name_len), sizeof(u32));
	std::string name(name_len, '\0');
	in.read(name.data(), name_len);

	u32 meet_msg_len;
	in.read(reinterpret_cast<char*>(&meet_msg_len), sizeof(u32));
	std::string meet_msg(meet_msg_len, '\0');
	in.read(meet_msg.data(), meet_msg_len);

	u32 def_msg_len;
	in.read(reinterpret_cast<char*>(&def_msg_len), sizeof(u32));
	std::string def_msg(def_msg_len, '\0');
	in.read(def_msg.data(), def_msg_len);

	u16 reward;
	in.read(reinterpret_cast<char*>(&reward), sizeof(u16));

	u32 dialog_count = 0;
	in.read(reinterpret_cast<char*>(&dialog_count), sizeof(u32));
	std::vector<std::string> dialog;
	if (dialog_count > 0){
		dialog.reserve(dialog_count);
		for (u32 i = 0; i < dialog_count; i++){
			u32 dlg_len = 0;
			in.read(reinterpret_cast<char*>(&dlg_len), sizeof(u32));

			std::string dlg(dlg_len, '\0');
			in.read(dlg.data(), dlg_len);
			dialog.push_back(std::move(dlg));
		}
	}

	u32 prefix_len;
	in.read(reinterpret_cast<char*>(&prefix_len), sizeof(u32));

	std::string prefix(prefix_len, '\0');
	in.read(prefix.data(), prefix_len);

	u8 party_len;
	in.read(reinterpret_cast<char*>(&party_len), sizeof(u8));
	party_t party;
	while (party.size() < party_len){
		POKEDEX id;
		in.read(reinterpret_cast<char*>(&id), sizeof(u16));
		auto pokeentry = pokedex.get_pokedex_entry(id);
		if (!pokeentry.has_value()){
			pokeentry = pokedex.get_pokedex_entry(MISSINGNO);
		}
		u8 lvl;
		in.read(reinterpret_cast<char*>(&lvl), sizeof(u8));
		pokemon_t pokemon{
			pokeentry.value(),
			"",
			rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
			rnd::get_val<SEX>(SEX::MALE, SEX::FEMALE),
			lvl,
		};
		party.push_back(pokemon);
	}

	battle_entry_t battle_entry = {
		.foe = {
			battle_type,
			opp,
		},
		.name = name,
		.meet_msg = meet_msg,
		.defeat_msg = def_msg,
		.reward = reward,
		.dialog = dialog,
		.prefix = prefix,
		.party = party,
	};
	return battle_entry;
}
