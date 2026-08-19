#pragma once

#include "common.hpp"
#include "pokedex.hpp"
#include <string>

using party_t = std::array<pokemon_t, 6>;

class trainer_t{
	std::string name;
	party_t party;
	public:
};

struct gym_leader_t{
	trainer_t trainer;
};
