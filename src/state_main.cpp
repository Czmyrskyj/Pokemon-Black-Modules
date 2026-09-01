#include "st-ctx.hpp"
#include "time.hpp"
#include "random.hpp"
#include "formatter.hpp"

#include <print>
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
	bt_info = ctx.battle_info.get_battle_info(ctx.region, index);
	std::vector<std::string> for_pre;
	std::vector<std::string> for_post;
	for (const auto& d : bt_info.dialog){
		if (d.ends_with("A")){
			for_post.push_back(d);
		} else {
			for_pre.push_back(d);
		}
	}
	predialog.init(for_pre);
	postdialog.init(for_post);

	current_line = ctx.formatter.format(predialog.step(ctx.assets, writer, ctx.locale));
	writer.set_text(current_line);
}

void main_state_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
		ctx.halt_music = false;
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	auto season = ctx.time.get_season();
	auto hours = ctx.time.get_time().hours().count();
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
	for (usz i = 0; i < clouds.size(); i++){
		auto& c = clouds[i];
		c.pos.x -= 0.05f + i * 0.05f;
		if (c.pos.x <= -100.0f * c.scale){
			c.pos.x = wnd_w + rnd::get_val<float>(0.0f, 500.0f);
		}
	}
	if (predialog.update(ctx.assets, writer) && writer.finished && !predialog.finished){
		current_line = ctx.formatter.format(predialog.step(ctx.assets, writer, ctx.locale));
		writer.set_text(current_line);
	}
	if (predialog.finished) {
		allow_fight = true;
		if (ended_battle){
			if (!initialized_postdialog){
				current_line = ctx.formatter.format(postdialog.step(ctx.assets, writer, ctx.locale));
				writer.set_text(current_line);
				initialized_postdialog = true;
			}
			if (postdialog.update(ctx.assets, writer) && writer.finished){
				current_line = ctx.formatter.format(postdialog.step(ctx.assets, writer, ctx.locale));
				writer.set_text(current_line);
			}
		}
	}
	if (allow_fight && predialog.hid_txtbox()){
		if (!initialized_meet_msg){
			writer.set_text(ctx.formatter.format(bt_info.meet_msg));
			initialized_meet_msg = true;
		}
		ctx.current_music = ctx.assets.get_mus(
				std::format(
					"{}.ogg",
					[&]() -> std::string{
					if (bt_info.prefix.empty()){
					return bt_info.name;
					}
					return "Trainer";
					}()
					)
				);
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
	DrawTexture(ctx.assets.get_tx(std::format("{}.png", get_region_str(ctx.region))), 0, 0, WHITE);
	DrawTexturePro(ctx.assets.get_tx("bar.png"), {0, 0, 256, 24}, {0, 0, 1920, 72}, {0, 0}, 0, WHITE);
	for (usz i = 0; i < encounters_count[static_cast<u8>(ctx.region)]; i++){
		DrawTextureEx(ctx.assets.get_tx("level-point.png"), {100 + i * 400.0f, 600.0f}, 0.0f, 2.0f, (i == index) ? WHITE : DARKGRAY);
	}
	DrawTextureEx(ctx.assets.get_tx("level-point-outline.png"), {100 + index * 400.0f - 15.0f, 585.0f}, 0.0f, 3.0f, WHITE);
	if (!predialog.finished || !predialog.hid_txtbox()){
		draw_text_seq(ctx.assets, writer, predialog.txtbox_x, predialog.txtbox_y, 50, WHITE, 1);
		predialog.draw_speaker(ctx.assets);
	} 
	if (!postdialog.finished || !postdialog.hid_txtbox()){
		draw_text_seq(ctx.assets, writer, postdialog.txtbox_x, postdialog.txtbox_y, 50, WHITE, 1);
		postdialog.draw_speaker(ctx.assets);
	}
	if (allow_fight && predialog.hid_txtbox()){
		if (encounter_info_txtbox_y > wnd_h - 200.0f){
			encounter_info_txtbox_y -= 10.0f;
		}
		DrawTexture(ctx.assets.get_tx("txtbox1.png"), 0, encounter_info_txtbox_y, WHITE);
		draw_text(ctx.assets, bt_info.name, 200, encounter_info_txtbox_y + 20.0f, 50, WHITE);
		draw_text_seq(ctx.assets, writer, 200, encounter_info_txtbox_y + 80.0f, 50, WHITE);
		draw_text(ctx.assets, std::format("Reward: {}[POKEDOLLAR]", bt_info.reward), 200, encounter_info_txtbox_y + 140.0f, 50, WHITE);
	}
	ctx.bianca.get_party().data()[0].render(ctx.assets, POKEMON_RENDER_TYPE::MENU, {1000, encounter_info_txtbox_y + 50, 160, 120});
}
