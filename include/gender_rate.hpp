#pragma once
#include "sex.hpp"
#include "pokedex.hpp"
#include "random.hpp"

inline SEX roll_sex(const pokedex_entry_t& entry){
	if (entry.gender_rate < 0) return SEX::NONE;
	return rnd::get_val<int>(0, 7) < entry.gender_rate ? SEX::FEMALE : SEX::MALE;
}
