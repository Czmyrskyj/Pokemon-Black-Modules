#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "trainer.hpp"
#include "dialog.hpp"
#include "random.hpp"
#include "item_effects.hpp"
#include <variant>
#include <format>
#include <algorithm>
#include <optional>
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

enum class BATTLE_FIGURE{
	NONE,
	TRAINER,
	FOE,
};

struct none_action_t{};

struct attack_action_t{
	u8 move_slot;
	u8 target;
};

struct item_action_t{
	u16 item_id;
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
	u8 m_turn_count = 0;
	turn_action_t m_foe_turn_action = none_action_t{};
	turn_action_t m_trainer_turn_action = none_action_t{};
	item_result_t m_last_item_result = {};

	BATTLE_FIGURE m_figure_missed = BATTLE_FIGURE::NONE;

	std::array<BATTLE_TURN, 2> m_order{};
	u8 m_order_pos = 2;

	BATTLE_FIGURE m_winner = BATTLE_FIGURE::NONE;
	bool m_is_crit = false;

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
	void set_current_foe_pokemon_idx(u8 idx) {
		m_current_foe_pokemon_idx = idx;
	}
	u8 txtbox_alpha() const{
		return m_txtbox_alpha;
	}
	void set_action(TURN_ACTION act, BATTLE_TURN turn, u8 reg1, u8 reg2 = 0xFF){
		((turn == BATTLE_TURN::TRAINER) ? m_trainer_turn_action : m_foe_turn_action) = [&]() -> turn_action_t{
			switch (act){
				case TURN_ACTION::ATTACK: return attack_action_t{static_cast<u8>(reg1), reg2};
				case TURN_ACTION::ITEM: return item_action_t{reg1, reg2};
				case TURN_ACTION::SWITCH: return switch_pokemon_action_t{static_cast<u8>(reg1)};
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
	i8 action_priority(const turn_action_t& a, const pokemon_t& user, const movedex_t& movedex){
		return std::visit([&](auto&& act) -> i8 {
				using T = std::decay_t<decltype(act)>;
				if constexpr (std::is_same_v<T, attack_action_t>){
				return movedex.get_move(user.moves()[act.move_slot]).priority;
				} else if constexpr (std::is_same_v<T, switch_pokemon_action_t> || std::is_same_v<T, item_action_t>){
				return 10;
				} else {
				return -128;
				}
				}, a);
	}
	void determine_order(const movedex_t& movedex){
		auto& foe_pkmn = (*foe_party)[m_current_foe_pokemon_idx];
		auto& ally_pkmn = player.get_party()[m_current_trainer_pokemon_idx];

		i8 foe_pr = action_priority(m_foe_turn_action, foe_pkmn, movedex);
		i8 ally_pr = action_priority(m_trainer_turn_action, ally_pkmn, movedex);

		bool trainer_first;
		if (foe_pr != ally_pr) trainer_first = ally_pr > foe_pr;
		else if (foe_pkmn.speed() != ally_pkmn.speed()) trainer_first = ally_pkmn.speed() > foe_pkmn.speed();
		else trainer_first = rnd::get_val<u8>(0, 1) == 0;

		m_order = trainer_first ? 
			std::array{BATTLE_TURN::TRAINER, BATTLE_TURN::FOE} :
			std::array{BATTLE_TURN::FOE, BATTLE_TURN::TRAINER};
		m_order_pos = 0;
		m_turn_count++;
	}
	std::optional<BATTLE_TURN> next_turn() const{
		if (m_order_pos >= 2) return std::nullopt;
		return m_order[m_order_pos];
	}
	void execute_item(const item_action_t& ia, bool is_trainer, const itemdex_t& itemdex, assets_t& assets){
		if (!is_trainer) return;
		const item_t& item = itemdex.get_item(ia.item_id);
		auto& party = player.get_party();
		auto& foep = (*foe_party)[m_current_foe_pokemon_idx];
		auto& ally = party[m_current_trainer_pokemon_idx];
		pokemon_t* target = nullptr;
		switch (item.target){
			case ITEM_TARGET::ALLY: 
				{
					if (ia.target < party.size()) target = &party[ia.target]; 
					PlaySound(assets.get_sfx("SEQ_SE_BE_SE_09.wav"));
				}
				break;
			case ITEM_TARGET::FOE: target = &foep; break;
			case ITEM_TARGET::NONE: break;
		}
		if (!target) return;

		catch_ctx_t cctx{};
		cctx.turn = m_turn_count;
		cctx.ally_level = ally.lvl();
		cctx.ally_sex = ally.sex();
		cctx.ally_species = ally.get_pokedex().id;
		
		const bool was_fainted = target->hp() <= 0;
		m_last_item_result = apply_item(item, *target, std::holds_alternative<trainer_t>(foe), cctx);
		

		if (m_last_item_result.consumed()){
			player.consume_item(ia.item_id);
		}
		if (m_last_item_result.kind == ITEM_RESULT::REVIVED && was_fainted){
			player.set_conscious_count(player.conscious_count() + 1);
		}
		if (m_last_item_result.kind == ITEM_RESULT::CAUGHT){
			player.push_pokemon(foep);
			m_winner = BATTLE_FIGURE::TRAINER;
		}
	}
	void execute_action(movedex_t& movedex, const itemdex_t& itemdex, assets_t& assets){
		if (m_order_pos >= 2) return;

		bool is_trainer = m_order[m_order_pos++] == BATTLE_TURN::TRAINER;

		auto& foep = (*foe_party)[m_current_foe_pokemon_idx];
		auto& ally = player.get_party()[m_current_trainer_pokemon_idx];
		auto& action = is_trainer ? m_trainer_turn_action : m_foe_turn_action;
		auto& atk = is_trainer ? ally : foep;
		auto& def = is_trainer ? foep : ally;

		if (atk.hp() > 0 && std::holds_alternative<attack_action_t>(action)){
			u8 slot = std::get<attack_action_t>(action).move_slot;
			const move_t& mv = movedex.get_move(atk.moves()[slot]);
			u8 mv_acc = rnd::get_val<u8>(1, 100);
			if (mv.accuracy == 0 || mv_acc <= mv.accuracy){
				PlaySound(assets.get_sfx(std::format("{}.wav", mv.name)));

				int crit_stage = mv.crit_rate;
				if (atk.focus_energy()) crit_stage += 2;
				if (atk.held_item() == 209 || atk.held_item() == 303) crit_stage++;
				if (atk.get_pokedex().ability1 == ABILITY::SUPER_LUCK) crit_stage++;

				if (crit_stage > 4) crit_stage = 4;
				
				const auto& dd = def.get_pokedex();
				bool blocked = dd.ability1 == ABILITY::BATTLE_ARMOR || dd.ability2 == ABILITY::BATTLE_ARMOR
					|| dd.ability1 == ABILITY::SHELL_ARMOR  || dd.ability2 == ABILITY::SHELL_ARMOR;

				if (!blocked){
					static constexpr std::array<int, 5> crit_denominator = {16, 8, 4, 3, 2};
					bool always_crit = mv.id == 524 || mv.id == 480;
					m_is_crit = always_crit || rnd::get_val<u8>(0, crit_denominator[crit_stage] - 1) == 0;
				}
				def.hp_damage(mv.power * (m_is_crit ? 2 : 1), mv.type , atk.lvl(), atk.get_pokedex().base_attack);
			} else {
				m_figure_missed = is_trainer ? BATTLE_FIGURE::TRAINER : BATTLE_FIGURE::FOE;
			}
			auto& used = atk.get_pp_used_array()[slot];
			if (used < mv.pp) used++;
		} else if (const auto* ia = std::get_if<item_action_t>(&action)){
			execute_item(*ia, is_trainer, itemdex, assets);
		}
		action = none_action_t{};

	}
	turn_action_t trainer_turn_action() const{
		return m_trainer_turn_action;
	}
	turn_action_t foe_turn_action() const{
		return m_foe_turn_action;
	}
	void set_winner(BATTLE_FIGURE who){
		m_winner = who;
	}
	BATTLE_FIGURE winner() const{
		return m_winner;
	}
	void set_txtbox_alpha(u8 alpha){
		m_txtbox_alpha = alpha;
	}
	BATTLE_FIGURE figure_missed() const{
		return m_figure_missed;
	}
	void set_figure_missed(BATTLE_FIGURE f){
		m_figure_missed = f;
	}
	item_result_t last_item_result() const{
		return m_last_item_result;
	}
	bool is_crit() const{
		return m_is_crit;
	}
	void set_crit(bool c){
		m_is_crit = c;
	}
};
