#include "battle.hpp"
#include "writer.hpp"
#include "names.hpp"
std::string battle_t::get_battle_intro(){
	return std::visit([&](auto&& arg) -> std::string{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, trainer_t>){
			return "CHALLANGED_BY_TRAINER";
			} else if constexpr (std::is_same_v<T, pokemon_t>){
			return "WILD_POKEMON_APPEARED";
			} else if constexpr (std::is_same_v<T, legendary_t>){
			return "LEGENDARY_POKEMON_APPEARED";
			} else if constexpr (std::is_same_v<T, gym_leader_t>){
			return "CHALLANGED_BY_GYM_LEADER";
			}
			}, foe.entry);
}

void battle_t::write(assets_t& assets, writer_t& writer){
	if (!writer.finished){
		if (txtbox_alpha < 255){
			txtbox_alpha+=5;
		}
	} else {
		txtbox_peak_alpha_timer++;
		if (txtbox_alpha > 0 && txtbox_peak_alpha_timer > 50){
			txtbox_alpha-=5;
		} else if (txtbox_alpha <= 0){
			txtbox_peak_alpha_timer = 0;
		}
	}
	draw_text_seq(assets, writer, 10, wnd_h - 200, 50, {255, 255, 255, txtbox_alpha}, 1, {150, 150, 150, txtbox_alpha});
}
