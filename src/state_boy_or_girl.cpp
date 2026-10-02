#include "st-ctx.hpp"
#include "formatter.hpp"
#include <cmath>

boy_or_girl_t::boy_or_girl_t(st_ctx_t& ctx){
	formatted_prompt = ctx.formatter.format(ctx.locale.get(Locale::TYPE::DIALOG, "PROMPT_SEX"));
	writer.set_text(formatted_prompt, 0.03f);
}

void boy_or_girl_t::update(st_ctx_t& ctx){
	writer.update();
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
		ctx.set_music("Abandoned Lot of Dreams.ogg");
		ctx.halt_music = false;
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	char_frame_count_timer++;
	if (char_frame_count_timer >= 10){
		char_frame_count++;
		char_frame_count_timer = 0;
	}
	char_frame_count %= 4;
	char_frame = static_cast<usz>(frame_order[char_frame_count]);
	screen_x -= 1.0f;
	Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
	Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");

	if (writer.finished && ctx.player.sex() == SEX::NONE){
		opt = ctx.control_selection(ctx.selected_option, 1, sel_sfx, dec_sfx, true);
	}

	if (opt != -1){
		if (ctx.player.sex() == SEX::NONE){
			ctx.player.set_sex(static_cast<SEX>(opt));
		} else {
			ctx.switch_state(STATE::WHATS_YOUR_NAME);
		}
	}
}

void boy_or_girl_t::render(st_ctx_t& ctx){
	auto bg_strip = ctx.assets.get_tx("options-screen-strip.png");
	if (screen_x < -bg_strip.width * 6.0f) screen_x = 0.0f;
	float alpha = std::sinf(GetTime()) * 40;
	for (usz i = 0; i < 20; i++){
		DrawTextureEx(bg_strip, {0 + i * bg_strip.width * 6.0f + screen_x, 0}, 0, 6.0f + (alpha * 0.03f), Color{255, 255, 255, static_cast<u8>(200 - alpha)});
	}
	draw_text_seq(ctx.assets, writer, 10, 100, 50, WHITE, 1);
	if (writer.finished){

		auto hilbert = ctx.assets.get_tx("hilbert.png");
		auto hilda = ctx.assets.get_tx("hilda.png");

		Rectangle hilbert_src = {
			ctx.selected_option == 0 && ctx.player.sex() == SEX::NONE ? char_frame * 32.0f : 0,
			0,
			32,
			static_cast<float>(hilbert.height)

		};
		Rectangle hilda_src = {
			ctx.selected_option == 1 && ctx.player.sex() == SEX::NONE ? char_frame * 32.0f : 0,
			0,
			32,
			static_cast<float>(hilda.height)

		};
		Rectangle hilbert_dest = {
			350,
			500,
			ctx.selected_option == 0 ? 256.0f : 192.0f,
			ctx.selected_option == 0 ? 216.0f : 162.0f,
		};
		Rectangle hilda_dest = {
			1250,
			500,
			ctx.selected_option == 1 ? 256.0f : 192.0f,
			ctx.selected_option == 1 ? 216.0f : 162.0f,
		};

		DrawTexturePro(hilbert, hilbert_src, hilbert_dest, {0, 0}, 0, ctx.selected_option == 0 ? WHITE : Color{200, 200, 200, 100});
		DrawTexturePro(hilda, hilda_src, hilda_dest, {0, 0}, 0, ctx.selected_option == 1 ? WHITE : Color{200, 200, 200, 100});
	}
}
