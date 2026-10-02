#include "st-ctx.hpp"
#include "writer.hpp"
#include "formatter.hpp"
#include <cmath>

whats_your_name_t::whats_your_name_t(st_ctx_t& ctx){
	formatted_prompt = ctx.locale.get(Locale::TYPE::DIALOG, "PROMPT_NAME");
	writer.set_text(formatted_prompt, 0.03f);
}
void whats_your_name_t::update(st_ctx_t& ctx){
	writer.update();
	if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
	Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
	Sound can_sfx = ctx.assets.get_sfx("SEQ_SE_CANCEL3.wav");
	Sound bep_sfx = ctx.assets.get_sfx("SEQ_SE_BEEP.wav");
	Sound dec2_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE2.wav");

	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	if (ctx.player.name() == "nodata" && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT))){
		char_cursor_blinker = 0;
		PlaySound(sel_sfx);
	}
	if (IsKeyPressed(KEY_Z)){
		if (selected_char < 70){
			if (entered_name.size() < 7){
				entered_name += name_entry_chars[selected_char];
				PlaySound(dec_sfx);
			} else {
				PlaySound(bep_sfx);
			}
		} else {
			if (selected_char == 70){
				if (entered_name.size() < 7){
					entered_name += ' ';
					PlaySound(dec_sfx);
				} else {
					PlaySound(bep_sfx);
				}
			} else if (selected_char == 74 && (!entered_name.empty() && entered_name != "nodata" && ctx.player.name() == "nodata")){
				ctx.player.set_name(entered_name);
				ctx.formatter.feed("[PLAYER]", std::string(ctx.player.name()));
				PlaySound(dec2_sfx);
			}
		}
	}

	if (IsKeyPressed(KEY_X)){
		if (!entered_name.empty() && ctx.player.name() == "nodata"){
			entered_name.pop_back();
			PlaySound(can_sfx);
		} else {
			PlaySound(bep_sfx);
		}
	}


	char_cursor_blinker++;
	if (char_cursor_blinker > 40) char_cursor_blinker = 0;
	screen_x += 1.0f;
	if (screen_x >= 1416) screen_x = 0;


	if (ctx.player.name() == "nodata"){
		if (IsKeyPressed(KEY_DOWN)){
			if (selected_char < 52){
				if (selected_char < 52){
					selected_char += 13;
				} else {
					selected_char %= 13;
				}
			} else {
				selected_char = 70;
			}
		} else if (IsKeyPressed(KEY_UP)){
			if (selected_char > 12){
				selected_char -= 13;
			} else {
				selected_char = 52 + (selected_char % 13);
			}
		} else if (IsKeyPressed(KEY_LEFT)){
			if (selected_char < 70){
				if (selected_char > 0){
					selected_char--;
				} else {
					selected_char = 64;
				}
			} else {
				if (selected_char == 74) selected_char = 70;
				else selected_char = 74;
			}
		} else if (IsKeyPressed(KEY_RIGHT)){
			if (selected_char < 70){
				if (selected_char < 64){
					selected_char++;
				} else {
					selected_char = 0;
				}
			} else {
				if (selected_char == 70) selected_char = 74;
				else selected_char = 70;
			}
		}
	}

	SetMusicVolume(ctx.current_music, ctx.music_volume);
	if (ctx.player.name() != "nodata"){
		ctx.switch_state(STATE::JUNIPER_LETTER, 100);
		if (ctx.music_volume > 0.0f){
			ctx.music_volume -= GetFrameTime()*0.5f;
		}
		return;
	}

}

void whats_your_name_t::render(st_ctx_t& ctx){
	float wave_pos = std::sinf(GetTime()) * 10;
	for (int i = -1; i < 2; i++){
		DrawTexturePro(ctx.assets.get_tx("your-name-bg.png"), {0, 0, 257, 194}, {i * 1416.0f + screen_x, 0, 1416, 1080}, {0, 0}, 0, WHITE);
	}
	draw_text_seq(ctx.assets, writer, 10, 1000, 50, WHITE, 1);
	DrawTextureEx(ctx.assets.get_tx("text-entry.png"), {300, 20 + wave_pos}, 0, 5, WHITE);
	draw_text(ctx.assets, entered_name, 710, 80 + wave_pos, 80, WHITE, {50, 50, 50, 255}, 70);
	ctx.player.draw_overworld_preview(ctx.assets, {325, 30 + wave_pos});
	auto char_cursor = ctx.assets.get_tx(selected_char < 70 ? "char-select.png" : "char-select-wide.png");
	if (char_cursor_blinker < 20){
		DrawTextureEx(char_cursor, {408.0f + ((selected_char % 13) * 80), 200.0f + ((static_cast<u8>(selected_char / 13)) * 120) + wave_pos}, 0, 5.1f, WHITE);
	}
}

void whats_your_name_t::clear(assets_t& assets){
	for (const auto& i : {"hilbert.png", "hilda.png"}){
		assets.unload_texture(i);
	}
	assets.unload_track("Abandoned Lof of Dreams.ogg");
}
