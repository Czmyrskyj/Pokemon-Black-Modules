#include "state_choose_a_starter.hpp"
#include "st-ctx.hpp"
#include "dialog.hpp"
#include "formatter.hpp"
#include "pokedex.hpp"
#include "names.hpp"
#include "from_all_caps_to_capitalized.hpp"

#include <cmath>
#include <format>

namespace ChooseAStarter{
	static dialog_t dialog;
	static writer_t writer;
	static bool initialized = false;
	static std::string current_line;
	static u8 current_option = 1;
	void init(st_ctx_t& ctx){
		dialog.init({
			"CHAR_CHEREN_1_1",
			"CHAR_BIANCA_1_1",
			"CHAR_CHEREN_1_2",
			"CHAR_BIANCA_1_2",
			});
		ctx.current_music = ctx.assets.get_mus("Professor Juniper.mp3");
	}
	static std::array<POKEDEX, 3> starters{
		SNIVY,
		TEPIG,
		OSHAWOTT,
	};
	static std::array<float, 3> starter_scales{
		2.0f,
		2.0f,
		2.0f,
	};
	static std::array<Color, 3> starter_colors{
		Color{45, 45, 45, 200},
		Color{45, 45, 45, 200},
		Color{45, 45, 45, 200},
	};
	static std::array<std::string_view, 3> starter_type_labels{
		"Grass",
		"Fire",
		"Water",
	};
	static float jump_factor = 0.0f;
	void update(st_ctx_t& ctx, Locale::locale_t& locale, trainer_t& trainer){
		if (!initialized){
			init(ctx);
			current_line = fmt::format(dialog.step(ctx.assets, writer, locale), &trainer);
			writer.set_text(current_line);
			initialized = true;
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			ctx.resume_state_alpha(ctx.transition_alpha);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		if (dialog.update(ctx.assets, writer) && writer.finished){
			current_line = fmt::format(dialog.step(ctx.assets, writer, locale), &trainer);
			writer.set_text(current_line);
		}
		if (dialog.finished && dialog.hid_txtbox()){
			Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
			Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
			ctx.control_selection(current_option, 2, sel_sfx, dec_sfx, true);
		}
		writer.update();
	}
	void render(st_ctx_t& ctx){
		draw_text_seq(ctx.assets, writer, dialog.txtbox_x, dialog.txtbox_y, 50, WHITE, 1);
		dialog.draw_speaker(ctx.assets);
		if (dialog.finished && dialog.hid_txtbox()){
			for (usz i = 0; i < 3; i++){
				if (current_option == i){
					if (starter_colors[i].r < 255) {
						starter_colors[i].a += 3;
						starter_colors[i].r += 15;
						starter_colors[i].g += 15;
						starter_colors[i].b += 15;
					}
					if (starter_scales[i] < 6.0f){
						starter_scales[i] += 0.25f;
						jump_factor = std::fabs(std::sinf(GetTime() * 20.0f) * 150.0f);
					}
				} else {
					if (starter_colors[i].r > 45) {
						starter_colors[i].a -= 3;
						starter_colors[i].r -= 15;
						starter_colors[i].g -= 15;
						starter_colors[i].b -= 15;
					}
					if (starter_scales[i] > 2.0f){
						starter_scales[i] -= 0.25f;
					}
					jump_factor = 0.0f;
				}
				DrawTexturePro(ctx.assets.get_tx(std::format("starter-{}.png", names[starters[i]])), 
						{0, 0, 96, 96}, {300 + i * 600.0f, 300.0f - jump_factor, 96 * starter_scales[i], 96 * starter_scales[i]},
						{48 * starter_scales[i], 0}, 0.0f, starter_colors[i]);

				if (starter_scales[i] == 6.0f){
					draw_text(ctx.assets, std::format("The {}-type Pokémon\n\t{}", starter_type_labels[i], new_all_caps_to_capitalized(names[starters[i]])), 600, 600, 50, WHITE, {150, 150, 150, 255}, 0, 1);
				}
			}
		}
	}
};
