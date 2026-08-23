#include "state_juniper_letter.hpp"
#include "st-ctx.hpp"
#include "font.hpp"
#include <raylib.h>

namespace JuniperLetter{
	static u16 current_txt_index = 0;
	static float txt_seq_timer = 0.0f;
	void update(st_ctx_t& ctx){
		 if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			 ctx.resume_state_alpha(ctx.transition_alpha);
		 }
		 if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			 ctx.current_music = ctx.assets.get_mus("Professor Juniper.mp3");
			 ctx.set_state_meta = true;
		 }
	}
	void render(st_ctx_t& ctx){
		DrawTextureEx(ctx.assets.get_tx("mincinno-mail.png"), {300, 100}, 0, 5, WHITE);
		draw_text_seq(ctx.assets, ctx.locale.get(Locale::TYPE::DIALOG, "JUNIPER_LETTER"), 400, 215, 38, current_txt_index, txt_seq_timer, BLACK, 0, BLANK, 0.04f);
	}
};
