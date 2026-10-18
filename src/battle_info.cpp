#include "battle_info.hpp"
#include "random.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
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

constexpr usz battle_offset(REGION r){
	usz sum = 0;
	for (usz i = 0; i < static_cast<u8>(r); i++){
		sum += encounters_count[i];
	}
	return sum;
}

battle_entry_t battle_info_t::get_battle_info(REGION region, u8 index, pokedex_t& pokedex){
	u16 total_index = battle_offset(region) + index;
	std::ifstream in("BATTLEINFO", std::ios::binary);
	if (!in.is_open()){
		std::println("failed to open BATTLEINFO");
		return {};
	}

	in.seekg(table.at(total_index).offset);

	u8 battle_type;
	in.read(reinterpret_cast<char*>(&battle_type), sizeof(u8));


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

	TRAINER_TYPE trainer_type;
	in.read(reinterpret_cast<char*>(&trainer_type), sizeof(TRAINER_TYPE));

	u8 party_len;
	in.read(reinterpret_cast<char*>(&party_len), sizeof(u8));
	party_t party;

	foe_t foe = [&]() -> foe_t{
		switch (battle_type){
			case 0: return trainer_t{name, trainer_type};
			case 1: return pokemon_t{};
			case 2: return legendary_t{};
		}
		return {};
	}();

	std::vector<u8> wild_rates;


	if (std::holds_alternative<trainer_t>(foe)){
		while (party.size() < party_len){
			POKEDEX id;
			in.read(reinterpret_cast<char*>(&id), sizeof(u16));
			auto pokeentry = pokedex.get_pokedex_entry(id);
			u8 lvl;
			in.read(reinterpret_cast<char*>(&lvl), sizeof(u8));

			std::array<u16, 4> moves;
			in.read(reinterpret_cast<char*>(moves.data()), moves.size()*sizeof(u16));


			u8 move_count = std::ranges::count_if(moves, [](u16 move_id){
					return move_id != 0;
					});

			pokemon_t pokemon{
				pokeentry,
					"",
					rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
					rnd::get_val<SEX>(SEX::MALE, SEX::FEMALE),
					lvl,
					moves,
					move_count,
			};
			party.push_back(pokemon);
		}
	} else {
		struct slot_key_t{
			u16 id;
			u8 lvl;
			std::array<u16, 4> moves;
			bool operator==(const slot_key_t&) const = default; 
		};
		std::vector<slot_key_t> seen;
		party = party_t{MAX_FOE_PARTY};

		for (usz slot = 0; slot < party_len; slot++){
			slot_key_t key{};
			in.read(reinterpret_cast<char*>(&key.id), sizeof(u16));
			in.read(reinterpret_cast<char*>(&key.lvl), sizeof(u8));
			in.read(reinterpret_cast<char*>(key.moves.data()), key.moves.size()*sizeof(u16));

			u8 rate = (slot < wild_encounter_slot_rates.size()) ? static_cast<u8>(wild_encounter_slot_rates[slot]) : 0;

			if (auto it = std::ranges::find(seen, key); it != seen.end()){
				wild_rates[it - seen.begin()] += rate;
				continue;
			}

			seen.push_back(key);
			wild_rates.push_back(rate);

			auto pokeentry = pokedex.get_pokedex_entry(static_cast<POKEDEX>(key.id));
			u8 move_count = std::ranges::count_if(key.moves, [](u16 move_id){
					return move_id != 0;
					});

			party.push_back(pokemon_t{
					pokeentry,
					"",
					rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
					rnd::get_val<SEX>(SEX::MALE, SEX::FEMALE),
					key.lvl,
					key.moves,
					move_count,
					});

		}
	}

	if (std::holds_alternative<trainer_t>(foe)){
		std::get<trainer_t>(foe).set_conscious_count(party.size());
	}
	battle_entry_t battle_entry = {
		.foe = foe,
		.name = name,
		.meet_msg = meet_msg,
		.defeat_msg = def_msg,
		.reward = reward,
		.dialog = dialog,
		.trainer_type = trainer_type,
		.party = party,
		.wild_rates = wild_rates,
	};
	return battle_entry;
}
