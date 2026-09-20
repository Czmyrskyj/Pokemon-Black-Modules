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
class pokemon_t{
	u8 frame{0};
	float menu_y{0.0f};
	float frame_timer{0.0f};
	pokedex_entry_t pokedex{};
	std::string nickname	= "";
	NATURE nature		= NATURE::NONE;
	u8 __lvl = 1;
	float __iv = 0.0f;
	float __ev = 0.0f;
	float __hp = 0.0f;
	float __max_hp = 0.0f;
	SEX __sex = SEX::NONE;
	public:
	pokemon_t() = default;
	pokemon_t(pokedex_entry_t p, std::string nick, NATURE nat, SEX s) : pokedex(std::move(p)), nickname(std::move(nick)), nature(nat), __sex(s){
		__hp = (((2 * pokedex.base_hp) + __iv + (__ev / 4)) * __lvl) / 100 + __lvl + 10;
		__max_hp = __hp;
	}
	void render(assets_t& assets, POKEMON_RENDER_TYPE type, Vector3 pos, Vector2 scale, Camera3D* camera = nullptr);
	const pokedex_entry_t& get_pokedex() const{
		return pokedex;
	}
	float hp() const { return __hp; }
	float max_hp() const { return __max_hp; }
	u8 lvl() const { return __lvl; }
	SEX sex() const { return __sex; }
};


struct legendary_t{
	pokemon_t pokemon;
};
