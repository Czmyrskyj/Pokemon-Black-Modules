#pragma once

#include "common.hpp"
#include "pokedex.hpp"
#include "sex.hpp"
#include <string>

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
	u8 m_iv = 0;
	u8 m_ev = 0;
	u32 m_exp = 0;
	u32 m_exp_cap = 0;
	public:
	pokemon_t() = default;
	pokemon_t(pokedex_entry_t p, std::string nick, NATURE nat, SEX s, u8 lv) : pokedex(std::move(p)), m_nickname(std::move(nick)), m_nature(nat), m_sex(s), m_lvl(lv){
		m_hp = (((2 * pokedex.base_hp) + m_iv + (m_ev / 4)) * m_lvl) / 100 + m_lvl + 10;
		m_max_hp = m_hp;
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
};
struct legendary_t{
	pokemon_t pokemon;
};
