#pragma once

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
	u8 txtbox_alpha = 0;
	u8 txtbox_peak_alpha_timer = 0;
	bool __finished_line = false;
	public:
	battle_t(trainer_t& p, foe_t& f) : player(p), foe(f){};
	battle_t(trainer_t& p, trainer_t& t) : player(p), foe(foe_t{OPPONENT::TRAINER, t}){};
	battle_t(trainer_t& p, pokemon_t& w) : player(p), foe(foe_t{OPPONENT::WILD, w}){};
	battle_t(trainer_t& p, legendary_t& l) : player(p), foe(foe_t{OPPONENT::LEGENDARY, l}){};
	battle_t(trainer_t& p, gym_leader_t& g) : player(p), foe(foe_t{OPPONENT::GYM_LEADER, g}){};
	~battle_t() = default;

	std::string get_battle_intro();
	void write(assets_t& assets, writer_t& writer);
	void finish_line(bool f){
		__finished_line = f;
	}
	bool finished_line() const{
		return __finished_line;
	}
};
