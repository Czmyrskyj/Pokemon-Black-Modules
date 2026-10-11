#include "states.hpp"
#include "st-ctx.hpp"
#include "names.hpp"
#include "battle-bg.hpp"
#include "music_name.hpp"

#include <algorithm>
#include <format>
#include <cmath>

constexpr float pokemon_scale_dest{1.0f};
constexpr float pokemon_y_dest{0.1f};
constexpr float turn_timer_dest{30};


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

bool battle_state_t::control_foe_pokeballs() {
	if (current_foe_pokeball < 6){
		if (foe_pokeball_x[current_foe_pokeball] < (5 - current_foe_pokeball) * 72.0f + 30.0f){
			foe_pokeball_x[current_foe_pokeball] += 60.0f;
		} else {
			foe_pokeball_x[current_foe_pokeball] = (5 - current_foe_pokeball) * 72.0f + 30.0f;
			current_foe_pokeball++;
			return true;
		}
	}
	return false;
}

bool battle_state_t::control_ally_pokeballs() {
	if (current_trainer_pokeball < 6){
		if (trainer_pokeball_x[current_trainer_pokeball] > current_trainer_pokeball * 72.0f + (wnd_w - 500.0f)){
			trainer_pokeball_x[current_trainer_pokeball] -= 60.0f;
		} else {
			trainer_pokeball_x[current_trainer_pokeball] = current_trainer_pokeball * 72.0f + (wnd_w - 500.0f);
			current_trainer_pokeball++;
			return true;
		}
	}
	return false;
}

void battle_state_t::set_phase(assets_t& assets, BATTLE_PHASE p){
	phase = p;
	switch (p){
		case BATTLE_PHASE::FOE_SEND_OUT: 
			{
				PlaySound(assets.get_sfx("SEQ_SE_NAGERU.wav"));
			}
			break;
		case BATTLE_PHASE::MENU:
			{
				PlaySound(assets.get_sfx("SEQ_SE_OPEN2.wav"));
			}
			break;
		default: break;
	}
}

void battle_state_t::begin_foe_send_out(assets_t& assets){
	battle.set_current_foe_pokemon_idx(next_foe_idx);
	foe_thrown_pokeball_renderer.thrown = false;
	foe_thrown_pokeball_renderer.opened = false;
	foe_thrown_pokeball_renderer.spinned = 0;
	foe_thrown_pokeball_renderer.frame = 0;
	foe_thrown_pokeball_renderer.frame_timer = 0;
	foe_pokemon_scale = 0.0f;
	foe_pokemon_y = 0.5f;
	foe_hud_x = -620;
	txted = false;
	writer.finished = false;
	set_phase(assets, BATTLE_PHASE::FOE_SEND_OUT);
}

static party_t& pick_party(st_ctx_t& ctx){
	return (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();
}

battle_state_t::battle_state_t(st_ctx_t& ctx) : battle(ctx.player, ctx.bt_info.foe, pick_party(ctx)){
	ctx.camera.camera = {
		.position = {0.4f, 0.6f, -2.0f},
		.target = {-0.3f, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
	ctx.formatter.feed("[FOE_TYPE]", ctx.formatter.format(trainer_type_prefixes[static_cast<u8>(ctx.bt_info.trainer_type)]));
	ctx.formatter.feed("[FOE]", ctx.bt_info.name);
	current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, battle.get_dialog_intro()));
	foe_pokeball_x.fill(0.0f);
	trainer_pokeball_x.fill(wnd_w);
	writer.set_text(current_line);
}

void battle_state_t::update(st_ctx_t& ctx){
	ctx.camera.set_route({
			[&]() -> Vector3{
			switch (camera_pos){
			case BATTLE_CAMERA_POS::DEFAULT: return {0.2f, 0.6f, 2.0f};
			case BATTLE_CAMERA_POS::TRAINER: return {0.3f, 0.6f, 4.0f};
			case BATTLE_CAMERA_POS::ALLY: return {0.2f, 0.5f, 3.0f};
			case BATTLE_CAMERA_POS::FOE: return {0.4f, 0.5f, 1.5f};
			default: return {0.2f, 0.6f, 2.0f};
			}
			}()
			});
	ctx.camera.set_targ({
			[&]() -> Vector3{
			switch (camera_pos){
			case BATTLE_CAMERA_POS::DEFAULT: return {0.0f, 0.0f, 0.0f};
			case BATTLE_CAMERA_POS::TRAINER: return {0.0f, 0.0f, 0.0f};
			case BATTLE_CAMERA_POS::ALLY: return {-0.3f, 0.0f, 0.8f};
			case BATTLE_CAMERA_POS::FOE: return {0.9, 0.0f, -0.9f};
			default: return {0.0f, 0.0f, 0.0f};
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
	const auto* ally = &p_party[battle.current_trainer_pokemon_idx()];
	const auto* foep = &party[battle.current_foe_pokemon_idx()];

	if (next_menu_state != BATTLE_MENU_STATE::NONE){
		float current_alpha = static_cast<float>(ctx.transition_alpha);
		if (menu_state != next_menu_state){
			current_alpha+=300.0f*GetFrameTime();
			if (current_alpha >= 255.0f){
				current_alpha = 255.0f;
				for (usz i = 0; i < p_party.size(); i++){
					p_party[i].block_frames(false);
				}
				menu_state = next_menu_state;
				opt = 0;
				sel_opt = -1;
			}
		} else {
			if (next_pokemon != -1){
				ally_pokemon_scale = 0.0f;
				ally_pokemon_y = 0.5f;
				ally_hud_x = wnd_w;
				ally = &p_party[next_pokemon];
				critical_hp = false;
				ally_hp_render = ally->hp();
				battle.set_current_trainer_pokemon_idx(next_pokemon);
				set_phase(ctx.assets, BATTLE_PHASE::ALLY_SEND_OUT);
				trainer_thrown_pokeball_renderer.thrown = false;
				trainer_thrown_pokeball_renderer.opened = false;
				trainer_thrown_pokeball_renderer.spinned = 0;
				txted = false;
				next_pokemon = -1;
			}
			current_alpha-=300.0f*GetFrameTime();
			if (current_alpha <= 0.0f){
				current_alpha = 0.0f;
				next_menu_state = BATTLE_MENU_STATE::NONE;
			}
		}
		ctx.transition_alpha = static_cast<unsigned char>(current_alpha);
	}


	switch (phase){
		case BATTLE_PHASE::INTRO:
			{
				if (ctx.set_state_meta){
					if (foe_accent_color < 255){
						foe_accent_color += 15;
					}
				}
				bool play_foe_pokeball = control_foe_pokeballs();
				if (line_under_foe_pokeballs < 0.0f) line_under_foe_pokeballs += 40.0f;
				if (play_foe_pokeball){
					PlaySound(party[current_foe_pokeball - 1].get_pokedex().id == MISSINGNO ? absent : present);
				}
				if (animation_done){
					if (!party.empty()){
						set_phase(ctx.assets, BATTLE_PHASE::FOE_SEND_OUT);
						txted = false;
						writer.finished = false;
					}
				}

			}
			break;
		case BATTLE_PHASE::FOE_SEND_OUT:
			{
				if (!txted){
					current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "FOE_SENT_OUT"), foep->get_pokedex().id);
					writer.set_text(current_line);
					txted = true;
				}
				if (foe_alpha > 0){
					foe_alpha -= 15;
				}
				if (foe_z > -3.5f){
					foe_z -= 0.05f;
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
						if (foe_pokemon_scale < 1.0f) foe_pokemon_scale += 0.05f;
						if (foe_pokemon_scale >= pokemon_scale_dest && foe_pokemon_y > pokemon_y_dest){
							foe_pokemon_y -= 0.05f;
						} else if (foe_pokemon_y <= pokemon_y_dest){
							if (foe_hud_x < 0.0f && battle.txtbox_alpha() == 0){
								foe_hud_x += 20.0f;
								camera_pos = BATTLE_CAMERA_POS::TRAINER;
								if (!foe_about_to_send){
									set_phase(ctx.assets, BATTLE_PHASE::ALLY_SEND_OUT);
								} else {
									set_phase(ctx.assets, BATTLE_PHASE::MENU);
									menu_state = BATTLE_MENU_STATE::OPTIONS;
									camera_pos = BATTLE_CAMERA_POS::ALLY;
									foe_about_to_send = false;
								}
								txted = false;
							}
						}
					}
					if (!foe_thrown_pokeball_renderer.opened){
						PlaySound(ctx.assets.get_sfx("SEQ_SE_BOWA2.wav"));
						PlaySound(ctx.assets.get_cry(foep->get_pokedex().id));
						foe_hp_render = foep->hp();
						foe_thrown_pokeball_renderer.opened = true;
					}
				}
			}
			break;
		case BATTLE_PHASE::ALLY_SEND_OUT:
			{
				if (!txted){
					current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "TRAINER_SENT_OUT"), ally->get_pokedex().id);
					writer.set_text(current_line);
					txted = true;
				}
				if (foe_hud_x < 0.0f){
					foe_hud_x += 20.0f;
				}

				if (!finished_drawing_back){
					bool play_trainer_pokeball = control_ally_pokeballs();
					if (play_trainer_pokeball){
						PlaySound(p_party[current_trainer_pokeball - 1].get_pokedex().id == MISSINGNO ? absent : present);
					}
					if (line_under_trainer_pokeballs > 1120.0f) line_under_trainer_pokeballs -= 40.0f;
				} else {
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
							PlaySound(ctx.assets.get_cry(ally->get_pokedex().id));
							ally_hp_render = ally->hp();
							trainer_thrown_pokeball_renderer.opened = true;
						} else {
							if (ally_hud_x > wnd_w - 120.0f * 5.0f){
								ally_hud_x -= 20.0f;
							}
						}
						if (ally_pokemon_scale < 1.0f) ally_pokemon_scale += 0.05f;
						if (ally_pokemon_scale >= pokemon_scale_dest && ally_pokemon_y > pokemon_y_dest){
							ally_pokemon_y -= 0.05f;
						} else {
							camera_pos = BATTLE_CAMERA_POS::ALLY;
							if (ally_pokemon_scale >= pokemon_scale_dest && ally_pokemon_y <= pokemon_y_dest && battle.txtbox_alpha() == 0){
								if (!foe_about_to_send) set_phase(ctx.assets, BATTLE_PHASE::MENU);
								else begin_foe_send_out(ctx.assets);
							}
						}
					}
				}
				if (finished_drawing_back && trainer_alpha > 0){
					trainer_alpha -= 5;
				}
			}
			break;
		case BATTLE_PHASE::MENU:
			{
				if (foe_hud_x < 0.0f) foe_hud_x += 20.0f;
				if (battle_menu_y > wnd_h - 174.0f * 3.0f){
					battle_menu_y -= 29.0f;
				}
				switch (menu_state){
					case BATTLE_MENU_STATE::OPTIONS:
						if (next_menu_state == BATTLE_MENU_STATE::NONE && !running_from_tr_batt){
							sel_opt = ctx.control_selection(opt, 3, sel_sfx, dec_sfx);
						}
						if (sel_opt != -1){
							switch (sel_opt){
								case 0:
									menu_state = BATTLE_MENU_STATE::MOVES;
									break;
								case 1:
									next_menu_state = BATTLE_MENU_STATE::POKEMON;
									break;
								case 2:
									next_menu_state = BATTLE_MENU_STATE::BAG;
									break;
								case 3:
									running_from_tr_batt = true;
									break;
							}
						}
						break;
					case BATTLE_MENU_STATE::MOVES:
						if (IsKeyPressed(KEY_X)){
							menu_state = BATTLE_MENU_STATE::OPTIONS;
						}
						if (IsKeyPressed(KEY_RIGHT)){
							PlaySound(sel_sfx);
							if (opt < ally->move_count() - 1){
								opt ++;
							} else {
								opt = 0;
							}
						}
						if (IsKeyPressed(KEY_LEFT)){
							PlaySound(sel_sfx);
							if (opt > 0){
								opt --;
							} else {
								opt = ally->move_count() - 1;
							}
						}
						if (IsKeyPressed(KEY_DOWN)){
							PlaySound(sel_sfx);
							if (opt < 2){
								opt += 2;
							} else {
								opt = (opt % 2) != 0;
							}
						}
						if (IsKeyPressed(KEY_UP)){
							PlaySound(sel_sfx);
							if (opt > 1){
								opt -= 2;
							} else {
								opt = (opt % 2 == 0) ? ally->move_count() - 2 : ally->move_count() - 1;
							}
						}
						if (IsKeyPressed(KEY_Z)){
							battle.set_action(TURN_ACTION::ATTACK, BATTLE_TURN::TRAINER, opt, 0);
						}
						break;
					case BATTLE_MENU_STATE::MODULES:
						break;
					case BATTLE_MENU_STATE::POKEMON:
						for (usz i = 0; i < ctx.player.get_party().size(); i++){
							p_party[i].block_frames(true);
						}
						if (IsKeyPressed(KEY_X) && !switch_necessary){
							next_menu_state = BATTLE_MENU_STATE::OPTIONS;
						}
						if (next_menu_state == BATTLE_MENU_STATE::NONE){
							if (IsKeyPressed(KEY_RIGHT)){
								PlaySound(sel_sfx);
								if (opt < p_party.size() - 1){
									opt ++;
								} else {
									opt = 0;
								}
							}
							if (IsKeyPressed(KEY_LEFT)){
								PlaySound(sel_sfx);
								if (opt > 0){
									opt --;
								} else {
									opt = p_party.size() - 1;
								}
							}
							if (IsKeyPressed(KEY_DOWN)){
								PlaySound(sel_sfx);
								if (opt < p_party.size() - 2){
									opt += 2;
								} else {
									opt = (opt % 2) != 0;
								}
							}
							if (IsKeyPressed(KEY_UP)){
								PlaySound(sel_sfx);
								if (opt > 1){
									opt -= 2;
								} else {
									opt = (opt % 2 == 0) ? p_party.size() - 2 : p_party.size() - 1;
								}
							}
							if (IsKeyPressed(KEY_Z)){
								PlaySound(dec_sfx);
								if (opt != battle.current_trainer_pokemon_idx() && p_party[opt].hp() > 0){
									next_pokemon = opt;
								}
							}
							if (next_pokemon != -1){
								next_menu_state = BATTLE_MENU_STATE::OPTIONS;
								if (!switch_necessary){
									battle.set_action(TURN_ACTION::SWITCH, BATTLE_TURN::TRAINER, next_pokemon);
								}
								switch_necessary = false;
							}
						}
						break;
					case BATTLE_MENU_STATE::BAG:
						if (IsKeyPressed(KEY_X)){
							next_menu_state = BATTLE_MENU_STATE::OPTIONS;
						}
						break;
					default: break;
				}
				if (std::holds_alternative<trainer_t>(ctx.bt_info.foe) && !critical_hp && !IsMusicStreamPlaying(ctx.assets.get_mus("Victory is Right Before Your Eyes!.ogg"))){
					auto& tr = std::get<trainer_t>(ctx.bt_info.foe);
					if (ctx.bt_info.trainer_type == TRAINER_TYPE::GYM_LEADER && tr.conscious_count() == 1){
						ctx.set_music("Victory is Right Before Your Eyes!.ogg");
					}
				}
			}
			break;
		case BATTLE_PHASE::FIGHT:
			if (writer.finished && foe_hp_render == foep->hp() && ally_hp_render == ally->hp()){
				if (auto next = battle.next_turn()){
					const bool is_trainer = (*next == BATTLE_TURN::TRAINER);
					const pokemon_t& attacked = !is_trainer ? *ally : *foep;
					const pokemon_t& cur = is_trainer ? *ally : *foep;
					if (!txted){
						const turn_action_t act = is_trainer ? battle.trainer_turn_action() : battle.foe_turn_action();
						camera_pos = is_trainer ? BATTLE_CAMERA_POS::FOE : BATTLE_CAMERA_POS::ALLY;
						const auto* atk = std::get_if<attack_action_t>(&act);

						if (atk && cur.hp() > 0){
							current_line = ctx.formatter.format(
									ctx.locale.get(Locale::TYPE::UI, "PKMN_USED"), 
									cur.get_pokedex().id, 
									ctx.movedex.get_move(cur.moves()[atk->move_slot]).name);
							writer.set_text(current_line);
							txted = true;
							turn_timer = 0;
						} else {
							if (++turn_timer >= turn_timer_dest){
								battle.execute_action(ctx.movedex, ctx.assets);
								turn_timer = 0;
							}
						}
					} else {
						if (++turn_timer >= turn_timer_dest){
							battle.execute_action(ctx.movedex, ctx.assets);
							txted = false;
							if (attacked.hp() == 0){
								trainer_t* owner = nullptr;
								float& poke_y = is_trainer ?  foe_pokemon_y : ally_pokemon_y;
								if (std::holds_alternative<trainer_t>(ctx.bt_info.foe)){
									owner = is_trainer ?  &std::get<trainer_t>(ctx.bt_info.foe) : &ctx.player;
								} else {
									owner = &ctx.player;
								}
								if (owner->conscious_count() > 0){
									owner->set_conscious_count(owner->conscious_count() - 1);
								}
								if (owner->conscious_count() == 0){
									battle.set_winner(is_trainer ? BATTLE_FIGURE::TRAINER : BATTLE_FIGURE::FOE);
								}
								if (poke_y > -1.0f){
									Sound dead = ctx.assets.get_sfx("SEQ_SE_DEAD.wav");
									if (foe_pokemon_y >= 0.0f && !IsSoundPlaying(dead)){
										PlaySound(dead);
									}
									poke_y = -0.02f;
								}
							} else {
								turn_timer = 0;
							}
						}
						if (battle.figure_missed() != BATTLE_FIGURE::NONE){
							current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "ATTACK_MISSED"), cur.get_pokedex().id);
							writer.set_text(current_line);
							battle.set_figure_missed(BATTLE_FIGURE::NONE);
						}
					}
				} else {
					float& poke_y = ally_pokemon_y < 0.0f ? ally_pokemon_y : foe_pokemon_y;
					if (poke_y >= 0.0f){
						set_phase(ctx.assets, BATTLE_PHASE::MENU);
						camera_pos = BATTLE_CAMERA_POS::ALLY;
						menu_state = BATTLE_MENU_STATE::OPTIONS;
					} else if (poke_y < 0.0f && poke_y > -1.0f){
						poke_y -= 0.02f;
					} else if (battle.winner() == BATTLE_FIGURE::NONE) {
						if (foe_pokemon_y == poke_y){
							set_phase(ctx.assets, BATTLE_PHASE::FOE_ABOUT_TO_SEND);
							menu_state = BATTLE_MENU_STATE::OPTIONS;
						} else if (std::holds_alternative<trainer_t>(ctx.bt_info.foe) && ctx.player.conscious_count() > 0){
							set_phase(ctx.assets, BATTLE_PHASE::MENU);
							menu_state = BATTLE_MENU_STATE::NONE;
							next_menu_state = BATTLE_MENU_STATE::POKEMON;
							switch_necessary = true;
						}
						camera_pos = BATTLE_CAMERA_POS::ALLY;
						txted = false;
					} else {
						set_phase(ctx.assets, BATTLE_PHASE::END);
						txted = false;
					}
				}
			}
			if (foe_hp_render > foep->hp()){
				foe_hp_render--;
			}
			if (foe_hp_render < foep->hp()){
				foe_hp_render++;
			}
			if (ally_hp_render > ally->hp()){
				ally_hp_render--;
			}
			if (ally_hp_render < ally->hp()){
				ally_hp_render++;
			}
			break;
		case BATTLE_PHASE::FOE_ABOUT_TO_SEND:
			{
				if (!txted){
					usz n = battle.current_foe_pokemon_idx() + 1;
					while (n < party.size() && (party[n].hp() == 0 || party[n].get_pokedex().id == MISSINGNO)) n++;
					next_foe_idx = (n < party.size()) ? n : battle.current_foe_pokemon_idx();
					opt = 0;
					current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "FOE_ABOUT_TO_SEND"), party[next_foe_idx].get_pokedex().id);
					writer.set_text(current_line);
					txted = true;
				} else if (unique_txt_timer < 50){
					unique_txt_timer++;
				} else if (unique_txt_entry != UNIQUE_TXT_ENTRY::WILL_SWITCH_PKMN){
					current_line = ctx.locale.get(Locale::TYPE::UI, "WILL_SWITCH_PKMN");
					writer.set_text(current_line);
					unique_txt_entry = UNIQUE_TXT_ENTRY::WILL_SWITCH_PKMN;
				} else {
					const i8 choice = ctx.control_selection(opt, 1, sel_sfx, dec_sfx, false);
					foe_about_to_send = true;
					if (choice != -1){
						unique_txt_entry = UNIQUE_TXT_ENTRY::NONE;
						unique_txt_timer = 0;
						if (choice == 0){
							menu_state = BATTLE_MENU_STATE::NONE;
							next_menu_state = BATTLE_MENU_STATE::POKEMON;
							switch_necessary = true;
							txted = false;
							set_phase(ctx.assets, BATTLE_PHASE::MENU);
						} else if (choice == 1){
							begin_foe_send_out(ctx.assets);
						}
					}
				}
			}
			break;
		case BATTLE_PHASE::END:
			{
				foe_alpha = 255;
				foe_z = -0.9f;
				camera_pos = BATTLE_CAMERA_POS::DEFAULT;
				if (battle.winner() == BATTLE_FIGURE::TRAINER){
					ctx.set_music(get_victory_music_name(ctx));
					if (!txted){
						current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "FOE_WAS_DEFEATED"));
						writer.set_text(current_line);
						txted = true;
					}
					if (txted && writer.finished){
						unique_txt_timer++;
					}
					if (unique_txt_entry != UNIQUE_TXT_ENTRY::FOE_DEFEAT_MSG && unique_txt_timer > 50){
						current_line = ctx.formatter.format(ctx.bt_info.defeat_msg);
						writer.set_text(current_line);
						unique_txt_entry = UNIQUE_TXT_ENTRY::FOE_DEFEAT_MSG;
					}
				}
			}
			break;
	}
	if (!std::holds_alternative<none_action_t>(battle.trainer_turn_action()) &&
			(ally_pokemon_y <= pokemon_y_dest &&
			 ally_pokemon_scale >= pokemon_scale_dest &&
			 next_menu_state == BATTLE_MENU_STATE::NONE
			) && phase != BATTLE_PHASE::FIGHT){
		battle.set_ai_action();
		battle.determine_order(ctx.movedex);
		turn_timer = 0;
		txted = false;
		phase = BATTLE_PHASE::FIGHT;
	}
	if (ally_hp_render == ally->hp()){
		float hp_ratio = ally->hp() / ally->max_hp();	
		bool critical = hp_ratio < 0.4f && hp_ratio > 0.0f;
		if (critical){
			critical_hp = true;
			if (!IsMusicStreamPlaying(ctx.assets.get_mus("Battle Trouble!.ogg"))){
				ctx.set_music("Battle Trouble!.ogg");
			}
		} else if (critical_hp) {
			ctx.set_music(get_music_name(ctx));
		}
	}
	if (running_from_tr_batt){
		if (std::holds_alternative<trainer_t>(ctx.bt_info.foe)){
			if (no_run_from_tr_batt_scrn_alpha < 200 && no_run_from_tr_batt_scrn_alpha_tim < 100){
				no_run_from_tr_batt_scrn_alpha += 10;
			} else {
				if (no_run_from_tr_batt_scrn_alpha_tim < 100){
					no_run_from_tr_batt_scrn_alpha_tim++;
				} else {
					no_run_from_tr_batt_scrn_alpha -= 10;
				}
			}
			if (no_run_from_tr_batt_scrn_alpha == 0 && no_run_from_tr_batt_scrn_alpha_tim >= 100){
				running_from_tr_batt = false;
				no_run_from_tr_batt_scrn_alpha = 0;
				no_run_from_tr_batt_scrn_alpha_tim = 0;
			}
		}
	}
	writer.update();
}


void battle_state_t::render(st_ctx_t& ctx){
	BeginMode3D(ctx.camera.camera);

	auto season = ctx.time.get_season();
	DrawModelEx(ctx.assets.get_model(get_battle_bg_name(ctx.region, season, BAT_BG_NAME::BG)), {0, 0, 0}, {0, 0, 0}, 0, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model(get_battle_bg_name(ctx.region, season, BAT_BG_NAME::BG)), {0, -1, 2}, {0, 1, 0}, bg_rotation, {0.2f, 0.2f, 0.2f}, WHITE);
	DrawModelEx(ctx.assets.get_model(get_battle_bg_name(ctx.region, season, BAT_BG_NAME::STAGE)), {0.9f, 0.0f, -0.9f}, {0, 0, 0}, 0.0f, {0.15f, 0.15f, 0.15f}, WHITE);
	DrawModelEx(ctx.assets.get_model(get_battle_bg_name(ctx.region, season, BAT_BG_NAME::STAGE)), {-0.3f, 0.0f, 1.8f}, {0, 0, 0}, 0.0f, {0.15f, 0.15f, 0.15f}, WHITE);
	auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
		(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
		ctx.bianca.get_party();
	auto& foep = party[battle.current_foe_pokemon_idx()];
	auto& p_party = ctx.player.get_party();
	if (std::holds_alternative<trainer_t>(ctx.bt_info.foe)){
		if (std::ranges::contains(std::array{"Cheren", "Bianca", "N"}, ctx.bt_info.name)){
			animation_done = anim_trainer_render.draw(ctx.assets, ctx.bt_info.name, {0.7f, 0.08f, foe_z}, &ctx.camera.camera, {foe_accent_color, foe_accent_color, foe_accent_color, foe_alpha}, foe_accent_color == 255);
		} else {
			draw_trainer(ctx.assets, ctx.bt_info.trainer_type, ctx.bt_info.name, {0.5f, 0.08f, foe_z}, &ctx.camera.camera, {foe_accent_color, foe_accent_color, foe_accent_color, foe_alpha});
			animation_timer ++;
			if (animation_timer > 125){
				animation_done = true;
			}
		}
	}
	auto& ally = p_party[battle.current_trainer_pokemon_idx()];
	if (phase >= BATTLE_PHASE::FOE_SEND_OUT){
		foep.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::FRONT, {0.6f, foe_pokemon_y, -0.3f}, {foe_pokemon_scale, foe_pokemon_scale}, &ctx.camera.camera);
		if (player_back.draw_back(ctx.assets, (ctx.player.sex() == SEX::MALE) ? "hilbert" : "hilda", {-6.0f, -1.0f, 3.5f}, ctx.camera.camera, {255, 255, 255, trainer_alpha})) finished_drawing_back = true;
		if (foe_thrown_pokeball_renderer.spinned < 3){
			foe_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {0.7f, 0.5f, -0.5f}, ctx.camera.camera);
		}
	}
	if (phase >= BATTLE_PHASE::ALLY_SEND_OUT || foe_about_to_send){
		ally.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::BACK, {-0.4f, ally_pokemon_y, 2.0f}, {ally_pokemon_scale, ally_pokemon_scale}, &ctx.camera.camera);
		if (finished_drawing_back && trainer_thrown_pokeball_renderer.spinned < 3){
			trainer_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {-0.5f, trainer_thrown_pokeball_y, trainer_thrown_pokeball_z}, ctx.camera.camera);
		}
	}
	EndMode3D();
	if (unique_txt_entry == UNIQUE_TXT_ENTRY::NONE){
		battle.write(ctx.assets, writer);
	} else {
		if (battle.txtbox_alpha() < 255){
			battle.set_txtbox_alpha(battle.txtbox_alpha() + 1);
		}
		draw_text_seq(ctx.assets, writer, 10, wnd_h - 200, 50, {255, 255, 255, battle.txtbox_alpha()}, 1, {150, 150, 150, battle.txtbox_alpha()});
	}
	if (phase < BATTLE_PHASE::FOE_SEND_OUT){
		DrawTextureEx(ctx.assets.get_tx("line-under-foe-pokeballs.png"), {line_under_foe_pokeballs, 300.0f}, 0.0f, 8.0f, WHITE);
		auto xs = foe_pokeball_x;
		for (usz i = 0; i < 6; i++){
			DrawTexturePro(ctx.assets.get_tx("foe-pokeball.png"), {(party[i].get_pokedex().id == MISSINGNO) ? 0.0f : 13.0f, 0, 13, 14}, {xs[i], 200.0f, 70.0f, 75.0f}, {0, 0}, 0, WHITE);
		}
	} else {
		if (foe_pokemon_y <= pokemon_y_dest && foe_pokemon_y >= 0.0f){
			float hp_ratio = foe_hp_render / foep.max_hp();
			float hp_idx = [&]() -> float{
				if (hp_ratio >= 0.7f){
					return 2;
				} else if (hp_ratio >= 0.4f){
					return 1;
				} else {
					return 0;
				}
			}();
			DrawTextureEx(ctx.assets.get_tx("foe-hud.png"), {foe_hud_x, 300.0f}, 0, 5, WHITE);
			DrawTexturePro(ctx.assets.get_tx("hp.png"), {hp_idx, 2, 1, 2}, {foe_hud_x + 220.0f, 345.0f, hp_ratio * 240.0f, 10.0f}, {0, 0}, 0, WHITE);
			draw_pokename(ctx.assets, names[foep.get_pokedex().id].data(), foe_hud_x + 100, 295.0f);
			char lvl_buf[4];
			std::to_chars(lvl_buf, lvl_buf + sizeof(lvl_buf), foep.lvl());
			draw_pokename(ctx.assets, lvl_buf, foe_hud_x + 450, 295.0f);
			DrawTexturePro(ctx.assets.get_tx("sex-sign.png"), {static_cast<float>(foep.sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {foe_hud_x + 385.0f, 295.0f, 28.0f, 40.0f}, {0, 0}, 0, WHITE);
		}
		if (!finished_drawing_back){
			const auto& ln_tx = ctx.assets.get_tx("line-under-foe-pokeballs.png");
			DrawTexturePro(ln_tx, {0, 0, static_cast<float>(-ln_tx.width), static_cast<float>(ln_tx.height)}, {line_under_trainer_pokeballs, 600.0f, ln_tx.width * 8.0f, ln_tx.height * 8.0f}, {0, 0}, 0.0f, WHITE);
			auto xs = trainer_pokeball_x;
			for (usz i = 0; i < 6; i++){
				DrawTexturePro(ctx.assets.get_tx("foe-pokeball.png"), {(p_party[i].get_pokedex().id == MISSINGNO) ? 0.0f : 13.0f, 0, 13, 14}, {xs[i], 500.0f, 70.0f, 75.0f}, {0, 0}, 0, WHITE);
			}
		} else {
			float hp_ratio = ally_hp_render / ally.max_hp();
			float hp_idx = [&]() -> float{
				if (hp_ratio >= 0.7f){
					return 2;
				} else if (hp_ratio >= 0.4f){
					return 1;
				} else {
					return 0;
				}
			}();
			DrawTextureEx(ctx.assets.get_tx("ally-hud.png"), {ally_hud_x, 540.0f}, 0, 5, WHITE);
			DrawTexturePro(ctx.assets.get_tx("hp.png"), {hp_idx, 2, 1, 2}, {ally_hud_x + 280.0f, 585.0f, hp_ratio * 240.0f, 10.0f}, {0, 0}, 0, WHITE);
			draw_pokename(ctx.assets, ally.nickname(), ally_hud_x + 100, 535.0f);
			char lvl_buf[4];
			auto [lvl_ptr, _] = std::to_chars(lvl_buf, lvl_buf + sizeof(lvl_buf), ally.lvl());
			char hp_buf[4];
			char max_hp_buf[4];
			auto [hp_ptr, _] = std::to_chars(hp_buf, hp_buf + sizeof(hp_buf), ally.hp());
			auto [max_hp_ptr, _] = std::to_chars(max_hp_buf, max_hp_buf + sizeof(max_hp_buf), ally.max_hp());
			ally.calculate_exp_cap();
			draw_pokename(ctx.assets, {lvl_buf, lvl_ptr}, ally_hud_x + 450, 535.0f);
			DrawTexturePro(ctx.assets.get_tx("sex-sign.png"), {static_cast<float>(ally.sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {ally_hud_x + 365.0f, 535.0f, 28.0f, 40.0f}, {0, 0}, 0, WHITE);
			draw_pokename(ctx.assets, {hp_buf, hp_ptr}, ally_hud_x + 330, 600.0f);
			draw_pokename(ctx.assets, {max_hp_buf, max_hp_ptr}, ally_hud_x + 420, 600.0f);
			DrawRectangle(ally_hud_x + 160, 655.0f, (static_cast<float>(ally.exp()) / ally.exp_cap()) * 400.0f, 5, {0, 153, 148, 255});
		}
	}
	if (phase == BATTLE_PHASE::MENU){
		if (!switch_necessary){
			DrawTextureEx(ctx.assets.get_tx("battle-menu.png"), {0, battle_menu_y}, 0, 3, WHITE);
		}
		switch (menu_state){
			case BATTLE_MENU_STATE::OPTIONS:
				DrawTextureEx(ctx.assets.get_tx("battle-menu-opts.png"), {1050, battle_menu_y + 225.0f}, 0, 3, WHITE);
				DrawTextureEx(ctx.assets.get_tx("battle-menu-opt-sqbr.png"), {970, battle_menu_y + 225.0f + opt * 39.0f}, 0, 3, WHITE);
				break;
			case BATTLE_MENU_STATE::MOVES:
				for (usz i = 0; i < 4; i++){
					move_t move = ctx.movedex.get_move(ally.moves()[i]);
					u8 col = (opt == i) ? 255 : 150;
					Color typ_col = get_type_color(move.type);
					DrawTextureEx(ctx.assets.get_tx("move.png"), {1330.0f + (i % 2) * 270.0f, 770.0f + static_cast<u8>((i / 2)) * 140.0f}, 0, 3.4f, {static_cast<u8>(typ_col.r * (col / 255.0f)), static_cast<u8>(typ_col.g * (col / 255.0f)), static_cast<u8>(typ_col.b * (col / 255.0f)), 255});
					draw_text(ctx.assets, move.name, 1340.0f + (i % 2) * 270.0f, 785.0f + static_cast<u8>((i / 2)) * 140.0f, 35, {col, col, col, 255}, {static_cast<u8>(col - 105), static_cast<u8>(col - 105), static_cast<u8>(col - 105), 255});
					if (move.id != 0){
						draw_text(ctx.assets, std::format("PP:{}/{}", move.pp - ally.pp_used()[i], move.pp), 1340.0f + (i % 2) * 270.0f, 850.0f + static_cast<u8>((i / 2)) * 140.0f, 30, {col, col, col, 255}, {static_cast<u8>(col - 105), static_cast<u8>(col - 105), static_cast<u8>(col - 105), 255});
						DrawTexturePro(ctx.assets.get_tx("type-icons.png"), {0, (static_cast<u8>(move.type) - 1) * 12.0f, 32, 12}, {1470.0f + (i % 2) * 270.0f, 840.0f + static_cast<u8>((i / 2)) * 140.0f, 100.0f, 38.0f}, {0, 0}, 0, {col, col, col, 255});
					}
				}
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
					}

				}
				break;
			default: break;
		}
		if (running_from_tr_batt){
			DrawRectangle(0, 0, wnd_w, wnd_h, Color{0, 0, 0, no_run_from_tr_batt_scrn_alpha});
			draw_text(ctx.assets, "No! There's no running from Trainer battle!", wnd_w / 2.0f - 400.0f, wnd_h / 2.0f, 50, Color{255, 255, 255, no_run_from_tr_batt_scrn_alpha});
		}
	}
	if (phase == BATTLE_PHASE::FOE_ABOUT_TO_SEND){
		if (unique_txt_entry == UNIQUE_TXT_ENTRY::WILL_SWITCH_PKMN){
			for (usz i = 0; i < 2; i++){
				Texture2D button = ctx.assets.get_tx("opt.png");
				DrawTexturePro(button, {0, (opt == i) * 24.0f, 70.0f, 24.0f}, {wnd_w - 300, wnd_h - 410 + i * (button.height * 2.0f), button.width * 4.0f, button.height * 2.0f}, {0, 0}, 0, WHITE);
				draw_text(ctx.assets, yes_no[i], wnd_w - 300 + 30.0f, wnd_h - 410 + i * (button.height * 2.0f) + 15.0f, 60, WHITE);
			}
		}
	}
}

