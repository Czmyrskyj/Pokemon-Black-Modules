#include "st-ctx.hpp"
#include "dialog.hpp"
#include "formatter.hpp"
#include "pokedex.hpp"
#include "names.hpp"
#include "random.hpp"
#include "pokemon.hpp"

#include <print>

constexpr float shift_speed{30.0f};

choose_a_starter_t::choose_a_starter_t(st_ctx_t& ctx){
	dialog.init({
			"CHAR_CHEREN_1_1",
			"CHAR_BIANCA_1_1",
			"CHAR_CHEREN_1_2",
			"CHAR_BIANCA_1_2",
			});
	current_line = ctx.formatter.format(dialog.step(ctx.assets, writer, ctx.locale));
	writer.set_text(current_line);
	for (auto& o : yes_no){
		o = ctx.locale.get(Locale::TYPE::UI, o);
	}
}

void choose_a_starter_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	if (dialog.update(ctx.assets, writer) && writer.finished){
		current_line = ctx.formatter.format(dialog.step(ctx.assets, writer, ctx.locale));
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
					jump_factor = std::fabs(std::sinf(GetTime() * 20.0) * 150.0f);
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
		SetMusicVolume(ctx.current_music, volume);
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
					if (!handed_pokemon){
						switch (selected_starter){
							case 0:
								cheren_selected_starter = 1;
								bianca_selected_starter = 2;
								break;
							case 1:
								cheren_selected_starter = 2;
								bianca_selected_starter = 0;
								break;
							case 2:
								cheren_selected_starter = 0;
								bianca_selected_starter = 1;
								break;
						}
						auto players_entry = ctx.pokedex.get_pokedex_entry(starters[selected_starter]);
						if (!players_entry.has_value()){
							std::println(stderr, "fatal error: could not fetch pokedex entry for: {}", names[starters[selected_starter]]);
							std::abort();
						}
						pokemon_t players_chosen{
							players_entry.value(),
							std::string(names[starters[selected_starter]]),
							rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
							SEX::MALE,
						};
						auto cherens_entry = ctx.pokedex.get_pokedex_entry(starters[cheren_selected_starter]);
						if (!cherens_entry.has_value()){
							std::println(stderr, "fatal error: could not fetch pokedex entry for: {}", names[starters[cheren_selected_starter]]);
							std::abort();
						}
						pokemon_t cherens_chosen{
							cherens_entry.value(),
							"",
							rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
							SEX::MALE,
						};
						auto biancas_entry = ctx.pokedex.get_pokedex_entry(starters[bianca_selected_starter]);
						if (!biancas_entry.has_value()){
							std::println(stderr, "fatal error: could not fetch pokedex entry for: {}", names[starters[bianca_selected_starter]]);
							std::abort();
						}
						pokemon_t biancas_chosen{
							biancas_entry.value(),
							"",
							rnd::get_val<NATURE>(NATURE::BASHFUL, NATURE::TIMID),
							SEX::MALE,
						};
						ctx.player.push_pokemon(players_chosen);
						ctx.cheren.push_pokemon(cherens_chosen);
						ctx.bianca.push_pokemon(biancas_chosen);
						ctx.formatter.feed("[PLAYER-STARTER]", std::string(names[players_chosen.get_pokedex().id]));
						ctx.formatter.feed("[BIANCA-STARTER]", std::string(names[biancas_chosen.get_pokedex().id]));
						ctx.formatter.feed("[CHEREN-STARTER]", std::string(names[cherens_chosen.get_pokedex().id]));
						handed_pokemon = true;
					}
					ctx.current_music = {};
					ctx.switch_state(STATE::STARTER_SELECTED, 100);
					return;
				}
				if ((IsKeyPressed(KEY_X) || selected_opt == 1) && !handed_pokemon){
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
	writer.update();
}
void choose_a_starter_t::render(st_ctx_t& ctx){
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
					draw_text(ctx.assets, ctx.formatter.format(desc, starters[current_starter], ctx.locale.get(Locale::TYPE::UI, std::format("STARTER_TYPE_LABEL{}", current_starter))), 700, 900, 50, WHITE, {150, 150, 150, 255}, 0, 1);
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

void choose_a_starter_t::clear(assets_t& assets){
	for (const auto& i : {"starter-SNIVY.png", "starter-TEPIG.png", "starter-OSHAWOTT.png"}){
		assets.unload_texture(i);
	}
	assets.unload_track("Professor Juniper.ogg");
}
