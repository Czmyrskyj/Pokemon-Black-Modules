#include "battle.hpp"
#include "names.hpp"
#include <print>
#include <format>
void battle_t::start(){
	auto [intro_str, foe_prefix, name] = std::visit([&](auto&& arg) -> std::tuple<std::string_view, std::string_view, std::string_view>{
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, trainer_t>){
			return {"You are challanged by {} {}!", arg.get_prefix().empty() ? "[PKMN] Trainer" : arg.get_prefix(), arg.get_name()};
		} else if constexpr (std::is_same_v<T, pokemon_t>){
			return {"{} {} appeared!", "Wild", names[arg.id]};
		} else if constexpr (std::is_same_v<T, legendary_t>){
			return {"{}{} appeared!", "", names[arg.pokemon.id]};
		} else if constexpr (std::is_same_v<T, gym_leader_t>){
			return {"You are challanged by {} {}!", "Leader", arg.trainer.get_name()};
		}
	}, foe.entry);
	std::println("{}", std::vformat(intro_str, std::make_format_args(foe_prefix, name)));
}
