#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "trainer.hpp"
#include "dialog.hpp"
#include <variant>

enum class OPPONENT{
	TRAINER,
	WILD,
	LEGENDARY,
	GYM_LEADER,
};

using opp_entry_t = std::variant<trainer_t, pokemon_t, legendary_t, gym_leader_t>;

struct foe_t{
	OPPONENT opp;
	opp_entry_t entry;
};

class battle_t{
	trainer_t& player;
	foe_t foe;
	std::vector<std::string> lines;
	u8 current_line = 0;
	u8 txtbox_alpha = 0;
	u8 txtbox_peak_alpha_timer = 0;
	bool __finished_line = false;
	u8 current_foe_pokemon_idx = 0;
	bool __finished_dialog = false;
	bool __foe_sent_out_pokemon = false;
	public:
	battle_t(trainer_t& p, foe_t& f) : player(p), foe(f){
		if (f.opp == OPPONENT::WILD || f.opp == OPPONENT::LEGENDARY) __foe_sent_out_pokemon = true;
	};
	~battle_t() = default;

	void init_dialog(std::vector<std::string> l){
		lines = std::move(l);
		current_line = 0;
		txtbox_alpha = 0;
		txtbox_peak_alpha_timer = 0;
		__finished_line = false;
	}

	std::string get_dialog_intro();
	std::string get_dialog_trainer_sent_out();
	std::string step_dialog(writer_t& writer, Locale::locale_t& locale);

	void write(assets_t& assets, writer_t& writer);
	void finish_line(bool f){
		__finished_line = f;
	}
	bool finished_line() const{
		return __finished_line;
	}
	bool finished_dialog() const{
		return __finished_dialog;
	}
	u8 get_current_foe_pokemon_idx() const {
		return current_foe_pokemon_idx;
	}
	bool foe_sent_out_pokemon() const {
		return __foe_sent_out_pokemon;
	}
	void set_foe_sent_out_pokemon(bool s = true){
		__foe_sent_out_pokemon = s;
	}
};
