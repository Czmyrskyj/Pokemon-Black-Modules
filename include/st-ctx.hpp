#pragma once
#include "common.hpp"
#include "assets.hpp"
#include "trainer.hpp"
#include "locale.hpp"

struct st_ctx_t;

#include "states.hpp"

constexpr float alpha_delta{300.0f};
struct st_ctx_t{
	assets_t& assets;
	trainer_t player{"nodata", ""};
	trainer_t cheren{"Cheren", "[PKMN_SIGN]"};
	trainer_t bianca{"Bianca", "[PKMN_SIGN]"};
	Locale::locale_t& locale;
	pokedex_t pokedex;
	Time::time_t& time;
	STATE current_state = STATE::TITLE;
	active_state_t state_data = title_state_t{};
	Color bg_color = BLACK;
	Color transition_color = BLACK;
	u8 transition_alpha = 255;
	bool set_state_meta{false};
	bool halt_music{false};
	Music current_music{};
	u8 selected_option = 0;
	void resume_state_alpha(unsigned char& alpha){
		float current_alpha = static_cast<float>(alpha);
		if (current_alpha > 5.0f){
			current_alpha -= alpha_delta*GetFrameTime();
		} else {
			current_alpha = 0.0f;
		}
		alpha = static_cast<unsigned char>(current_alpha);
	}
	i8 control_selection(u8& opt, u8 max, const Sound& sel, const Sound& dec, bool horizontal = false){
		if (IsKeyPressed(!horizontal ? KEY_DOWN : KEY_RIGHT)){
			opt++;
			if (opt > max) opt = 0;
			PlaySound(sel);
		} else if (IsKeyPressed(!horizontal ? KEY_UP : KEY_LEFT)){
			if (opt > 0){
				opt--;
			} else {
				opt = max;
			}
			PlaySound(sel);
		}
		if (IsKeyPressed(KEY_Z)){
			PlaySound(dec);
			return opt;
		}
		return -1;
	}
	void switch_data(STATE new_state){
		switch (new_state){
			case STATE::TITLE:
				state_data = title_state_t{};
				break;
			case STATE::MAIN_MENU:
				state_data.emplace<main_menu_t>(locale);
				break;
			case STATE::BOY_OR_GIRL:
				state_data.emplace<boy_or_girl_t>(locale);
				break;
			case STATE::WIFI_CONNECTION:
				state_data = wifi_connection_t{};
				break;
			case STATE::WHATS_YOUR_NAME:
				state_data.emplace<whats_your_name_t>(locale);
				break;
			case STATE::JUNIPER_LETTER:
				state_data.emplace<juniper_letter_t>(locale, player);
				break;
			case STATE::CHOOSE_STARTER:
				state_data.emplace<choose_a_starter_t>(assets, locale, player);
				break;
			case STATE::STARTER_SELECTED:
				state_data.emplace<starter_selected_t>(bg_color, locale, player);
				break;
			case STATE::WHICH_SEASON_AND_REGION:
				state_data.emplace<which_season_and_region_t>(bg_color, locale, time);
				break;
			default:
				break;
		}
	}

	void switch_state(STATE new_state, float delta = alpha_delta){
		float current_alpha = static_cast<float>(transition_alpha);
		if (current_state != new_state){
			current_alpha+=delta*GetFrameTime();
			if (transition_alpha >= 255.0f){
				current_alpha = 255;
				current_state = new_state;
				switch_data(new_state);
				selected_option = 0;
				set_state_meta = false;
			}
		}
		transition_alpha = static_cast<unsigned char>(current_alpha);
	}
};
