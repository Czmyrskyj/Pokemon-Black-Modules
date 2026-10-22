#include "st-ctx.hpp"
#include "time.hpp"
#include "random.hpp"
#include "formatter.hpp"
#include "trainer.hpp"
#include <charconv>
#include <algorithm>
#include <cmath>

constexpr float celestial_orbit{wnd_w * 0.5f};

main_state_t::main_state_t(st_ctx_t& ctx){
	for (usz i = 0; i < 20; i++){
		clouds.push_back(
				{
				.pos = {
				rnd::get_val<float>(0.0f, wnd_w),
				rnd::get_val<float>(0.0f, wnd_h / 2.0f),	
				},
				.scale = rnd::get_val<float>(2.0f, 8.0f),
				}
				);
	}
	ctx.bt_info = ctx.battle_info.get_battle_info(ctx.region, ctx.index, ctx.pokedex);
	is_foe_trainer = std::holds_alternative<trainer_t>(ctx.bt_info.foe);
	ctx.music_volume = 1.0f;
	std::string_view region_music = [&]() -> std::string_view{
		auto reg_str = get_region_str(ctx.region);
		if (reg_str.ends_with("Gym")){
			return "Gym";
		}
		return reg_str;
	}();
	ctx.set_music(std::format("{}.ogg", region_music));
	if (is_foe_trainer){
		for (const auto& d : ctx.bt_info.dialog){
			if (d.ends_with("A")){
				for_post.push_back(d);
			} else {
				for_pre.push_back(d);
			}
		}
		postdialog.init(for_post);

		if (!for_pre.empty()){
			predialog.init(for_pre);
			predialog.change_music_if_char(ctx);
			current_line = ctx.formatter.format(predialog.step(ctx.assets, writer, ctx.locale));
			writer.set_text(current_line);
		} else {
			ctx.predialog_finished = true;
			ctx.show_encounter_info = true;
			allow_fight = true;
		}
		SetMusicVolume(ctx.current_music, ctx.music_volume);
	} else {
	}
}

auto get_time_palette = [](auto season, int hours, Color& sky_color, st_ctx_t& ctx) -> void{
	switch (season) {
		case Time::SEASON::SPRING: {
						   if (hours >= 5 && hours <= 9) {
							   sky_color    = {163, 201, 226, 255};
							   ctx.bg_color = {255, 229, 180, 255};
						   } else if (hours >= 10 && hours <= 16) {
							   sky_color    = {74, 144, 226, 255};
							   ctx.bg_color = {185, 227, 254, 255};
						   } else if (hours >= 17 && hours <= 19) {
							   sky_color    = {92, 84, 112, 255};
							   ctx.bg_color = {255, 179, 133, 255};
						   } else if (hours >= 20 || hours <= 4) {
							   sky_color    = {16, 25, 53, 255};
							   ctx.bg_color = {43, 58, 103, 255};
						   }
					   } break;

		case Time::SEASON::SUMMER: {
						   if (hours >= 4 && hours <= 8) {
							   sky_color    = {124, 185, 232, 255};
							   ctx.bg_color = {255, 209, 102, 255};
						   } else if (hours >= 9 && hours <= 18) {
							   sky_color    = {30, 96, 145, 255};
							   ctx.bg_color = {78, 168, 222, 255};
						   } else if (hours >= 19 && hours <= 20) {
							   sky_color    = {74, 21, 75, 255};
							   ctx.bg_color = {255, 158, 0, 255};
						   } else if (hours >= 21 || hours <= 3) {
							   sky_color    = {11, 9, 26, 255};
							   ctx.bg_color = {26, 27, 65, 255};
						   }
					   } break;

		case Time::SEASON::AUTUMN: {
						   if (hours >= 5 && hours <= 9) {
							   sky_color    = {155, 190, 200, 255};
							   ctx.bg_color = {226, 176, 129, 255};
						   } else if (hours >= 10 && hours <= 16) {
							   sky_color    = {59, 122, 153, 255};
							   ctx.bg_color = {216, 196, 152, 255};
						   } else if (hours >= 17 && hours <= 19) {
							   sky_color    = {88, 24, 69, 255};
							   ctx.bg_color = {255, 87, 51, 255};
						   } else if (hours >= 20 || hours <= 4) {
							   sky_color    = {24, 18, 43, 255};
							   ctx.bg_color = {57, 48, 83, 255};
						   }
					   } break;

		case Time::SEASON::WINTER: {
						   if (hours >= 7 && hours <= 10) {
							   sky_color    = {192, 192, 192, 255};
							   ctx.bg_color = {243, 213, 192, 255};
						   } else if (hours >= 11 && hours <= 15) {
							   sky_color    = {112, 146, 165, 255};
							   ctx.bg_color = {220, 229, 231, 255};
						   } else if (hours >= 16 && hours <= 18) {
							   sky_color    = {75, 67, 83, 255};
							   ctx.bg_color = {220, 214, 205, 255};
						   } else if (hours >= 19 || hours <= 6) {
							   sky_color    = {15, 23, 42, 255};
							   ctx.bg_color = {30, 41, 59, 255};
						   }
					   } break;
	}
};

void main_state_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	if (ctx.set_state_meta && ctx.transition_alpha >= 255.0f){
		ctx.show_encounter_info = false;
	}

	auto season = ctx.time.get_season();
	auto hours = ctx.time.get_time().hours().count();
	get_time_palette(season, hours, sky_color, ctx);
	for (usz i = 0; i < clouds.size(); i++){
		auto& c = clouds[i];
		c.pos.x -= 0.05f + i * 0.05f;
		if (c.pos.x <= -100.0f * c.scale){
			c.pos.x = wnd_w + rnd::get_val<float>(0.0f, 500.0f);
		}
	}
	if (started_battle){
		ctx.predialog_finished = predialog.finished;
		ctx.switch_state(STATE::BATTLE_VS);
		SetMusicVolume(ctx.current_music, ctx.music_volume);
		if (ctx.music_volume > 0.0f){
			ctx.music_volume -= GetFrameTime()*1.5f;
		}
		return;
	}
	if ((!ctx.predialog_finished && allow_fight && predialog.hid_txtbox()) || ctx.show_encounter_info){
		if (!ctx.initialized_encounter_info){
			std::string reward_string(5, '\0');
			std::to_chars(reward_string.data(), reward_string.data() + reward_string.size(), ctx.bt_info.reward);
			if (usz pos = reward_string.find('\0'); pos != std::string::npos){
				reward_string = reward_string.substr(0, pos);
			}
			current_line = std::format("{}\n{}\n{}", is_foe_trainer ? ctx.bt_info.name : "Wild Pokémon", ctx.formatter.format(ctx.bt_info.meet_msg), ctx.formatter.format(ctx.locale.get(Locale::TYPE::UI, "ENCOUNTER_REWARD"), MISSINGNO, reward_string));
			writer.set_text(current_line);
			ctx.initialized_encounter_info = true;
		}
		if (is_foe_trainer){
			ctx.set_music(
					std::format(
						"{}.ogg",
						[&]() -> std::string{
						auto tt = ctx.bt_info.trainer_type;
						switch (tt){
						case TRAINER_TYPE::PKMN_TRAINER: return ctx.bt_info.name;
						case TRAINER_TYPE::GYM_LEADER: return "Gym";
						case TRAINER_TYPE::TEAM_PLASMA_GRUNT: return "Team Plasma Appears!";
						default: return std::string(trainer_type_prefixes[static_cast<u8>(ctx.bt_info.trainer_type)]);
						}
						}()
						)
				     );
		}
		if (IsKeyPressed(KEY_Z) && writer.finished){
			started_battle = true;
		}
	}
	if (!ctx.predialog_finished){
		if (!is_foe_trainer){
			allow_fight = true;
		} else {
			if (predialog.update(ctx.assets, writer) && writer.finished && !predialog.finished){
				current_line = ctx.formatter.format(predialog.step(ctx.assets, writer, ctx.locale));
				writer.set_text(current_line);
			}
			if (predialog.finished && predialog.hid_txtbox()) {
				allow_fight = true;
			}
		}

		ctx.postdialog_finished = for_post.empty() ? true : postdialog.finished;
	} else if (!ctx.postdialog_finished && !ctx.show_encounter_info){
		if (!initialized_postdialog){
			current_line = ctx.formatter.format(postdialog.step(ctx.assets, writer, ctx.locale));
			writer.set_text(current_line);
			initialized_postdialog = true;
		}
		if (postdialog.update(ctx.assets, writer) && writer.finished){
			current_line = ctx.formatter.format(postdialog.step(ctx.assets, writer, ctx.locale));
			writer.set_text(current_line);
		}
		if (writer.finished){
			ctx.postdialog_finished = true;
			if (ctx.index < encounters_count[static_cast<usz>(ctx.region)]) ctx.index++;
		}
	}
	writer.update();
}

void main_state_t::render(st_ctx_t& ctx){
	DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), sky_color, ctx.bg_color);
	auto t = ctx.time.get_time();
	float celestial_time = static_cast<float>(t.hours().count()) +
		static_cast<float>(t.minutes().count() / 60.0f) +
		static_cast<float>(t.seconds().count() / 3600.0f);
	Vector2 orbit_center = {wnd_w / 2.0, wnd_h / 2.0f};
	double angle = (static_cast<double>(celestial_time) / 24.0) * 2 * M_PI + (M_PI / 2.0);
	celestial_pos.x = orbit_center.x + std::cosf(angle) * (celestial_orbit * 0.8f);
	celestial_pos.y = orbit_center.y + std::sinf(angle) * (celestial_orbit * 0.5f);
	Vector2 celestial_render_pos = {celestial_pos.x - 25 * 3.0f, celestial_pos.y - 25 * 3.0f};
	DrawTextureEx(ctx.assets.get_tx("sun.png"), celestial_render_pos, 0.0f, 3.0f, WHITE);
	for (usz i = 0; i < clouds.size(); i++){
		auto& c = clouds[i];
		DrawTextureEx(ctx.assets.get_tx("cloud.png"), c.pos, 0.0f, c.scale, {255, 255, 255, static_cast<u8>(100 + (i * 5))});
	}
	DrawTextureEx(ctx.assets.get_tx(std::format("{}.png", get_region_str(ctx.region))), {0, 0}, 0, 2.4, WHITE);
	DrawTexturePro(ctx.assets.get_tx("bar.png"), {0, 0, 256, 24}, {0, 0, 1920, 72}, {0, 0}, 0, WHITE);
	for (usz i = 0; i < encounters_count[static_cast<u8>(ctx.region)]; i++){
		DrawTextureEx(ctx.assets.get_tx("level-point.png"), {100 + i * 400.0f, 600.0f}, 0.0f, 2.0f, (i == ctx.index) ? WHITE : DARKGRAY);
	}
	DrawTextureEx(ctx.assets.get_tx("level-point-outline.png"), {100 + ctx.index * 400.0f - 15.0f, 585.0f}, 0.0f, 3.0f, WHITE);
	if (!predialog.finished || !predialog.hid_txtbox()){
		draw_text_seq(ctx.assets, writer, predialog.txtbox_x, predialog.txtbox_y, 50, WHITE, 1);
		predialog.draw_speaker(ctx.assets);
	} 
	if (!postdialog.finished || !postdialog.hid_txtbox()){
		draw_text_seq(ctx.assets, writer, postdialog.txtbox_x, postdialog.txtbox_y, 50, WHITE, 1);
		postdialog.draw_speaker(ctx.assets);
	}
	if (is_foe_trainer){
		if ((allow_fight && predialog.hid_txtbox()) || ctx.show_encounter_info){
			if (encounter_info_txtbox_y >= wnd_h - 200){
				encounter_info_txtbox_y -= 10;
			}
			draw_text_seq(ctx.assets, writer, 200, encounter_info_txtbox_y, 50, WHITE, 1);
			auto& party = (!ctx.bt_info.party.empty()) ? ctx.bt_info.party :
				(ctx.bt_info.name == "Cheren") ? ctx.cheren.get_party() :
				ctx.bianca.get_party();
			for (usz i = 0; i < party.size(); i++){
				party[i].render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::MENU, {1000 + i * 80.0f, encounter_info_txtbox_y - 20.0f, 0}, {242, 200});
			}
			if (std::ranges::contains(std::array{"Cheren", "Bianca", "N"}, ctx.bt_info.name)){
				anim_trainer_render.draw(ctx.assets, ctx.bt_info.name, {10, encounter_info_txtbox_y - 20, 0.0f});
			} else {
				draw_trainer(ctx.assets, ctx.bt_info.trainer_type, ctx.bt_info.name, {10, encounter_info_txtbox_y - 120, 0.0f});
			}
			draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "FIGHT"), 1700, encounter_info_txtbox_y + 100, 50, WHITE);
		}
	} else {
		// ISSUE: MAIN PORTRAIT UPDATES TOO FAST BECAUSE OF MENU PORTRAITS;
		// FIX: SEPARATE UPDATE FROM RENDER [WILL AFFECT BATTLE RENDER TOO];
		if ((allow_fight && predialog.hid_txtbox()) || ctx.show_encounter_info){
			wild_pokemon_render_id_timer++;
			if (encounter_info_txtbox_y >= wnd_h - 200){
				encounter_info_txtbox_y -= 10;
			}
			draw_text_seq(ctx.assets, writer, 200, encounter_info_txtbox_y, 50, WHITE, 1);
			draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "FIGHT"), 1700, encounter_info_txtbox_y + 100, 50, WHITE);
			auto& wild_range = ctx.bt_info.party;
			for (usz i = 0; i < wild_range.size(); i++){
				wild_range[i].render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::MENU, {1000 + i * 80.0f, encounter_info_txtbox_y - 20.0f, 0}, {242, 200});
			}
			if (wild_pokemon_render_id_timer > 10){
				wild_pokemon_render_id++;
				wild_pokemon_render_id_timer = 0;
			}
			if (wild_pokemon_render_id > wild_range.size() - 1) wild_pokemon_render_id = 0;
			wild_range[wild_pokemon_render_id].render(ctx.assets, ctx.spritedex, POKEMON_RENDER_TYPE::FRONT, {10, encounter_info_txtbox_y, 0}, {150, 150});
		}
	}

}
