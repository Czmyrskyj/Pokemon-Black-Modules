#include "battle.hpp"
#include "writer.hpp"
std::string battle_t::get_dialog_intro(){
	return std::visit([&](auto&& arg) -> std::string{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, trainer_t>){
			return "CHALLANGED_BY_FOE";
			} else if constexpr (std::is_same_v<T, pokemon_t>){
			return "WILD_POKEMON_APPEARED";
			} else if constexpr (std::is_same_v<T, legendary_t>){
			return "LEGENDARY_POKEMON_APPEARED";
			}
			return "";
			}, foe);
}

std::string battle_t::get_dialog_foe_sent_out(){
	return std::visit([&](auto&& arg) -> std::string{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, trainer_t>){
			return "FOE_SENT_OUT";
			} else if constexpr (std::is_same_v<T, pokemon_t>){
			return "GYM_LEADER_SENT_OUT";
			} else if constexpr (std::is_same_v<T, legendary_t>){
			return "";
			}
			}, foe);
}

void battle_t::write(assets_t& assets, writer_t& writer){
	if (!writer.finished){
		if (m_txtbox_alpha < 255){
			m_txtbox_alpha+=15;
		}
	} else {
		m_txtbox_peak_alpha_timer++;
		if (m_txtbox_alpha > 0 && m_txtbox_peak_alpha_timer > 50){
			m_txtbox_alpha-=15;
		} else if (m_txtbox_alpha <= 0){
			m_txtbox_peak_alpha_timer = 0;
		}
	}
	draw_text_seq(assets, writer, 10, wnd_h - 200, 50, {255, 255, 255, m_txtbox_alpha}, 1, {150, 150, 150, m_txtbox_alpha});
}

