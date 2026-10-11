#pragma once
#include "st-ctx.hpp"

inline std::string get_music_name(st_ctx_t& ctx){
	auto& nm = ctx.bt_info.name;
	TRAINER_TYPE tt = ctx.bt_info.trainer_type;
	auto ms = std::visit([&](auto&& arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, trainer_t>){
					if (tt == TRAINER_TYPE::PKMN_TRAINER){
						if (nm == "Bianca" || nm == "Cheren") return "Battle! Cheren A Belle";
						if (nm == "N") return "Battle! N";
					} else {
						if (tt == TRAINER_TYPE::TEAM_PLASMA_GRUNT) return "Battle! Team Plasma";
						if (tt == TRAINER_TYPE::ELITE_FOUR) return "Battle! Elite Four";
						if (tt == TRAINER_TYPE::GYM_LEADER) return "Battle! Gym Leader";
						return "Battle! Trainer";
					}
				} else if constexpr (std::is_same_v<T, pokemon_t>){
					return "Battle! Wild Pokemon";
				} else if constexpr (std::is_same_v<T, legendary_t>){
					return "Battle! Legendary Pokemon";
				}
				return "";
			}, ctx.bt_info.foe);
	return std::format("{}.ogg", ms);
}

inline std::string get_victory_music_name(st_ctx_t& ctx){
	TRAINER_TYPE tt = ctx.bt_info.trainer_type;
	auto ms = std::visit([&](auto&& arg) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, trainer_t>){
			if (tt == TRAINER_TYPE::TEAM_PLASMA_GRUNT) return "Victory Against Team Plasma!";
			if (tt == TRAINER_TYPE::ELITE_FOUR) return "Victory Against Gym Leader!";
			if (tt == TRAINER_TYPE::GYM_LEADER) return "Victory Against Gym Leader!";
			return "Victory Against Trainer!";
			} else if constexpr (std::is_same_v<T, pokemon_t>){
			return "Victory Against Wild Pokemon.ogg";
			} else if constexpr (std::is_same_v<T, legendary_t>){
			return "Victory Against Wild Pokemon.ogg";
			}
			return "";
			}, ctx.bt_info.foe);
	return std::format("{}.ogg", ms);
}
