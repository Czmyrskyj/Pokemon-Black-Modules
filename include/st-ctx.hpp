#pragma once
#include "common.hpp"
#include "assets.hpp"
#include "states.hpp"
#include "trainer.hpp"
#include "locale.hpp"
constexpr float alpha_delta{300.0f};
struct st_ctx_t{
	assets_t& assets;
	trainer_t& player;
	Locale::locale_t& locale;
	STATE current_state;
	u8 transition_alpha = 255;
	bool set_state_meta{false};
	bool halt_music{false};
	Music current_music{};
	u8 selected_option = 0;
	void switch_state(STATE new_state, unsigned char& alpha, float delta = alpha_delta){
		float current_alpha = static_cast<float>(alpha);
		if (current_state != new_state){
			current_alpha+=delta*GetFrameTime();
			if (alpha >= 255.0f){
				current_alpha = 255;
				current_state = new_state;
				selected_option = 0;
				set_state_meta = false;
			}
		}
		alpha = static_cast<unsigned char>(current_alpha);
	}
	void resume_state_alpha(unsigned char& alpha){
		float current_alpha = static_cast<float>(alpha);
		if (current_alpha > 5.0f){
			current_alpha -= alpha_delta*GetFrameTime();
		} else {
			current_alpha = 0.0f;
		}
		alpha = static_cast<unsigned char>(current_alpha);
	}
	i8 control_selection(u8& opt, u8 min, u8 max, const Sound& sel, const Sound& dec, bool horizontal = false){
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
};
