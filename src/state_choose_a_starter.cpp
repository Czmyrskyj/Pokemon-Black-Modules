#include "state_choose_a_starter.hpp"
#include "st-ctx.hpp"
#include "dialog.hpp"
#include "formatter.hpp"
#include "pokedex.hpp"
#include "names.hpp"
#include "from_all_caps_to_capitalized.hpp"

#include <cmath>
#include <format>

constexpr float shift_speed{30.0f};

namespace ChooseAStarter{
	static dialog_t dialog;
	static writer_t writer;
	static bool initialized = false;
	static std::string current_line;
	static float screen_x = 0.0f;
	static u8 current_starter = 1;
	static i8 selected_starter = -1;
	static u8 current_opt = 0;
	static i8 selected_opt = -1;
	static bool played_cry = false;
	static float volume = 1.0f;
	static bool played_demo_03 = false;
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
	static constexpr std::array<std::string_view, 3> starter_type_labels{
		"Grass",
		"Fire",
		"Water",
	};
	static std::array<float, 3> starter_positions{
		400.0f,
		900.0f,
		1500.0f,
	};
	static float jump_factor = 0.0f;
	static std::array<std::string_view, 2> yes_no{
		"YES",
		"NO",
	};
	void update(st_ctx_t& ctx, trainer_t& trainer){
		if (!initialized){
			init(ctx);
			current_line = fmt::format(dialog.step(ctx.assets, writer, ctx.locale), &trainer);
			writer.set_text(current_line);
			for (auto& o : yes_no){
				o = ctx.locale.get(Locale::TYPE::UI, o);
			}
			initialized = true;
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			ctx.resume_state_alpha(ctx.transition_alpha);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		if (dialog.update(ctx.assets, writer) && writer.finished){
			current_line = fmt::format(dialog.step(ctx.assets, writer, ctx.locale), &trainer);
			writer.set_text(current_line);
		}
		if (selected_starter == -1 && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))){
			played_cry = false;
		}
		if (dialog.finished && dialog.hid_txtbox()){
			Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
			Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
			if (selected_starter == -1){
				selected_starter = ctx.control_selection(current_starter, 2, sel_sfx, dec_sfx, true);
			}
			for (usz i = 0; i < 3; i++){
				if (current_starter == i){
					if (starter_colors[i].r < 255) {
						starter_colors[i].a += 3;
						starter_colors[i].r += 15;
						starter_colors[i].g += 15;
						starter_colors[i].b += 15;
					}
					if (starter_scales[i] < 6.0f){
						starter_scales[i] += 0.25f;
						jump_factor = std::fabs(std::sinf(GetTime() * 20.0f) * 150.0f);
					} else if (!played_cry){
						PlaySound(ctx.assets.get_sfx(std::format("{}.wav", std::to_string(starters[i]))));
						played_cry = true;
					}
					if (starter_positions[i] < 1000.0f){
						starter_positions[i] += shift_speed;
					}
					if (starter_positions[i] > 1000.0f){
						starter_positions[i] -= shift_speed;
					}
				} else {
					starter_colors[i].a = 200;
					starter_colors[i].r = 45;
					starter_colors[i].g = 45;
					starter_colors[i].b = 45;

					starter_scales[i] = 2.0f;

					if (i == 0){
						starter_positions[i] = 400.0f;
					}
					if (i == 2){
						starter_positions[i] = 1500.0f;
					}
				}
			}
			if (selected_starter != -1){
				if (volume > 0.2f){
					volume -= 0.05f;
				} else {
					if (selected_opt == -1){
						selected_opt = ctx.control_selection(current_opt, 1, sel_sfx, dec_sfx);
					}
					if (selected_opt == 0){
						if (!played_demo_03){
							Sound demo_03 = ctx.assets.get_sfx("SEQ_SE_SDEMO_03.wav");
							PlaySound(demo_03);
							ctx.transition_color = WHITE;
							played_demo_03 = true;
						}
						if (volume > 0.0f){
							volume -= 0.05f;
						}
						ctx.switch_state(STATE::STARTER_SELECTED, 100);
					}
					if (IsKeyPressed(KEY_X) || selected_opt == 1){
						selected_starter = -1;
						current_starter = 1;
						current_opt = 0;
						selected_opt = -1;
					}
				}
			} else {
				if (volume < 1.0f){
					volume += 0.05f;
				}
			}
		}
		screen_x += 1.0f;
		SetMusicVolume(ctx.current_music, volume);
		writer.update();
	}
	void render(st_ctx_t& ctx){
		auto bg_strip = ctx.assets.get_tx("options-screen-strip.png");
		if (screen_x > bg_strip.width * 6.0f) screen_x = 0.0f;
		float alpha = std::sinf(GetTime()) * 40;
		for (int i = -1; i < 19; i++){
			float y_sin = std::sinf(i * bg_strip.width * 6.0f + screen_x * 0.05f) * 20.0f;
			DrawTextureEx(bg_strip, {i * bg_strip.width * 6.0f + screen_x, y_sin}, 0, 6.0f + (alpha * 0.05f), Color{255, 255, 255, static_cast<u8>(200 - alpha)});
			DrawTextureEx(bg_strip, {i * bg_strip.width * 6.0f - screen_x, y_sin}, 0, 6.0f + (alpha * 0.01f), Color{255, 255, 255, static_cast<u8>(100 - alpha)});
		}
		draw_text_seq(ctx.assets, writer, dialog.txtbox_x, dialog.txtbox_y, 50, WHITE, 1);
		dialog.draw_speaker(ctx.assets);
		if (dialog.finished && dialog.hid_txtbox()){
			auto draw_starter = [&](usz i){
				float current_jump = (current_starter == i) ? jump_factor : 0.0f;
				DrawTexturePro(ctx.assets.get_tx(std::format("starter-{}.png", names[starters[i]])), 
						{0, 0, 96, 96}, {starter_positions[i], 300.0f - current_jump, 96 * starter_scales[i], 96 * starter_scales[i]},
						{48 * starter_scales[i], 0}, 0.0f, starter_colors[i]);

				if (starter_scales[i] == 6.0f){
					std::string_view desc = ctx.locale.get(Locale::TYPE::UI, "SR_PKMN_DESC");
					if (selected_starter == -1){
						draw_text(ctx.assets, fmt::format(desc, nullptr, starters[i], starter_type_labels[i]), 700, 900, 50, WHITE, {150, 150, 150, 255}, 0, 1);
					} else {
						draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "IS_THIS_PKMN_U_WANT"), 700, 900, 50, WHITE, {150, 150, 150, 255}, 0, 1);
					}
				}
			};
			for (usz i = 0; i < 3; i++){
				if (i != current_starter){
					draw_starter(i);
				}
			}
			if (current_starter < 3){
				draw_starter(current_starter);
			}
		}
		if (selected_starter != -1){
			for (usz i = 0; i < 2; i++){
				Texture2D button = ctx.assets.get_tx(current_opt == i ? "opt-selected.png" : "opt.png");
				DrawTextureEx(button, {wnd_w - 300, wnd_h - 400 + i * (button.height * 4.0f)}, 0, 4.0f, WHITE);
				draw_text(ctx.assets, yes_no[i], wnd_w - 300 + 30.0f, wnd_h - 400 + i * (button.height * 4.0f) + 15.0f, 60, WHITE);
			}
		}
	}
};
