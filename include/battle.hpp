#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "trainer.hpp"
#include "dialog.hpp"
#include "random.hpp"
#include <variant>
#include <format>
#include <algorithm>
#include <print>

using foe_t = std::variant<trainer_t, pokemon_t, legendary_t>;

enum class TURN_ACTION{
	ATTACK,
	ITEM,
	SWITCH
};

enum class BATTLE_TURN{
	TRAINER,
	FOE,
};

struct none_action_t{};

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

using turn_action_t = std::variant<none_action_t, attack_action_t, item_action_t, switch_pokemon_action_t>;

class battle_t{
	trainer_t& player;
	foe_t foe;
	party_t* foe_party;
	u8 m_txtbox_alpha = 0;
	u8 m_txtbox_peak_alpha_timer = 0;
	u8 m_current_foe_pokemon_idx = 0;
	u8 m_current_trainer_pokemon_idx = 0;
	turn_action_t m_foe_turn_action = none_action_t{};
	turn_action_t m_trainer_turn_action = none_action_t{};

	BATTLE_TURN m_attack_turn = BATTLE_TURN::TRAINER;
	public:
	battle_t(trainer_t& p, foe_t& f, party_t& party) : player(p), foe(f), foe_party(&party){};
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
	void set_action(TURN_ACTION act, BATTLE_TURN turn, u8 reg1, u8 reg2 = 0xFF){
		((turn == BATTLE_TURN::TRAINER) ? m_trainer_turn_action : m_foe_turn_action) = [&]() -> turn_action_t{
			switch (act){
				case TURN_ACTION::ATTACK: return attack_action_t{reg1, reg2};
				case TURN_ACTION::ITEM: return item_action_t{reg1, reg2};
				case TURN_ACTION::SWITCH: return switch_pokemon_action_t{reg1};
			}
			return {};
		}();
		
	}
	void set_ai_action(){
		if (std::holds_alternative<trainer_t>(foe)){
			auto& t = std::get<trainer_t>(foe);
			auto& cur_pokemon = (*foe_party)[m_current_foe_pokemon_idx];
			if (std::ranges::contains(std::array{TRAINER_TYPE::PKMN_TRAINER, 
								TRAINER_TYPE::ACE_TRAINER, 
								TRAINER_TYPE::DOCTOR, 
								TRAINER_TYPE::NURSE, 
								TRAINER_TYPE::VETERAN, 
								TRAINER_TYPE::GYM_LEADER, 
								TRAINER_TYPE::ELITE_FOUR, 
								TRAINER_TYPE::BOSS}, t.trainer_type())){
				
	
				float hp_ratio = cur_pokemon.hp() / cur_pokemon.max_hp();
				if (hp_ratio > 0.2f){
					set_action(TURN_ACTION::ATTACK, BATTLE_TURN::FOE, rnd::get_val<u8>(0, cur_pokemon.move_count() - 1), 0);
				} else {
					set_action(TURN_ACTION::ITEM, BATTLE_TURN::FOE, 0, 0);
				}

			} else {
			 
			/*  WARN:
			 *  RIGHT NOW, TARGET IS HARDCODED TO 0, SHOULD BE DYNAMIC IN FUTURE;
			 */
				set_action(TURN_ACTION::ATTACK, BATTLE_TURN::FOE, rnd::get_val<u8>(0, cur_pokemon.move_count() - 1), 0);
			}
		} else {

			/*  INFO:
			 *  THIS IS POKEMON AI, IT ALWAYS ATTACKS;
			 *  WARN:
			 *  RIGHT NOW, TARGET IS HARDCODED TO 0, SHOULD BE DYNAMIC IN FUTURE;
			 */
			const auto& p = std::get<pokemon_t>(foe);
			set_action(TURN_ACTION::ATTACK, BATTLE_TURN::FOE, rnd::get_val<u8>(0, p.move_count() - 1), 0);
		}
	}
	void execute_action(movedex_t& movedex, assets_t& assets){
		auto& foe_pokemon = (*foe_party)[m_current_foe_pokemon_idx];
		auto& ally_pokemon = player.get_party()[m_current_trainer_pokemon_idx];
		if (std::holds_alternative<trainer_t>(foe)){
			if (!std::holds_alternative<attack_action_t>(m_trainer_turn_action)){
				m_trainer_turn_action = none_action_t{};
			} else {
				move_t used_move = movedex.get_move(player.get_party()[m_current_trainer_pokemon_idx].moves()[std::get<attack_action_t>(m_trainer_turn_action).move_slot]);
				auto power = used_move.power;
				PlaySound(assets.get_sfx(std::format("{}.wav", used_move.name)));
				foe_pokemon.hp_damage(power, ally_pokemon.lvl(), ally_pokemon.get_pokedex().base_attack);
				m_trainer_turn_action = none_action_t{};
			}
		}
		if (!std::holds_alternative<attack_action_t>(m_foe_turn_action)){
			m_foe_turn_action = none_action_t{};
		} else {
			move_t used_move = movedex.get_move((*foe_party)[m_current_foe_pokemon_idx].moves()[std::get<attack_action_t>(m_foe_turn_action).move_slot]);
			auto power = used_move.power;
			PlaySound(assets.get_sfx(std::format("{}.wav", used_move.name)));
			ally_pokemon.hp_damage(power, foe_pokemon.lvl(), foe_pokemon.get_pokedex().base_attack);
			m_foe_turn_action = none_action_t{};
		}
	}
	turn_action_t trainer_turn_action() const{
		return m_trainer_turn_action;
	}
};
