#pragma once

#include "common.hpp"
#include "pokedex.hpp"
#include <string>

using party_t = std::array<pokemon_t, 6>;

enum class SEX{
	NONE = -1,
	MALE = 0,
	FEMALE = 1,
};

class trainer_t{
	std::string name;
	std::string prefix;
	SEX sex = SEX::NONE;
	party_t party;
	public:
	trainer_t(std::string n, std::string p) : name(std::move(n)), prefix(std::move(p)) {}
	std::string_view get_name() const{
		return name;
	}
	std::string_view get_prefix() const{
		return prefix;
	}
	void set_sex(SEX s){
		sex = s;
	}
	SEX get_sex() const{
		return sex;
	}
};

struct gym_leader_t{
	trainer_t trainer;
};
