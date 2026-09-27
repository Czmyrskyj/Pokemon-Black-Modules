#include "states.hpp"
#include "st-ctx.hpp"
#include "names.hpp"

#include <algorithm>
#include <format>
#include <cmath>

constexpr float pokemon_scale_dest{1.0f};
constexpr float pokemon_y_dest{0.1f};


Rectangle full_tx(const Texture2D& tx){
	return Rectangle{0, 0, static_cast<float>(tx.width), static_cast<float>(tx.height)};
}

void draw_pokename(assets_t& assets, std::string_view text, float x, float y){
	const auto& pokename_font = assets.get_tx("pokename-font.png");
	float y_offset = 0.0f;
	float x_offset = 0.0f;
	float current_x = x;
	for (char c : text){
		current_x += 22.0f;
		if (c >= 'A' && c <= 'Z'){
			y_offset = 10;
			x_offset = (c - 'A') * 6.0f;
		} else if (c >= 'a' && c <= 'z'){
			y_offset = 20;
			x_offset = (c - 'a') * 6.0f;
		} else {
			y_offset = 0;
			x_offset = (c - '0') * 6.0f;
		}
		DrawTexturePro(pokename_font, {x_offset, y_offset, 5, 9}, {current_x, y, 22, 40}, {0, 0}, 0, WHITE);
	}
}

battle_state_t::battle_state_t(st_ctx_t& ctx) : battle(ctx.player, ctx.bt_info.foe){
	ctx.camera.camera = {
		.position = {0.4f, 0.6f, -2.0f},
		.target = {-0.3f, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
	battle.init_dialog({
			battle.get_dialog_intro(),
			battle.get_dialog_foe_sent_out(),
			"TRAINER_SENT_OUT",
			});
	ctx.formatter.feed("[FOE_TYPE]", ctx.formatter.format((!ctx.bt_info.prefix.empty()) ? ctx.bt_info.prefix : "[PKMN_SIGN] Trainer"));
	ctx.formatter.feed("[FOE]", ctx.bt_info.name);
	current_line = ctx.formatter.format(battle.step_dialog(writer, ctx.locale));
	writer.set_text(current_line);
}

void battle_state_t::update(st_ctx_t& ctx){
	ctx.camera.set_route({
			[&]() -> Vector3{
			switch (battle.camera_pos()){
			case BATTLE_CAMERA_POS::DEFAULT: return {0.2f, 0.6f, 2.0f};
			case BATTLE_CAMERA_POS::TRAINER: return {0.3f, 0.6f, 4.0f};
			case BATTLE_CAMERA_POS::ALLY: return {0.0f, 0.5f, 3.0f};
			default: return {0.2f, 0.6f, 2.0f};
			}
			}()
			});
	if (!ctx.set_state_meta && ctx.transition_alpha > 0 && next_menu_state == BATTLE_MENU_STATE::NONE){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	
	auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();

	Sound absent = ctx.assets.get_sfx("SEQ_SE_TB_KARA.wav");
	Sound present = ctx.assets.get_sfx("SEQ_SE_TB_KON.wav");

	Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
	Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
	
	bg_rotation = std::sinf(GetTime()) * 15.0f;

	auto& p_party = ctx.player.get_party();

	if (next_menu_state != BATTLE_MENU_STATE::NONE){
		float current_alpha = static_cast<float>(ctx.transition_alpha);
		if (battle.menu_state() != next_menu_state){
			current_alpha+=300.0f*GetFrameTime();
			if (current_alpha >= 255.0f){
				current_alpha = 255.0f;
				for (usz i = 0; i < p_party.size(); i++){
					ctx.player.get_party()[i].block_frames(false);
				}
				battle.set_menu_state(next_menu_state);
				opt = 0;
				sel_opt = -1;
			}
		} else {
			current_alpha-=300.0f*GetFrameTime();
			if (current_alpha <= 0.0f){
				current_alpha = 0.0f;
				next_menu_state = BATTLE_MENU_STATE::NONE;
			}
		}
		ctx.transition_alpha = static_cast<unsigned char>(current_alpha);
	}

	switch (battle.phase()){
		case BATTLE_PHASE::INTRO:
			{
				if (ctx.set_state_meta){
					if (battle.foe_accent_color() < 255){
						battle.foe_accent_color_delta(15);
					}
				}
				bool play_foe_pokeball = battle.control_foe_pokeballs();
				battle.move_line_under_foe_pokeballs();
				if (play_foe_pokeball){
					PlaySound(party[battle.current_foe_pokeball() - 1].get_pokedex().id == MISSINGNO ? absent : present);
				}
				if (animation_done){
					if (!party.empty()){
						current_line = ctx.formatter.format(battle.step_dialog(writer, ctx.locale), party[battle.current_foe_pokemon_idx()].get_pokedex().id);
						writer.set_text(current_line);
						battle.set_phase(ctx.assets, BATTLE_PHASE::FOE_SEND_OUT);
					}
				}

			}
			break;
		case BATTLE_PHASE::FOE_SEND_OUT:
			{
				if (battle.foe_alpha() > 0){
					battle.foe_alpha_delta(-15);
				}
				if (battle.foe_z() > -3.5f){
					battle.foe_z_delta(-0.05f);
				}

				if (finished_drawing_back && battle.trainer_alpha() > 0){
					battle.trainer_alpha_delta(-5);
				}
				if (foe_thrown_pokeball_renderer.spinned < 3){
					foe_thrown_pokeball_renderer.frame_timer += 1.0f;
					if (foe_thrown_pokeball_renderer.frame_timer > 1.0f){
						foe_thrown_pokeball_renderer.frame++;
						foe_thrown_pokeball_renderer.frame_timer = 0.0f;
					}
					if (foe_thrown_pokeball_renderer.frame > 9){
						foe_thrown_pokeball_renderer.spinned++;
						foe_thrown_pokeball_renderer.frame = 0;
					}
				} else {
					if (writer.finished){
						battle.inc_foe_pokemon_scale();
						if (battle.foe_pokemon_scale() >= pokemon_scale_dest && battle.foe_pokemon_y() > pokemon_y_dest){
							battle.foe_pokemon_y_delta(-0.05f);
						} else if (battle.foe_pokemon_y() <= pokemon_y_dest){
							if (battle.foe_hud_x() < 0.0f){
								battle.foe_hud_x_delta(20.0f);
								battle.set_camera_pos(BATTLE_CAMERA_POS::TRAINER);
								battle.set_phase(ctx.assets, BATTLE_PHASE::ALLY_SEND_OUT);
							}
						}
					}
					if (!foe_thrown_pokeball_renderer.opened){
						PlaySound(ctx.assets.get_sfx("SEQ_SE_BOWA2.wav"));
						PlaySound(ctx.assets.get_cry(party[battle.current_foe_pokemon_idx()].get_pokedex().id));
						foe_thrown_pokeball_renderer.opened = true;
					}
				}
			}
			break;
		case BATTLE_PHASE::ALLY_SEND_OUT:
			{

				if (battle.foe_hud_x() < 0.0f){
					battle.foe_hud_x_delta(20.0f);
				}

				if (!finished_drawing_back){
					bool play_trainer_pokeball = battle.control_trainer_pokeballs();
					if (play_trainer_pokeball){
						PlaySound(p_party[battle.current_trainer_pokeball() - 1].get_pokedex().id == MISSINGNO ? absent : present);
					}
					battle.move_line_under_trainer_pokeballs();
				} else {
					if (battle.ally_hud_x() > wnd_w - 120.0f * 5.0f){
						battle.ally_hud_x_delta(-20.0f);
					}
					if (!trainer_thrown_pokeball_renderer.thrown){
						PlaySound(ctx.assets.get_sfx("SEQ_SE_NAGERU.wav"));
						trainer_thrown_pokeball_renderer.thrown = true;
					}
					if (trainer_thrown_pokeball_renderer.spinned < 3){
						trainer_thrown_pokeball_renderer.frame_timer += 1.0f;
						if (trainer_thrown_pokeball_renderer.frame_timer > 1.0f){
							trainer_thrown_pokeball_renderer.frame++;
							trainer_thrown_pokeball_renderer.frame_timer = 0.0f;
						}
						if (trainer_thrown_pokeball_renderer.frame > 9){
							trainer_thrown_pokeball_renderer.spinned++;
							trainer_thrown_pokeball_renderer.frame = 0;
						}
						if (trainer_thrown_pokeball_z > 2.0f) trainer_thrown_pokeball_z -= 0.05f;
						if (trainer_thrown_pokeball_y >= 0.7f){
							trainer_thrown_pokeball_y += trainer_thrown_pokeball_y_vel;
							trainer_thrown_pokeball_y_vel -= 0.0025f;
						}
					} else {
						if (!trainer_thrown_pokeball_renderer.opened){
							PlaySound(ctx.assets.get_sfx("SEQ_SE_BOWA2.wav"));
							PlaySound(ctx.assets.get_cry(ctx.player.get_party()[battle.current_trainer_pokemon_idx()].get_pokedex().id));
							trainer_thrown_pokeball_renderer.opened = true;
						}
						battle.inc_ally_pokemon_scale();
						if (battle.ally_pokemon_scale() >= pokemon_scale_dest && battle.ally_pokemon_y() > pokemon_y_dest){
							battle.ally_pokemon_y_delta(-0.05f);
						} else {
							battle.set_camera_pos(BATTLE_CAMERA_POS::ALLY);
							if (battle.ally_pokemon_scale() >= pokemon_scale_dest && battle.ally_pokemon_y() <= pokemon_y_dest){
								battle.set_phase(ctx.assets, BATTLE_PHASE::MENU);
							}
						}
					}
				}
			}
			break;
		case BATTLE_PHASE::MENU:
			{
				if (battle_menu_y > wnd_h - 174.0f * 3.0f){
					battle_menu_y -= 29.0f;
				}
				switch (battle.menu_state()){
					case BATTLE_MENU_STATE::OPTIONS:
						if (next_menu_state == BATTLE_MENU_STATE::NONE){
							sel_opt = ctx.control_selection(opt, 3, sel_sfx, dec_sfx);
						}
						if (sel_opt != -1){
							switch (sel_opt){
								case 0:
									battle.set_menu_state(BATTLE_MENU_STATE::MOVES);
									break;
								case 1:
									next_menu_state = BATTLE_MENU_STATE::POKEMON;
									break;
								case 2:
									next_menu_state = BATTLE_MENU_STATE::BAG;
									break;
								case 3:
									break;
							}
						}
						break;
					case BATTLE_MENU_STATE::MOVES:
						break;
					case BATTLE_MENU_STATE::MODULES:
						break;
					case BATTLE_MENU_STATE::POKEMON:
						for (usz i = 0; i < ctx.player.get_party().size(); i++){
							p_party[i].block_frames(true);
							if (IsKeyPressed(KEY_X)){
								next_menu_state = BATTLE_MENU_STATE::OPTIONS;
							}
						}
						break;
					case BATTLE_MENU_STATE::BAG:
						break;
					default: break;
				}
			}
			break;
		default: break;
	}
	writer.update();
}

void battle_state_t::render(st_ctx_t& ctx){
	BeginMode3D(ctx.camera.camera);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, 0, 0}, {0, 0, 0}, 0, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, -1, 1}, {0, 1, 0}, bg_rotation, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_stage01.glb"), {0.9f, 0.0f, -0.9f}, {0, 0, 0}, 0.0f, {0.15f, 0.15f, 0.15f}, WHITE);
	DrawModelEx(ctx.assets.get_model("batt_stage01.glb"), {-0.3f, 0.0f, 1.8f}, {0, 0, 0}, 0.0f, {0.15f, 0.15f, 0.15f}, WHITE);
	auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();
	auto& foep = party[battle.current_foe_pokemon_idx()];
	auto& p_party = ctx.player.get_party();
	if (std::holds_alternative<trainer_t>(ctx.bt_info.foe.entry)){
		if (std::ranges::contains(std::array{"Cheren", "Bianca", "N"}, ctx.bt_info.name)){
			animation_done = anim_trainer_render.draw(ctx.assets, ctx.bt_info.name, {0.7f, 0.08f, battle.foe_z()}, &ctx.camera.camera, {battle.foe_accent_color(), battle.foe_accent_color(), battle.foe_accent_color(), battle.foe_alpha()}, battle.foe_accent_color() == 255);
		} else {
			draw_trainer(ctx.assets, ctx.bt_info.prefix, ctx.bt_info.name, {0.5f, 0.08f, battle.foe_z()}, &ctx.camera.camera, {battle.foe_accent_color(), battle.foe_accent_color(), battle.foe_accent_color(), battle.foe_alpha()});
			animation_timer ++;
			if (animation_timer > 125){
				animation_done = true;
			}
		}
	}
	auto& ally = p_party[battle.current_trainer_pokemon_idx()];
	if (battle.phase() >= BATTLE_PHASE::FOE_SEND_OUT){
		foep.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::FRONT, {0.6f, battle.foe_pokemon_y(), -0.3f}, {battle.foe_pokemon_scale(), battle.foe_pokemon_scale()}, &ctx.camera.camera);
		if (player_back.draw_back(ctx.assets, (ctx.player.get_sex() == SEX::MALE) ? "hilbert" : "hilda", {-6.0f, -1.0f, 3.5f}, ctx.camera.camera, {255, 255, 255, battle.trainer_alpha()})) finished_drawing_back = true;
		if (foe_thrown_pokeball_renderer.spinned < 3){
			foe_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {0.7f, 0.5f, -0.5f}, ctx.camera.camera);
		}
	}
	if (battle.phase() >= BATTLE_PHASE::ALLY_SEND_OUT){
		ally.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::BACK, {-0.4f, battle.ally_pokemon_y(), 2.0f}, {battle.ally_pokemon_scale(), battle.ally_pokemon_scale()}, &ctx.camera.camera);
		if (finished_drawing_back && trainer_thrown_pokeball_renderer.spinned < 3){
			trainer_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {-0.5f, trainer_thrown_pokeball_y, trainer_thrown_pokeball_z}, ctx.camera.camera);
		}
	}
	EndMode3D();
	battle.write(ctx.assets, writer);
	if (battle.phase() < BATTLE_PHASE::FOE_SEND_OUT){
		DrawTextureEx(ctx.assets.get_tx("line-under-foe-pokeballs.png"), {battle.line_under_foe_pokeballs(), 300.0f}, 0.0f, 8.0f, WHITE);
		auto xs = battle.foe_pokeball_x();
		for (usz i = 0; i < 6; i++){
			DrawTexturePro(ctx.assets.get_tx("foe-pokeball.png"), {(party[i].get_pokedex().id == MISSINGNO) ? 0.0f : 13.0f, 0, 13, 14}, {xs[i], 200.0f, 70.0f, 75.0f}, {0, 0}, 0, WHITE);
		}
	} else {
		if (battle.foe_pokemon_y() <= pokemon_y_dest){
			float hp_ratio = foep.hp() / foep.max_hp();
			float hp_idx = [&]() -> float{
				if (hp_ratio >= 0.7f){
					return 2;
				} else if (hp_ratio >= 0.4f){
					return 1;
				} else {
					return 0;
				}
			}();
			DrawTextureEx(ctx.assets.get_tx("foe-hud.png"), {battle.foe_hud_x(), 300.0f}, 0, 5, WHITE);
			DrawTexturePro(ctx.assets.get_tx("hp.png"), {hp_idx, 2, 1, 2}, {battle.foe_hud_x() + 220.0f, 345.0f, hp_ratio * 240.0f, 10.0f}, {0, 0}, 0, WHITE);
			draw_pokename(ctx.assets, names[foep.get_pokedex().id].data(), battle.foe_hud_x() + 100, 295.0f);
			char lvl_buf[4];
			std::to_chars(lvl_buf, lvl_buf + sizeof(lvl_buf), foep.lvl());
			draw_pokename(ctx.assets, lvl_buf, battle.foe_hud_x() + 450, 295.0f);
			DrawTexturePro(ctx.assets.get_tx("sex-sign.png"), {static_cast<float>(foep.sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {battle.foe_hud_x() + 385.0f, 295.0f, 28.0f, 40.0f}, {0, 0}, 0, WHITE);
		}
		if (!finished_drawing_back){
			const auto& ln_tx = ctx.assets.get_tx("line-under-foe-pokeballs.png");
			DrawTexturePro(ln_tx, {0, 0, static_cast<float>(-ln_tx.width), static_cast<float>(ln_tx.height)}, {battle.line_under_trainer_pokeballs(), 600.0f, ln_tx.width * 8.0f, ln_tx.height * 8.0f}, {0, 0}, 0.0f, WHITE);
			auto xs = battle.trainer_pokeball_x();
			for (usz i = 0; i < 6; i++){
				DrawTexturePro(ctx.assets.get_tx("foe-pokeball.png"), {(p_party[i].get_pokedex().id == MISSINGNO) ? 0.0f : 13.0f, 0, 13, 14}, {xs[i], 500.0f, 70.0f, 75.0f}, {0, 0}, 0, WHITE);
			}
		} else {
			float hp_ratio = ally.hp() / ally.max_hp();
			float hp_idx = [&]() -> float{
				if (hp_ratio >= 0.7f){
					return 2;
				} else if (hp_ratio >= 0.4f){
					return 1;
				} else {
					return 0;
				}
			}();
			DrawTextureEx(ctx.assets.get_tx("ally-hud.png"), {battle.ally_hud_x(), 540.0f}, 0, 5, WHITE);
			DrawTexturePro(ctx.assets.get_tx("hp.png"), {hp_idx, 2, 1, 2}, {battle.ally_hud_x() + 280.0f, 585.0f, hp_ratio * 240.0f, 10.0f}, {0, 0}, 0, WHITE);
			draw_pokename(ctx.assets, ally.nickname(), battle.ally_hud_x() + 100, 535.0f);
			char lvl_buf[4];
			auto [lvl_ptr, _] = std::to_chars(lvl_buf, lvl_buf + sizeof(lvl_buf), ally.lvl());
			char hp_buf[4];
			char max_hp_buf[4];
			auto [hp_ptr, _] = std::to_chars(hp_buf, hp_buf + sizeof(hp_buf), ally.hp());
			auto [max_hp_ptr, _] = std::to_chars(max_hp_buf, max_hp_buf + sizeof(max_hp_buf), ally.max_hp());
			ally.calculate_exp_cap();
			draw_pokename(ctx.assets, {lvl_buf, lvl_ptr}, battle.ally_hud_x() + 450, 535.0f);
			DrawTexturePro(ctx.assets.get_tx("sex-sign.png"), {static_cast<float>(ally.sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {battle.ally_hud_x() + 365.0f, 535.0f, 28.0f, 40.0f}, {0, 0}, 0, WHITE);
			draw_pokename(ctx.assets, {hp_buf, hp_ptr}, battle.ally_hud_x() + 330, 600.0f);
			draw_pokename(ctx.assets, {max_hp_buf, max_hp_ptr}, battle.ally_hud_x() + 420, 600.0f);
			DrawRectangle(battle.ally_hud_x() + 160, 655.0f, (static_cast<float>(ally.exp()) / ally.exp_cap()) * 400.0f, 5, {0, 153, 148, 255});
		}
	}
	if (battle.phase() == BATTLE_PHASE::MENU){
		DrawTextureEx(ctx.assets.get_tx("battle-menu.png"), {0, battle_menu_y}, 0, 3, WHITE);
		switch (battle.menu_state()){
			case BATTLE_MENU_STATE::OPTIONS:
				DrawTextureEx(ctx.assets.get_tx("battle-menu-opts.png"), {1050, battle_menu_y + 225.0f}, 0, 3, WHITE);
				DrawTextureEx(ctx.assets.get_tx("battle-menu-opt-sqbr.png"), {970, battle_menu_y + 225.0f + opt * 39.0f}, 0, 3, WHITE);
				break;
			case BATTLE_MENU_STATE::MOVES:
				break;
			case BATTLE_MENU_STATE::POKEMON:
				{
					const auto& party_screen = ctx.assets.get_tx("party-screen.png");
					DrawTexturePro(party_screen, full_tx(party_screen), {0, 0, wnd_w, 1440}, {0, 0}, 0, WHITE);
					for (usz i = 0; i < 6; i++){
						u8 banner_idx = 0;
						if (p_party[i].get_pokedex().id == MISSINGNO){
							banner_idx = 2;	
						} else {
							banner_idx = (i == opt) ? 1 : 0;
						}
						Vector2 banner_pos = {200.0f + (i % 2) * 710.0f, 100.0f + static_cast<u8>((i / 2)) * 254.0f + ((i % 2 != 0) ? 100.0f : 0.0f)};
						DrawTexturePro(ctx.assets.get_tx("party-banner.png"), {0, banner_idx * 46.0f, 126, 46}, {banner_pos.x, banner_pos.y, 700, 244}, {0, 0}, 0, {255, 255, 255, 200});
						if (p_party[i].get_pokedex().id != MISSINGNO){
							p_party[i].render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::MENU, {banner_pos.x, banner_pos.y - 20.0f, 0.0f}, {242.0f, 200.0f});
							draw_text(ctx.assets, names[p_party[i].get_pokedex().id], banner_pos.x + 200.0f, banner_pos.y + 50.0f, 50, WHITE);
							DrawTexturePro(ctx.assets.get_tx("sex-sign.png"),{static_cast<float>(p_party[i].sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {750.0f + (i % 2) * 710.0f, 150.0f + static_cast<u8>((i / 2)) * 254.0f + ((i % 2 != 0) ? 100.0f : 0.0f), 28.0f, 40.0f}, {0, 0}, 0, WHITE);
							DrawTextureEx(ctx.assets.get_tx("hpbar.png"), {banner_pos.x + 200.0f, banner_pos.y + 120.0f}, 0, 5, WHITE);
							float hp_ratio = p_party[i].hp() / p_party[i].max_hp();
							float hp_idx = [&]() -> float{
								if (hp_ratio >= 0.7f){
									return 2;
								} else if (hp_ratio >= 0.4f){
									return 1;
								} else {
									return 0;
								}
							}();
							DrawTexturePro(ctx.assets.get_tx("hp.png"), {hp_idx, 2, 1, 2}, {banner_pos.x + 285.0f, banner_pos.y + 135.0f, hp_ratio * 240.0f, 10.0f}, {0, 0}, 0, WHITE);
							draw_text(ctx.assets, std::format("Lv.{}", p_party[i].lvl()), banner_pos.x + 60.0f, banner_pos.y + 180.0f, 50, WHITE);
							draw_text(ctx.assets, std::format("{}/{}", p_party[i].hp(), p_party[i].max_hp()), banner_pos.x + 400.0f, banner_pos.y + 180.0f, 50, WHITE);
						}
						continue;
					}

				}
				break;
			default: break;
		}
	}
}
