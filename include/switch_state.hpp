#pragma once
#include "states.hpp"
#include "st-ctx.hpp"
#include "state_title.hpp"
#include "state_main_menu.hpp"
#include "state_wifi_connection.hpp"
#include "state_boy_or_girl.hpp"
#include "state_whats_your_name.hpp"
#include "state_juniper_letter.hpp"
#include "state_choose_a_starter.hpp"
#include "state_starter_selected.hpp"

inline void switch_state(st_ctx_t& ctx, STATE new_state, float delta = alpha_delta){
	float current_alpha = static_cast<float>(ctx.transition_alpha);
	if (ctx.current_state != new_state){
		current_alpha+=delta*GetFrameTime();
		if (ctx.transition_alpha >= 255.0f){
			current_alpha = 255;
			switch (ctx.current_state){
				case STATE::TITLE: TitleState::clear(); break;
				case STATE::MAIN_MENU: MainMenu::clear(); break;
				case STATE::BOY_OR_GIRL: BoyOrGirl::clear(); break;
				case STATE::WHATS_YOUR_NAME: WhatsYourName::clear(); break;
				case STATE::JUNIPER_LETTER: JuniperLetter::clear(); break;
				case STATE::CHOOSE_STARTER: ChooseAStarter::clear(); break;
				case STATE::STARTER_SELECTED: StarterSelected::clear(); break;
			}
			ctx.current_state = new_state;
			ctx.selected_option = 0;
			ctx.set_state_meta = false;
		}
	}
	ctx.transition_alpha = static_cast<unsigned char>(current_alpha);
}
