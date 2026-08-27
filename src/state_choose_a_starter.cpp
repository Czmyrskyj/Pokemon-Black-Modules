#include "state_choose_a_starter.hpp"
#include "st-ctx.hpp"
#include "dialog.hpp"
#include "formatter.hpp"
#include "pokedex.hpp"
#include "names.hpp"
#include "random.hpp"
#include "pokemon.hpp"
#include "switch_state.hpp"

#include <cmath>
#include <format>
#include <print>
#include <memory>

constexpr float shift_speed{30.0f};

namespace ChooseAStarter{
	struct state_meta_t{
		dialog_t dialog;
		writer_t writer;
		std::string current_line;
		float screen_x = 0.0f;
		u8 current_starter = 1;
		i8 selected_starter = -1;
		u8 current_opt = 0;
		i8 selected_opt = -1;
		bool played_cry = false;
		float volume = 1.0f;
		bool played_demo_03 = false;
		bool handed_pokemon = false;
		i8 cheren_selected_starter = -1;
		i8 bianca_selected_starter = -1;
		std::array<POKEDEX, 3> starters{
			SNIVY,
				TEPIG,
				OSHAWOTT,
		};
		std::array<float, 3> starter_scales{
			2.0f,
				2.0f,
				2.0f,
		};
		std::array<Color, 3> starter_colors{
			Color{45, 45, 45, 200},
				Color{45, 45, 45, 200},
				Color{45, 45, 45, 200},
		};
		const std::array<std::string_view, 3> starter_type_labels{
			"Grass",
				"Fire",
				"Water",
		};
		std::array<float, 3> starter_positions{
			400.0f,
				900.0f,
				1500.0f,
		};
		float jump_factor = 0.0f;
		std::array<std::string_view, 2> yes_no{
			"YES",
				"NO",
		};
	};
	static std::unique_ptr<state_meta_t> meta;
	void init(st_ctx_t& ctx){
		if (!meta){
			meta = std::make_unique<state_meta_t>();
		}
		meta->dialog.init({
				"CHAR_CHEREN_1_1",
				"CHAR_BIANCA_1_1",
				"CHAR_CHEREN_1_2",
				"CHAR_BIANCA_1_2",
				});
		ctx.current_music = ctx.assets.get_mus("Professor Juniper.mp3");
	}
	void update(st_ctx_t& ctx, trainer_t& player, trainer_t& cheren, trainer_t& bianca){
		if (!meta){
			init(ctx);
			meta->current_line = fmt::format(meta->dialog.step(ctx.assets, meta->writer, ctx.locale), &player);
			meta->writer.set_text(meta->current_line);
			for (auto& o : meta->yes_no){
				o = ctx.locale.get(Locale::TYPE::UI, o);
			}
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			ctx.resume_state_alpha(ctx.transition_alpha);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		if (meta->dialog.update(ctx.assets, meta->writer) && meta->writer.finished){
			meta->current_line = fmt::format(meta->dialog.step(ctx.assets, meta->writer, ctx.locale), &player);
			meta->writer.set_text(meta->current_line);
		}
		if (meta->selected_starter == -1 && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))){
			meta->played_cry = false;
		}
		if (meta->dialog.finished && meta->dialog.hid_txtbox()){
			Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
			Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
			if (meta->selected_starter == -1){
				meta->selected_starter = ctx.control_selection(meta->current_starter, 2, sel_sfx, dec_sfx, true);
			}
			for (usz i = 0; i < 3; i++){
				if (meta->current_starter == i){
					if (meta->starter_colors[i].r < 255) {
						meta->starter_colors[i].a += 3;
						meta->starter_colors[i].r += 15;
						meta->starter_colors[i].g += 15;
						meta->starter_colors[i].b += 15;
					}
					if (meta->starter_scales[i] < 6.0f){
						meta->starter_scales[i] += 0.25f;
						meta->jump_factor = std::fabs(std::sinf(GetTime() * 20.0f) * 150.0f);
					} else if (!meta->played_cry){
						PlaySound(ctx.assets.get_sfx(std::format("{}.wav", std::to_string(meta->starters[i]))));
						meta->played_cry = true;
					}
					if (meta->starter_positions[i] < 1000.0f){
						meta->starter_positions[i] += shift_speed;
					}
					if (meta->starter_positions[i] > 1000.0f){
						meta->starter_positions[i] -= shift_speed;
					}
				} else {
					meta->starter_colors[i].a = 200;
					meta->starter_colors[i].r = 45;
					meta->starter_colors[i].g = 45;
					meta->starter_colors[i].b = 45;

					meta->starter_scales[i] = 2.0f;

					if (i == 0){
						meta->starter_positions[i] = 400.0f;
					}
					if (i == 2){
						meta->starter_positions[i] = 1500.0f;
					}
				}
			}
			if (meta->selected_starter != -1){
				if (meta->volume > 0.2f){
					meta->volume -= 0.05f;
				} else {
					if (meta->selected_opt == -1){
						meta->selected_opt = ctx.control_selection(meta->current_opt, 1, sel_sfx, dec_sfx);
					}
					if (meta->selected_opt == 0){
						if (!meta->played_demo_03){
							Sound demo_03 = ctx.assets.get_sfx("SEQ_SE_SDEMO_03.wav");
							PlaySound(demo_03);
							ctx.transition_color = WHITE;
							meta->played_demo_03 = true;
						}
						if (meta->volume > 0.0f){
							meta->volume -= 0.05f;
						}
						if (!meta->handed_pokemon){
							switch (meta->selected_starter){
								case 0:
									meta->cheren_selected_starter = 1;
									meta->bianca_selected_starter = 2;
									break;
								case 1:
									meta->cheren_selected_starter = 2;
									meta->bianca_selected_starter = 0;
									break;
								case 2:
									meta->cheren_selected_starter = 0;
									meta->bianca_selected_starter = 1;
									break;
							}
							auto players_entry = ctx.pokedex.get_pokedex_entry(meta->starters[meta->selected_starter]);
							if (!players_entry.has_value()){
								std::println(stderr, "fatal error: could not fetch pokedex entry for: {}", names[meta->starters[meta->selected_starter]]);
								std::abort();
							}
							pokemon_t players_choosen{
								.pokedex = players_entry.value(),
									.nickname = std::string(names[meta->starters[meta->selected_starter]]),
									.nature = rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
							};
							auto cherens_entry = ctx.pokedex.get_pokedex_entry(meta->starters[meta->cheren_selected_starter]);
							if (!cherens_entry.has_value()){
								std::println(stderr, "fatal error: could not fetch pokedex entry for: {}", names[meta->starters[meta->cheren_selected_starter]]);
								std::abort();
							}
							pokemon_t cherens_choosen{
								.pokedex = cherens_entry.value(),
									.nickname = "",
									.nature = rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
							};
							auto biancas_entry = ctx.pokedex.get_pokedex_entry(meta->starters[meta->bianca_selected_starter]);
							if (!biancas_entry.has_value()){
								std::println(stderr, "fatal error: could not fetch pokedex entry for: {}", names[meta->starters[meta->bianca_selected_starter]]);
								std::abort();
							}
							pokemon_t biancas_choosen{
								.pokedex = biancas_entry.value(),
									.nickname = "",
									.nature = rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
							};
							player.push_pokemon(players_choosen);
							cheren.push_pokemon(cherens_choosen);
							bianca.push_pokemon(biancas_choosen);
							meta->handed_pokemon = true;
						}
						switch_state(ctx, STATE::STARTER_SELECTED, 100);
						if (!meta){
							return;
						}
					}
					if ((IsKeyPressed(KEY_X) || meta->selected_opt == 1) && !meta->handed_pokemon){
						meta->selected_starter = -1;
						meta->current_starter = 1;
						meta->current_opt = 0;
						meta->selected_opt = -1;
					}
				}
			} else {
				if (meta->volume < 1.0f){
					meta->volume += 0.05f;
				}
			}
		}
		meta->screen_x += 1.0f;
		SetMusicVolume(ctx.current_music, meta->volume);
		meta->writer.update();
	}
	void render(st_ctx_t& ctx){
		if (!meta){
			return;
		}
		auto bg_strip = ctx.assets.get_tx("options-screen-strip.png");
		if (meta->screen_x > bg_strip.width * 6.0f) meta->screen_x = 0.0f;
		float alpha = std::sinf(GetTime()) * 40;
		for (int i = -1; i < 19; i++){
			float y_sin = std::sinf(i * bg_strip.width * 6.0f + meta->screen_x * 0.05f) * 20.0f;
			DrawTextureEx(bg_strip, {i * bg_strip.width * 6.0f + meta->screen_x, y_sin}, 0, 6.0f + (alpha * 0.05f), Color{255, 255, 255, static_cast<u8>(200 - alpha)});
			DrawTextureEx(bg_strip, {i * bg_strip.width * 6.0f - meta->screen_x, y_sin}, 0, 6.0f + (alpha * 0.01f), Color{255, 255, 255, static_cast<u8>(100 - alpha)});
		}
		draw_text_seq(ctx.assets, meta->writer, meta->dialog.txtbox_x, meta->dialog.txtbox_y, 50, WHITE, 1);
		meta->dialog.draw_speaker(ctx.assets);
		if (meta->dialog.finished && meta->dialog.hid_txtbox()){
			auto draw_starter = [&](usz i){
				float current_jump = (meta->current_starter == i) ? meta->jump_factor : 0.0f;
				DrawTexturePro(ctx.assets.get_tx(std::format("starter-{}.png", names[meta->starters[i]])), 
						{0, 0, 96, 96}, {meta->starter_positions[i], 300.0f - current_jump, 96 * meta->starter_scales[i], 96 * meta->starter_scales[i]},
						{48 * meta->starter_scales[i], 0}, 0.0f, meta->starter_colors[i]);

				if (meta->starter_scales[i] == 6.0f){
					std::string_view desc = ctx.locale.get(Locale::TYPE::UI, "SR_PKMN_DESC");
					if (meta->selected_starter == -1){
						draw_text(ctx.assets, fmt::format(desc, nullptr, meta->starters[i], meta->starter_type_labels[i]), 700, 900, 50, WHITE, {150, 150, 150, 255}, 0, 1);
					} else {
						draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "IS_THIS_PKMN_U_WANT"), 700, 900, 50, WHITE, {150, 150, 150, 255}, 0, 1);
					}
				}
			};
			for (usz i = 0; i < 3; i++){
				if (i != meta->current_starter){
					draw_starter(i);
				}
			}
			if (meta->current_starter < 3){
				draw_starter(meta->current_starter);
			}
		}
		if (meta->selected_starter != -1){
			for (usz i = 0; i < 2; i++){
				Texture2D button = ctx.assets.get_tx(meta->current_opt == i ? "opt-selected.png" : "opt.png");
				DrawTextureEx(button, {wnd_w - 300, wnd_h - 400 + i * (button.height * 4.0f)}, 0, 4.0f, WHITE);
				draw_text(ctx.assets, meta->yes_no[i], wnd_w - 300 + 30.0f, wnd_h - 400 + i * (button.height * 4.0f) + 15.0f, 60, WHITE);
			}
		}
	}
	void clear(){
		meta.reset();
	}
};
