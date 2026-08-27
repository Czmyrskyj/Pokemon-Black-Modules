#include "state_boy_or_girl.hpp"
#include "st-ctx.hpp"
#include "writer.hpp"
#include "formatter.hpp"
#include "switch_state.hpp"
#include <memory>
#include <cmath>

namespace BoyOrGirl{

	struct state_meta_t{
		const std::array<int, 4> frame_order = {0, 1, 0, 2};
		float screen_x = 0.0f;
		u8 char_frame_count_timer = 0;
		u8 char_frame_count = 0;
		i8 opt = -1;
		usz char_frame = 0;
		writer_t writer;
		std::string formatted_prompt;
	};

	static std::unique_ptr<state_meta_t> meta;

	void init(st_ctx_t& ctx){
		if (!meta){
			meta = std::make_unique<state_meta_t>();
		}
		meta->formatted_prompt = fmt::format(ctx.locale.get(Locale::TYPE::DIALOG, "PROMPT_SEX"));
		meta->writer.set_text(meta->formatted_prompt, 0.03f);
	}
	void update(st_ctx_t& ctx){
		if (!meta){
			init(ctx);
		}
		meta->writer.update();
		if (!ctx.set_state_meta && ctx.transition_alpha > 0){
			ctx.resume_state_alpha(ctx.transition_alpha);
			ctx.current_music = ctx.assets.get_mus("Abandoned Lot of Dreams.mp3");
			ctx.halt_music = false;
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		meta->char_frame_count_timer++;
		if (meta->char_frame_count_timer >= 10){
			meta->char_frame_count++;
			meta->char_frame_count_timer = 0;
		}
		meta->char_frame_count %= 4;
		meta->char_frame = meta->frame_order[meta->char_frame_count];
		meta->screen_x -= 1.0f;
		Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
		Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");

		if (meta->writer.finished && ctx.player.get_sex() == SEX::NONE){
			meta->opt = ctx.control_selection(ctx.selected_option, 1, sel_sfx, dec_sfx, true);
		}

		if (meta->opt != -1){
			if (ctx.player.get_sex() == SEX::NONE){
				ctx.player.set_sex(static_cast<SEX>(meta->opt));
			} else {
				switch_state(ctx, STATE::WHATS_YOUR_NAME);
			}
		}
	}

	void render(st_ctx_t& ctx){
		if (!meta){
			return;
		}
		auto bg_strip = ctx.assets.get_tx("options-screen-strip.png");
		if (meta->screen_x < -bg_strip.width * 6.0f) meta->screen_x = 0.0f;
		float alpha = std::sinf(GetTime()) * 40;
		for (usz i = 0; i < 20; i++){
			DrawTextureEx(bg_strip, {0 + i * bg_strip.width * 6.0f + meta->screen_x, 0}, 0, 6.0f + (alpha * 0.03f), Color{255, 255, 255, static_cast<u8>(200 - alpha)});
		}
		draw_text_seq(ctx.assets, meta->writer, 10, 100, 50, WHITE, 1);
		if (meta->writer.finished){

			auto hilbert = ctx.assets.get_tx("hilbert.png");
			auto hilda = ctx.assets.get_tx("hilda.png");

			Rectangle hilbert_src = {
				ctx.selected_option == 0 && ctx.player.get_sex() == SEX::NONE ? meta->char_frame * 32.0f : 0,
				0,
				32,
				static_cast<float>(hilbert.height)

			};
			Rectangle hilda_src = {
				ctx.selected_option == 1 && ctx.player.get_sex() == SEX::NONE ? meta->char_frame * 32.0f : 0,
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
	void clear(){
		meta.reset();
	}
}
