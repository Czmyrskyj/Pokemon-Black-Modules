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

enum class STAT : u8{
	ATK,
	DEF,
	SPA,
	SPD,
	SPE,
};

constexpr int nature_percent(NATURE n, STAT s){
	STAT up, down;
	switch (n){
		case NATURE::ADAMANT:	up = STAT::ATK; down = STAT::SPA; break;
		case NATURE::BRAVE:	up = STAT::ATK; down = STAT::SPE; break;
		case NATURE::LONELY:	up = STAT::ATK; down = STAT::DEF; break;
		case NATURE::NAUGHTY:	up = STAT::ATK; down = STAT::SPD; break;
		case NATURE::BOLD:	up = STAT::DEF; down = STAT::ATK; break;
		case NATURE::IMPISH:	up = STAT::DEF; down = STAT::SPA; break;
		case NATURE::LAX:	up = STAT::DEF; down = STAT::SPD; break;
		case NATURE::RELAXED:	up = STAT::DEF; down = STAT::SPE; break;
		case NATURE::MODEST:	up = STAT::SPA; down = STAT::ATK; break;
		case NATURE::MILD:	up = STAT::SPA; down = STAT::DEF; break;
		case NATURE::QUIET:	up = STAT::SPA; down = STAT::SPE; break;
		case NATURE::RASH:	up = STAT::SPA; down = STAT::SPD; break;
		case NATURE::CALM:	up = STAT::SPD; down = STAT::ATK; break;
		case NATURE::CAREFUL:	up = STAT::SPD; down = STAT::SPA; break;
		case NATURE::GENTLE:	up = STAT::SPD; down = STAT::DEF; break;
		case NATURE::SASSY:	up = STAT::SPD; down = STAT::SPE; break;
		case NATURE::HASTY:	up = STAT::SPE; down = STAT::DEF; break;
		case NATURE::JOLLY:	up = STAT::SPE; down = STAT::SPA; break;
		case NATURE::NAIVE:	up = STAT::SPE; down = STAT::SPD; break;
		case NATURE::TIMID:	up = STAT::SPE; down = STAT::ATK; break;
		default: return 100;
	}
	return s == up ? 110 : (s == down ? 90 : 100);
}

class assets_t;
class spritedex_t;

constexpr std::array<std::array<u8, 17>, 17> type_chart = {{
	/*NOR*/ {2,2,2,2,2,2,2,2,2,2,2,2,1,0,2,2,1},
	/*FIR*/ {2,1,1,2,4,4,2,2,2,2,2,4,1,2,1,2,4},
	/*WAT*/ {2,4,1,2,1,2,2,2,4,2,2,2,4,2,1,2,2},
	/*ELE*/ {2,2,4,1,1,2,2,2,0,4,2,2,2,2,1,2,2},
	/*GRA*/ {2,1,4,2,1,2,2,1,4,1,2,1,4,2,1,2,1},
	/*ICE*/ {2,1,1,2,4,1,2,2,4,4,2,2,2,2,4,2,1},
	/*FIG*/ {4,2,2,2,2,4,2,1,2,1,1,1,4,0,2,4,4},
	/*POI*/ {2,2,2,2,4,2,2,1,1,2,2,2,1,1,2,2,0},
	/*GRO*/ {2,4,2,4,1,2,2,4,2,0,2,1,4,2,2,2,4},
	/*FLY*/ {2,2,2,1,4,2,4,2,2,2,2,4,1,2,2,2,1},
	/*PSY*/ {2,2,2,2,2,2,4,4,2,2,1,2,2,2,2,0,1},
	/*BUG*/ {2,1,2,2,4,2,1,1,2,1,4,2,2,1,2,4,1},
	/*ROC*/ {2,4,2,2,2,4,1,2,1,4,2,4,2,2,2,2,1},
	/*GHO*/ {0,2,2,2,2,2,2,2,2,2,4,2,2,4,2,1,1},
	/*DRA*/ {2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,2,1},
	/*DAR*/ {2,2,2,2,2,2,1,2,2,2,4,2,2,4,2,1,1},
	/*STE*/ {2,1,1,1,2,4,2,2,2,2,2,2,4,2,2,2,1},	
}};

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
	u16 m_held_item = 0;
	bool m_focus_energy = false;
	u16 calc_stat(u8 base, STAT s) const{
		const u32 raw = ((2u * base + m_iv + m_ev / 4) * m_lvl) / 100 + 5;
		return static_cast<u16>(raw * nature_percent(m_nature, s) / 100);
	}
	u8 type_effectiveness(TYPE atk, TYPE def1, TYPE def2) const{
		u8 a = static_cast<u8>(atk) - 1;
		u8 e = type_chart[a][static_cast<u8>(def1) - 1];
		if (def2 != TYPE::NONE) e *= type_chart[a][static_cast<u8>(def2) - 1];
		else e *= 2;
		return e;
	}
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
	u16 calc_damage(const move_t& mv, const pokemon_t& def, bool crit, u8 roll) const{
		if (mv.power == 0 || mv.damage_class == static_cast<u8>(CATEGORY::STATUS)) return 0;

		const bool special = mv.damage_class == static_cast<u8>(CATEGORY::SPECIAL);
		const u32 a = special ? sp_att() : attack();
		const u32 d = std::max<u32>(1, special ? def.sp_def() : def.defense());

		u32 dmg = ((2u * m_lvl / 5 + 2) * mv.power * a / d) / 50 + 2;
		if (crit) dmg *= 2;
		dmg = dmg * roll / 100;
		if (mv.type == pokedex.type1 || mv.type == pokedex.type2) dmg = dmg * 3 / 2;
		
		const u8 type_eff = type_effectiveness(mv.type, def.pokedex.type1, def.pokedex.type2);
		dmg = dmg * type_eff / 4;
		if (type_eff != 0 && dmg == 0) dmg = 1;
		return static_cast<u16>(std::min<u32>(dmg, 0xFFFF));
	}
	void hp_damage(u16 dmg) { 
		m_hp = dmg >= m_hp ? 0 : m_hp - dmg;
	}
	void hp_heal(u16 power) {
		const u32 healed = static_cast<u32>(m_hp) + power;
		m_hp = healed >= m_max_hp ? m_max_hp : healed;
	}
	u16 attack() const{ return calc_stat(pokedex.base_attack, STAT::ATK); }
	u16 defense() const{ return calc_stat(pokedex.base_defense, STAT::DEF); }
	u16 sp_att() const{ return calc_stat(pokedex.base_sp_att, STAT::SPA); }
	u16 sp_def() const{ return calc_stat(pokedex.base_sp_def, STAT::SPD); }
	u16 speed() const{ return calc_stat(pokedex.base_speed, STAT::SPE); }
	std::span<const u16> moves() const{ return m_moves; }
	std::span<const u8> pp_used() const{ return m_pp_used; }
	std::array<u8, 4>& get_pp_used_array() { return m_pp_used; }
	u8 move_count() const { return m_move_count; }
	void move_count_delta(u8 delta){
		m_move_count += delta;
	}
	void revive(bool full){
		if (m_hp != 0) return;
		m_hp = full ? m_max_hp : std::max<u16>(1, m_max_hp / 2);
	}
	u16 held_item() const{
		return m_held_item;
	}
	void set_held_item(u16 id){
		m_held_item = id;
	}
	bool focus_energy() const{
		return m_focus_energy;
	}
	void set_focus_energy(bool f){
		m_focus_energy = f;
	}
};
struct legendary_t{
	pokemon_t pokemon;
};
