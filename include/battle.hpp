#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "trainer.hpp"
#include "dialog.hpp"
#include <variant>


using foe_t = std::variant<trainer_t, pokemon_t, legendary_t>;

struct attack_action_t{
	u8 move_slot;
	u8 target;
};

struct item_action_t{
	u8 item_id;
	u8 target;
};

struct switch_pokemon_action_t{
	u8 party_idx;
};

using turn_action_t = std::variant<attack_action_t, item_action_t, switch_pokemon_action_t>;

class battle_t{
	trainer_t& player;
	foe_t foe;
	u8 m_txtbox_alpha = 0;
	u8 m_txtbox_peak_alpha_timer = 0;
	u8 m_current_foe_pokemon_idx = 0;
	u8 m_current_trainer_pokemon_idx = 0;
	public:
	battle_t(trainer_t& p, foe_t& f) : player(p), foe(f){};
	~battle_t() = default;


	std::string get_dialog_intro();
	std::string get_dialog_foe_sent_out();

	void write(assets_t& assets, writer_t& writer);
	u8 current_foe_pokemon_idx() const {
		return m_current_foe_pokemon_idx;
	}
	u8 current_trainer_pokemon_idx() const{
		return m_current_trainer_pokemon_idx;
	}
	void set_current_trainer_pokemon_idx(u8 idx) {
		m_current_trainer_pokemon_idx = idx;
	}
	u8 txtbox_alpha() const{
		return m_txtbox_alpha;
	}
};
