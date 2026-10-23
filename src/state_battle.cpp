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
constexpr u8 unique_txt_dest{50};

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
				camera_pos = BATTLE_CAMERA_POS::ALLY;
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
	is_foe_trainer = std::holds_alternative<trainer_t>(ctx.bt_info.foe);
	if (is_foe_trainer){
		ctx.formatter.feed("[FOE_TYPE]", ctx.formatter.format(trainer_type_prefixes[static_cast<u8>(ctx.bt_info.trainer_type)]));
		ctx.formatter.feed("[FOE]", ctx.bt_info.name);
	} else {
		battle.determine_wild_encounter(ctx.bt_info.wild_rates);
		foe_pokemon_scale = 1.0f;
		foe_pokemon_y = pokemon_y_dest;
		foe_hp_render = ctx.bt_info.party[battle.current_foe_pokemon_idx()].max_hp();
	}
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
	if (running_from_battle){
		ctx.switch_state(STATE::MAIN);
		ctx.show_encounter_info = true;
		ctx.initialized_encounter_info = false;
		// TODO: FADE MUSIC
		return;
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
				opt2 = 0;
				opt3 = 0;
				sel_opt = -1;
				sel_opt2 = -1;
				bag_animation_frame_timer = 0;
				bag_animation_frame= 0 ;
				bags_x = -300.0f;
				bags_y_jump = 0.0f;
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
	SetMusicVolume(ctx.current_music, ctx.music_volume);

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
				if (animation_done && !party.empty()){
					if (is_foe_trainer){
						set_phase(ctx.assets, BATTLE_PHASE::FOE_SEND_OUT);
						txted = false;
						writer.finished = false;
					} else {
						set_phase(ctx.assets, BATTLE_PHASE::ALLY_SEND_OUT);
						camera_pos = BATTLE_CAMERA_POS::TRAINER;
						if (auto foe_cry = ctx.assets.get_cry(ctx.bt_info.party[battle.current_foe_pokemon_idx()].get_pokedex().id); !IsSoundPlaying(foe_cry)){
							PlaySound(foe_cry);
						}
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
				if (is_foe_trainer && foe_hud_x < 0.0f){
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
									item_used = 0;
									next_menu_state = BATTLE_MENU_STATE::POKEMON;
									break;
								case 2:
									next_menu_state = BATTLE_MENU_STATE::BAG;
									break;
								case 3:
									if (is_foe_trainer){
										running_from_tr_batt = true;
									} else {
										running_from_battle = true;
										return;
									}
									break;
							}
						}
						break;
					case BATTLE_MENU_STATE::MOVES:
						{
							usz mv_count = std::max<usz>(1, ally->move_count());
							if (IsKeyPressed(KEY_X)){
								menu_state = BATTLE_MENU_STATE::OPTIONS;
								opt = 0;
							}
							if (IsKeyPressed(KEY_RIGHT)){
								PlaySound(sel_sfx);
								opt = (opt + 1) % mv_count;
							}
							if (IsKeyPressed(KEY_LEFT)){
								PlaySound(sel_sfx);
								opt = (opt == 0) ? mv_count - 1 : opt - 1;
							}
							if (IsKeyPressed(KEY_DOWN)){
								PlaySound(sel_sfx);
								if (opt + 2u < mv_count){
									opt += 2;
								} else {
									opt = ((opt % 2) < mv_count) ? (opt % 2) : 0;
								}
							}
							if (IsKeyPressed(KEY_UP)){
								PlaySound(sel_sfx);
								if (opt >= 2){
									opt -= 2;
								} else {
									opt = ((opt % 2) < mv_count) ? (opt % 2) : mv_count - 1;
								}
							}
							if (IsKeyPressed(KEY_Z) && ally->pp_used()[opt] < ctx.movedex.get_move(ally->moves()[opt]).pp){
								battle.set_action(TURN_ACTION::ATTACK, BATTLE_TURN::TRAINER, opt, 0);
								opt = 0;
							}
						}
						break;
					case BATTLE_MENU_STATE::MODULES:
						break;
					case BATTLE_MENU_STATE::POKEMON:
						for (usz i = 0; i < ctx.player.get_party().size(); i++){
							p_party[i].block_frames(true);
						}
						if (IsKeyPressed(KEY_X) && !switch_necessary){
							next_menu_state = (item_used != 0) ? BATTLE_MENU_STATE::BAG : BATTLE_MENU_STATE::OPTIONS;
							item_used = 0;
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
								if (item_used == 0){
									if (opt != battle.current_trainer_pokemon_idx() && p_party[opt].hp() > 0){
										next_pokemon = opt;
									}
								} else {
									if (can_use_item(ctx.itemdex.get_item(item_used), p_party[opt])){
										battle.set_action(TURN_ACTION::ITEM, BATTLE_TURN::TRAINER, item_used, opt);
										item_used = 0;
										next_menu_state = BATTLE_MENU_STATE::OPTIONS;
									}
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
						{
							if (next_menu_state == BATTLE_MENU_STATE::NONE){
								if (sel_opt == -1){
									ctx.control_selection(opt, 4, sel_sfx, {}, true);
									if (ctx.player.inventory()[opt].size() > 0){
										sel_opt = ctx.control_selection(opt2, ctx.player.inventory()[opt].size() - 1, sel_sfx, dec_sfx);
									}
								} else if (sel_opt2 == -1) {
									sel_opt2 = ctx.control_selection(opt3, 1, sel_sfx, dec_sfx);
								}
								switch (sel_opt2){
									case 0:{
										       const item_t& it = ctx.player.inventory()[opt][sel_opt];
										       if (it.target == ITEM_TARGET::ALLY){
											       item_used = it.id;
											       next_menu_state = BATTLE_MENU_STATE::POKEMON;
										       } else if (it.target == ITEM_TARGET::FOE){
											       battle.set_action(TURN_ACTION::ITEM, BATTLE_TURN::TRAINER, it.id, battle.current_foe_pokemon_idx());
											       next_menu_state = BATTLE_MENU_STATE::OPTIONS;
										       } else {
											       sel_opt2 = -1;
											       opt3 = 0;
										       }
									       }
									       break;
									case 1:
									       sel_opt = -1;
									       opt3 = 0;
									       sel_opt2 = -1;
									       break;
									default: break;
								}
								if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)){
									bag_animation_frame_timer = 0;
									bag_animation_frame = 0;
									bags_y_jump = -20.0f;
									opt2 = 0;
								}
								if (bag_animation_frame < 2){
									bag_animation_frame_timer++;
									if (bag_animation_frame_timer > 2){
										bag_animation_frame++;
										bag_animation_frame_timer = 0;
									}
								}
								if (bags_x < 0.0f) bags_x += 25.0f;
								if (bags_y_jump < 0.0f) bags_y_jump += 1.0f;
								if (IsKeyPressed(KEY_X)){
									if (sel_opt == -1){
										next_menu_state = BATTLE_MENU_STATE::OPTIONS;
									} else {
										sel_opt = -1;
										sel_opt2 = -1;
										opt3 = 0;
									}
								}
							}
						}
						break;
					case BATTLE_MENU_STATE::GIVE_NICKNAME_TO_CAUGHT:
						{
							sel_opt = ctx.control_selection(opt, 1, sel_sfx, dec_sfx);
							if (sel_opt != -1 && !caught_and_pushed){
								if (sel_opt == 0){

								}
								ctx.player.push_pokemon(*foep);
								caught_and_pushed = true;
							}
							if (caught_and_pushed){
								ctx.switch_state(STATE::MAIN);
							}
							return;
						}
						break;
					default: break;
				}
				if (is_foe_trainer && !critical_hp && !IsMusicStreamPlaying(ctx.assets.get_mus("Victory is Right Before Your Eyes!.ogg"))){
					auto& tr = std::get<trainer_t>(ctx.bt_info.foe);
					if (ctx.bt_info.trainer_type == TRAINER_TYPE::GYM_LEADER && tr.conscious_count() == 1){
						ctx.set_music("Victory is Right Before Your Eyes!.ogg");
					}
				}
			}
			break;
		case BATTLE_PHASE::FIGHT:
			if (ally_height < 1.0f) ally_height += 0.1f;
			if (foe_height < 1.0f) foe_height += 0.1f;
			if (writer.finished && foe_hp_render == foep->hp() && ally_hp_render == ally->hp()){
				if (unique_txt_entry != UNIQUE_TXT_ENTRY::NONE){
					battle.set_txtbox_alpha(255);
					if (++unique_txt_timer >= unique_txt_dest){
						unique_txt_entry = UNIQUE_TXT_ENTRY::NONE;
						unique_txt_timer = 0;
						writer.finished = true;
					}
				} else if (auto next = battle.next_turn()){
					const bool is_trainer = (*next == BATTLE_TURN::TRAINER);
					float& attacked_height = !is_trainer ? ally_height : foe_height;
					const pokemon_t& attacked = !is_trainer ? *ally : *foep;
					const pokemon_t& cur = is_trainer ? *ally : *foep;
					const turn_action_t act = is_trainer ? battle.trainer_turn_action() : battle.foe_turn_action();
					const auto* atk = std::get_if<attack_action_t>(&act);
					const auto* itm = std::get_if<item_action_t>(&act);
					if (std::holds_alternative<switch_pokemon_action_t>(act)){
						battle.execute_action(ctx.movedex, ctx.itemdex, ctx.assets);
						txted = false;
						turn_timer = 0;
					} else if (!txted){
						if (cur.hp() > 0){
							if (atk){
								camera_pos = is_trainer ? BATTLE_CAMERA_POS::FOE : BATTLE_CAMERA_POS::ALLY;
								current_line = ctx.formatter.format(
										ctx.locale.get(Locale::TYPE::UI, "PKMN_USED"), 
										cur.get_pokedex().id, 
										ctx.movedex.get_move(cur.moves()[atk->move_slot]).name);
							} else if (itm) {
								current_line = ctx.formatter.format(
										ctx.locale.get(Locale::TYPE::UI, (is_trainer) ? "TRAINER_USED" : "FOE_USED"), 
										MISSINGNO, ctx.itemdex.get_item(itm->item_id).name);

							}
							writer.set_text(current_line);
							txted = true;
							turn_timer = 0;
						} else {
							if (++turn_timer >= turn_timer_dest){
								battle.execute_action(ctx.movedex, ctx.itemdex, ctx.assets);
								turn_timer = 0;
							}
						}
					} else {
						if (++turn_timer >= turn_timer_dest){
							const bool trainer_item = is_trainer && std::holds_alternative<item_action_t>(battle.trainer_turn_action());
							battle.execute_action(ctx.movedex, ctx.itemdex, ctx.assets);
							if (atk){
								attacked_height = 0.6f;
							}
							txted = false;
							turn_timer = 0;
							if (trainer_item){
								const auto kind= battle.last_item_result().kind;
								if (kind == ITEM_RESULT::CAUGHT || kind == ITEM_RESULT::BROKE_FREE){
									set_phase(ctx.assets, BATTLE_PHASE::THROW_POKEBALL);
									trainer_thrown_pokeball_renderer.thrown = false;
									trainer_thrown_pokeball_renderer.opened = false;
									trainer_thrown_pokeball_renderer.spinned = 0;
									trainer_thrown_pokeball_y_vel = 0.05f;
									trainer_thrown_pokeball_y = 0.7f;
									trainer_thrown_pokeball_z = 3.0f;
									catch_shakes = 1;
									break;
								}
							}
							if (attacked.hp() == 0){
								trainer_t* owner = nullptr;
								float& poke_y = is_trainer ?  foe_pokemon_y : ally_pokemon_y;
								if (!is_foe_trainer && is_trainer){
									battle.set_winner(BATTLE_FIGURE::TRAINER);
								} else {
									owner = is_foe_trainer && is_trainer ? &std::get<trainer_t>(ctx.bt_info.foe) : &ctx.player;
									if (owner->conscious_count() > 0){
										owner->set_conscious_count(owner->conscious_count() - 1);
									}
									if (owner->conscious_count() == 0){
										battle.set_winner(is_trainer ? BATTLE_FIGURE::TRAINER : BATTLE_FIGURE::FOE);
									}
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
							unique_txt_entry = UNIQUE_TXT_ENTRY::MISSED;
							unique_txt_timer = 0;
							battle.set_figure_missed(BATTLE_FIGURE::NONE);
						}
						if (battle.is_crit()){
							current_line = ctx.locale.get(Locale::TYPE::UI, "CRIT_HIT");
							writer.set_text(current_line);
							unique_txt_entry = UNIQUE_TXT_ENTRY::CRIT;
							unique_txt_timer = 0;
							battle.set_crit(false);
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
						} else if (ctx.player.conscious_count() > 0){
							// TODO: WILD POKEMON OPTIONAL POKEMON SUBSTITUDE/RUN
							set_phase(ctx.assets, BATTLE_PHASE::MENU);
							menu_state = BATTLE_MENU_STATE::NONE;
							next_menu_state = BATTLE_MENU_STATE::POKEMON;
							if (is_foe_trainer){
								switch_necessary = true;
							}
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
		case BATTLE_PHASE::THROW_POKEBALL:
			{
				camera_pos = BATTLE_CAMERA_POS::FOE;
				if (trainer_thrown_pokeball_y > 0.5f){
					trainer_thrown_pokeball_y_vel -= 0.003f;
					trainer_thrown_pokeball_y += trainer_thrown_pokeball_y_vel;
					trainer_thrown_pokeball_z -= 0.1f;
					trainer_thrown_pokeball_x += 0.03f;
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
					}
				} else if (foe_pokemon_scale > 0.0f){
					trainer_thrown_pokeball_renderer.frame = 10;
					trainer_thrown_pokeball_renderer.frame_timer = 0;
					trainer_thrown_pokeball_renderer.spinned = 0;
					foe_pokemon_scale -= 0.1f;
				} else {
					if (trainer_thrown_pokeball_y > 0.01f) trainer_thrown_pokeball_y-=0.01f;
					if (catch_shakes < battle.last_item_result().shakes){
						trainer_thrown_pokeball_renderer.frame_timer += 1.0f;
						if (trainer_thrown_pokeball_renderer.frame_timer > 3.0f){
							if (trainer_thrown_pokeball_renderer.frame >= 9){
								trainer_thrown_pokeball_renderer.frame++;
								trainer_thrown_pokeball_renderer.frame_timer = 0.0f;
								if (auto bowa = ctx.assets.get_sfx("SEQ_SE_BOWA1.wav"); !IsSoundPlaying(bowa)){
									PlaySound(ctx.assets.get_sfx("SEQ_SE_BOWA1.wav"));
								}
							} else if (trainer_thrown_pokeball_renderer.frame_timer > 20.0f){
								trainer_thrown_pokeball_renderer.frame = 13;
								trainer_thrown_pokeball_renderer.frame_timer = 0.0f;
							}
						}
						if (trainer_thrown_pokeball_renderer.frame > 15){
							trainer_thrown_pokeball_renderer.frame = 0;
							catch_shakes++;
						}
					} else {
						auto res = battle.last_item_result().kind;
						if (res == ITEM_RESULT::CAUGHT){
							ctx.set_music("Caught a Pokemon!.ogg");
							ctx.current_music.looping = false;
							if (!txted){
								current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "CAUGHT_PKMN"), foep->get_pokedex().id);
								writer.set_text(current_line);
								txted = true;
							}
							if (ctx.music_started && !IsMusicStreamPlaying(ctx.current_music)){
								battle.set_winner(BATTLE_FIGURE::TRAINER);
								set_phase(ctx.assets, BATTLE_PHASE::END);
								txted = false;
								ctx.current_music.looping = true;
								sel_opt = -1;
							}
						} else {
							if (foe_pokemon_scale < 1.0f) foe_pokemon_scale+=0.1f;
							else set_phase(ctx.assets, BATTLE_PHASE::FIGHT);
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
					if (!txted){
						StopMusicStream(ctx.current_music);
						ctx.set_music(get_victory_music_name(ctx));
						if ((ctx.predialog_finished && ctx.postdialog_finished) || !is_foe_trainer){
							if (ctx.index < encounters_count[static_cast<usz>(ctx.region)]) ctx.index++;
							ctx.predialog_finished = false;
							ctx.postdialog_finished = false;
						}
						if (is_foe_trainer){
							current_line = ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "FOE_WAS_DEFEATED"));
							writer.set_text(current_line);
						}
						txted = true;
					}
					if (is_foe_trainer){
						if (unique_txt_entry != UNIQUE_TXT_ENTRY::FOE_DEFEAT_MSG){
							if (txted && writer.finished && ++unique_txt_timer > 50){
								current_line = ctx.formatter.format(ctx.bt_info.defeat_msg);
								writer.set_text(current_line);
								unique_txt_entry = UNIQUE_TXT_ENTRY::FOE_DEFEAT_MSG;
								unique_txt_timer = 0;
							}
						} else if (writer.finished){
							if (++unique_txt_timer > 90 || (unique_txt_timer > 20 && IsKeyPressed(KEY_Z))){
								ctx.switch_state(STATE::MAIN);
								if (ctx.music_volume > 0.0f) ctx.music_volume -= 0.05f;
								else StopMusicStream(ctx.current_music);
								return;
							}
						}

					} else {
						if (battle.last_item_result().kind == ITEM_RESULT::CAUGHT){
							set_phase(ctx.assets, BATTLE_PHASE::MENU);
							menu_state = BATTLE_MENU_STATE::NONE;
							next_menu_state = BATTLE_MENU_STATE::GIVE_NICKNAME_TO_CAUGHT;
							return;
						} else {
							ctx.switch_state(STATE::MAIN);
							return;
						}
					}
				} else {
					// TODO: TEXT
					// DECREMENT MONEY
					ctx.initialized_encounter_info = false;
					ctx.switch_state(STATE::MAIN);
					ctx.show_encounter_info = true;
					return;
				}
			}
			break;
	}
	if (!std::holds_alternative<none_action_t>(battle.trainer_turn_action()) &&
			(ally_pokemon_y <= pokemon_y_dest &&
			 ally_pokemon_scale >= pokemon_scale_dest &&
			 next_menu_state == BATTLE_MENU_STATE::NONE
			) && phase != BATTLE_PHASE::FIGHT){
		battle.set_ai_action(ctx.bt_info.items);
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
		if (is_foe_trainer){
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
	Texture2D optb = ctx.assets.get_tx("opt.png");
	if (is_foe_trainer){
		if (std::ranges::contains(std::array{"Cheren", "Bianca", "N"}, ctx.bt_info.name)){
			animation_done = anim_trainer_render.draw(ctx.assets, ctx.bt_info.name, {0.7f, 0.08f, foe_z}, &ctx.camera.camera, {foe_accent_color, foe_accent_color, foe_accent_color, foe_alpha}, foe_accent_color == 255);
		} else {
			draw_trainer(ctx.assets, ctx.bt_info.trainer_type, ctx.bt_info.name, {0.5f, 0.08f, foe_z}, &ctx.camera.camera, {foe_accent_color, foe_accent_color, foe_accent_color, foe_alpha});
			animation_timer ++;
			if (animation_timer > 125){
				animation_done = true;
			}
		}
	} else {
		animation_done = true;
	}
	auto& ally = p_party[battle.current_trainer_pokemon_idx()];
	if (phase >= BATTLE_PHASE::FOE_SEND_OUT){
		foep.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::FRONT, {0.6f, foe_pokemon_y, -0.3f}, {foe_pokemon_scale, foe_pokemon_scale * foe_height}, &ctx.camera.camera);
		if (player_back.draw_back(ctx.assets, (ctx.player.sex() == SEX::MALE) ? "hilbert" : "hilda", {-6.0f, -1.0f, 3.5f}, ctx.camera.camera, {255, 255, 255, trainer_alpha})) finished_drawing_back = true;
		if (is_foe_trainer && foe_thrown_pokeball_renderer.spinned < 3){
			foe_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {0.7f, 0.5f, -0.5f}, ctx.camera.camera);
		}
	}
	if (phase >= BATTLE_PHASE::ALLY_SEND_OUT || foe_about_to_send){
		ally.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::BACK, {-0.4f, ally_pokemon_y, 2.0f}, {ally_pokemon_scale, ally_pokemon_scale * ally_height}, &ctx.camera.camera);
	}
	if (phase == BATTLE_PHASE::ALLY_SEND_OUT){
		if (finished_drawing_back && trainer_thrown_pokeball_renderer.spinned < 3){
			trainer_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {-0.5f, trainer_thrown_pokeball_y, trainer_thrown_pokeball_z}, ctx.camera.camera);
		}
	}
	if (phase == BATTLE_PHASE::THROW_POKEBALL){
		trainer_thrown_pokeball_renderer.draw(ctx.assets.get_tx("pokeballs.png"), POKEBALL::POKE_BALL, {trainer_thrown_pokeball_x, trainer_thrown_pokeball_y, trainer_thrown_pokeball_z}, ctx.camera.camera);
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
	if (is_foe_trainer && phase < BATTLE_PHASE::FOE_SEND_OUT){
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
			auto [ptr, ec] = std::to_chars(lvl_buf, lvl_buf + sizeof(lvl_buf), foep.lvl());
			std::string_view lvl_str(lvl_buf, ptr - lvl_buf);
			draw_pokename(ctx.assets, lvl_buf, foe_hud_x + 450, 295.0f);
			if (foep.sex() != SEX::NONE){
				DrawTexturePro(ctx.assets.get_tx("sex-sign.png"), {static_cast<float>(foep.sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {foe_hud_x + 385.0f, 295.0f, 28.0f, 40.0f}, {0, 0}, 0, WHITE);
			}
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
			if (ally.sex() != SEX::NONE){
				DrawTexturePro(ctx.assets.get_tx("sex-sign.png"), {static_cast<float>(ally.sex()) * 7.0f, 0.0f, 7.0f, 10.0f}, {ally_hud_x + 365.0f, 535.0f, 28.0f, 40.0f}, {0, 0}, 0, WHITE);
			}
			draw_pokename(ctx.assets, {hp_buf, hp_ptr}, ally_hud_x + 330, 600.0f);
			draw_pokename(ctx.assets, {max_hp_buf, max_hp_ptr}, ally_hud_x + 420, 600.0f);
			DrawRectangle(ally_hud_x + 160, 655.0f, (static_cast<float>(ally.exp()) / ally.exp_cap()) * 400.0f, 5, {0, 153, 148, 255});
		}
	}
	if (phase == BATTLE_PHASE::MENU){
		if (!switch_necessary && menu_state != BATTLE_MENU_STATE::NONE){
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
			case BATTLE_MENU_STATE::BAG:
				{
					DrawTextureEx(ctx.assets.get_tx("bag-bg.png"), {0, 0}, 0, 3.9f, WHITE);
					DrawTexturePro(ctx.assets.get_tx("bag.png"), {opt == 0 ? bag_animation_frame * 60.0f : 0, 0, 60, 63}, {0.0f + bags_x, 200.0f + (opt == 0 ? bags_y_jump : 0), 420.0f, 441.0f}, {0, 0}, 0.0f, WHITE);
					DrawTexturePro(ctx.assets.get_tx("key-bag.png"), {opt == 1 ? bag_animation_frame * 45.0f : 0, 0, 45, 48}, {400.0f + bags_x, 300.0f + (opt == 1 ? bags_y_jump : 0), 315.0f, 336.0f}, {0, 0}, 0.0f, WHITE);
					DrawTexturePro(ctx.assets.get_tx("berries-bag.png"), {opt == 2 ? bag_animation_frame * 38.0f : 0, 0, 38, 37}, {600.0f + bags_x, 500.0f + (opt == 2 ? bags_y_jump : 0), 266.0f, 259.0f}, {0, 0}, 0.0f, WHITE);
					DrawTexturePro(ctx.assets.get_tx("tm-bag.png"), {opt == 3 ? bag_animation_frame * 42.0f : 0, 0, 42, 45}, {300.0f + bags_x, 600.0f + (opt == 3 ? bags_y_jump : 0), 294.0f, 315.0f}, {0, 0}, 0.0f, WHITE);
					DrawTexturePro(ctx.assets.get_tx("medicine-bag.png"), {opt == 4 ? bag_animation_frame * 38.0f : 0, 0, 38, 49}, {0.0f + bags_x, 600.0f + (opt == 4 ? bags_y_jump : 0), 266.0f, 343.0f}, {0, 0}, 0.0f, WHITE);
					DrawTexturePro(ctx.assets.get_tx("bar.png"), {0, 0, 256, 24}, {0, wnd_h - 72, wnd_w, 72}, {0, 0}, 0, WHITE);
					draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, bag_labels[opt]), 50, wnd_h - 50, 50, WHITE);
					for (usz i = 0; i < ctx.player.inventory()[opt].size(); i++){
						DrawTexturePro(optb, {0, (opt2 == i) * 24.0f, 70.0f, 24.0f}, {1400, 100 + i * (optb.height * 2.5f), optb.width * 5.0f, optb.height * 2.5f}, {0, 0}, 0, WHITE);
						draw_text(ctx.assets, ctx.player.inventory()[opt][i].name, 1425, 130 + i * (optb.height * 2.5f), 50, WHITE);
					}
					if (ctx.player.inventory()[opt].size() > opt2){
						draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, ctx.player.inventory()[opt][opt2].name), 0, 0, 50, WHITE);
					}
					if (sel_opt != -1){
						DrawRectangle(0, 0, wnd_w, wnd_h, {0, 0, 0, 150});
						draw_text(ctx.assets, std::format("{} is selected.", ctx.player.inventory()[opt][sel_opt].name), 15, 15, 50, WHITE, TXT_SHADOW_COLOR, 0, 1);
						for (usz i = 0; i < 2; i++){
							DrawTexturePro(optb, {0, (opt3 == i) * 24.0f, 70.0f, 24.0f}, {1500, 600 + i * (optb.height * 2.5f), optb.width * 5.0f, optb.height * 2.5f}, {0, 0}, 0, WHITE);
							draw_text(ctx.assets, item_act_labels[i], 1525, 630 + i * (optb.height * 2.5f), 50, WHITE);
						}
					}
					// TODO: SCROLLING ITEMS
				}
				break;
			case BATTLE_MENU_STATE::GIVE_NICKNAME_TO_CAUGHT:
				{
					DrawTextureEx(ctx.assets.get_tx("give-nickname-to-caught-bg.png"), {0, 0}, 0, 2.4f, WHITE);
					draw_text(ctx.assets, ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "GIVE_NICKNAME_TO_CAUGHT"), foep.get_pokedex().id), 10, wnd_h - 200, 50, WHITE, TXT_SHADOW_COLOR, 0, 1);
					foep.block_frames(true);
					foep.render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::FRONT, {wnd_w / 2.0f - 128, wnd_h / 2.0f - 128, 0.0f}, {256, 256});
					for (usz i = 0; i < 2; i++){
						DrawTexturePro(optb, {0, (opt == i) * 24.0f, 70.0f, 24.0f}, {wnd_w - 300, wnd_h - 410 + i * (optb.height * 2.0f), optb.width * 4.0f, optb.height * 2.0f}, {0, 0}, 0, WHITE);
						draw_text(ctx.assets, yes_no[i], wnd_w - 300 + 30.0f, wnd_h - 410 + i * (optb.height * 2.0f) + 15.0f, 60, WHITE);
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
				DrawTexturePro(optb, {0, (opt == i) * 24.0f, 70.0f, 24.0f}, {wnd_w - 300, wnd_h - 410 + i * (optb.height * 2.0f), optb.width * 4.0f, optb.height * 2.0f}, {0, 0}, 0, WHITE);
				draw_text(ctx.assets, yes_no[i], wnd_w - 300 + 30.0f, wnd_h - 410 + i * (optb.height * 2.0f) + 15.0f, 60, WHITE);
			}
		}
	}
}

