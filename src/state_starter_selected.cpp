#include "st-ctx.hpp"
#include "formatter.hpp"
#include "trainer.hpp"
#include "writer.hpp"
#include "dialog.hpp"

#include <cmath>

starter_selected_t::starter_selected_t(st_ctx_t& ctx){
	dialog.top = true;
	ctx.bg_color = WHITE;
	formatted_string = fmt::format(ctx.locale.get(Locale::TYPE::UI, "RECEIVED_POKEMON"), &ctx.player, ctx.player.get_party().data()[0].pokedex.id);
	dialog.init({
			"CHAR_BIANCA_2_1",
			"CHAR_CHEREN_2_2",
			"CHAR_CHEREN_2_3",
			});
}
void starter_selected_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
		ctx.halt_music = true;
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	Sound received_starter_sound = ctx.assets.get_sfx("received-starter.wav");
	if (!played_received_starter){
		PlaySound(received_starter_sound);
		played_received_starter = true;
	} else if (!IsSoundPlaying(received_starter_sound)){
		if (!init_dialog){
			current_line = fmt::format(dialog.step(ctx.assets, writer, ctx.locale), nullptr, ctx.cheren.get_party().data()[0].pokedex.id);
			writer.set_text(current_line);
			init_dialog = true;
		}
		if (dialog.update(ctx.assets, writer) && writer.finished){
			current_line = fmt::format(dialog.step(ctx.assets, writer, ctx.locale), nullptr, ctx.cheren.get_party().data()[0].pokedex.id);
			writer.set_text(current_line);
		}
	}
	if (dialog.finished){
		ctx.transition_color = BLACK;
		ctx.switch_state(STATE::WHICH_SEASON_AND_REGION);
	}
	writer.update();
}
void starter_selected_t::render(st_ctx_t& ctx){
	for (usz i = 0; i < formatted_string.size(); i++){
		const char c[2]{formatted_string[i], '\0'};
		draw_text(ctx.assets, c, 10 + i * 30, std::sinf((10 + i * 30) + (GetTime() * 2.0)) * 5.0f + 1000, 50, BLACK);
	}
	draw_text_seq(ctx.assets, writer, dialog.txtbox_x, dialog.txtbox_y, 50, WHITE, 1);
	dialog.draw_speaker(ctx.assets);
}
