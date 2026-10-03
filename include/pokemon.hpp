#pragma once

#include "common.hpp"
#include "pokedex.hpp"
#include "sex.hpp"
#include "moves.hpp"
#include <span>
#include <string>
#include <cmath>

enum class NATURE : u8{
	NONE,
	BASHFUL,
	DOCILE,
	HARDLY,
	QUIRKY,
	SERIOUS,
	ADAMANT,
	BRAVE,
	LONELY,
	NAUGHTY,
	BOLD,
	IMPISH,
	LAX,
	RELAXED,
	MODEST,
	MILD,
	QUIET,
	RASH,
	CALM,
	CAREFUL,
	GENTLE,
	SASSY,
	HASTY,
	JOLLY,
	NAIVE,
	TIMID,
};

enum class POKEMON_RENDER_TYPE{
	FRONT,
	BACK,
	MENU,
};

class assets_t;
class spritedex_t;
class pokemon_t{
	u8 frame{0};
	float menu_y{0.0f};
	float frame_timer{0.0f};
	pokedex_entry_t pokedex{};
	std::string m_nickname	= "";
	NATURE m_nature		= NATURE::NONE;
	SEX m_sex = SEX::NONE;
	u16 m_hp = 0;
	u16 m_max_hp = 0;
	u8 m_lvl = 1;
	std::array<u16, 4> m_moves{};
	std::array<u8, 4> m_pp_used{};
	u8 m_move_count = 0;
	u8 m_iv = 0;
	u8 m_ev = 0;
	u32 m_exp = 0;
	u32 m_exp_cap = 0;
	bool m_frames_blocked = false;
	public:
	pokemon_t() = default;
	pokemon_t(pokedex_entry_t p, std::string nick, NATURE nat, SEX s, u8 lv, std::array<u16, 4> m, u8 mc) : pokedex(std::move(p)), m_nickname(std::move(nick)), m_nature(nat), m_sex(s), m_lvl(lv), m_moves(std::move(m)), m_move_count(mc){
		m_hp = (((2 * pokedex.base_hp) + m_iv + (m_ev / 4)) * m_lvl) / 100 + m_lvl + 10;
		m_max_hp = m_hp;
		m_pp_used.fill(0);
	}
	void render(assets_t& assets, spritedex_t& spritedex, POKEMON_RENDER_TYPE type, Vector3 pos, Vector2 scale, Camera3D* camera = nullptr);
	const pokedex_entry_t& get_pokedex() const{
		return pokedex;
	}
	float hp() const { return m_hp; }
	float max_hp() const { return m_max_hp; }
	u8 lvl() const { return m_lvl; }
	SEX sex() const { return m_sex; }
	std::string_view nickname() const { return m_nickname; }
	u32 exp() const { return m_exp; }
	u32 exp_cap() const { return m_exp_cap; }
	void calculate_exp_cap(){
		m_exp_cap = 3 * (m_lvl*m_lvl) + 3 * m_lvl + 1;
	}
	void block_frames(bool b) {m_frames_blocked = b; }
	void hp_damage(u16 power, u8 attacker_lvl, u8 attacker_attack) { 
		u16 final_power =  std::floor(std::floor(std::floor(2 * attacker_lvl / 5 + 2) * power * attacker_attack / pokedex.base_defense) / 50) + 2;
		m_hp -= final_power;
		if (static_cast<i16>(m_hp - final_power) <= 0) {
			m_hp = 0;
		}
	}
	std::span<const u16> moves() const{ return m_moves; }
	std::span<const u8> pp_used() const{ return m_pp_used; }
	std::array<u8, 4>& get_pp_used_array() { return m_pp_used; }
	u8 move_count() const { return m_move_count; }
	void move_count_delta(u8 delta){
		m_move_count += delta;
	}
};
struct legendary_t{
	pokemon_t pokemon;
};
