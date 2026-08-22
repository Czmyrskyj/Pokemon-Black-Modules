#pragma once

#include "common.hpp"
#include "pokedex.hpp"
#include <string>

using party_t = std::array<pokemon_t, 6>;

enum class SEX : i8{
	NONE = -1,
	MALE = 0,
	FEMALE = 1,
};
class assets_t;
struct Vector2;

class trainer_t{
	std::string name = "nodata";
	std::string prefix = "";
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
	void set_name(std::string n){
		name = std::move(n);
	}
	void draw_overworld_preview(assets_t& assets, Vector2 pos);
};

struct gym_leader_t{
	trainer_t trainer;
};
