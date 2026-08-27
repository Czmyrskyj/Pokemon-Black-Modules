#pragma once

#include "common.hpp"
#include "pokedex.hpp"
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

struct pokemon_t{
	pokedex_entry_t pokedex;
	std::string nickname	= "";
	NATURE nature		= NATURE::NONE;
};


struct legendary_t{
	pokemon_t pokemon;
};
