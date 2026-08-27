#include "state_whats_your_name.hpp"
#include "st-ctx.hpp"
#include "writer.hpp"
#include "formatter.hpp"
#include "switch_state.hpp"
#include <memory>
#include <cmath>

namespace WhatsYourName{

	struct state_meta_t{
		u8 selected_char = 0;
		u8 char_cursor_blinker = 0;
		std::string entered_name = "";
		float screen_x = 0.0f;
		const std::string_view name_entry_chars{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789., "};
		float music_volume = 1.0f;
		writer_t writer;
		std::string formatted_prompt;	
	};

	static std::unique_ptr<state_meta_t> meta;

	void init(st_ctx_t& ctx){
		if (!meta){
			meta = std::make_unique<state_meta_t>();
		}
		meta->formatted_prompt = fmt::format(ctx.locale.get(Locale::TYPE::DIALOG, "PROMPT_NAME"));
		meta->writer.set_text(meta->formatted_prompt, 0.03f);
	}
	void update(st_ctx_t& ctx){
		if (!meta){
			init(ctx);
		}
		meta->writer.update();
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
		if (ctx.player.get_name() == "nodata" && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT))){
			meta->char_cursor_blinker = 0;
			PlaySound(sel_sfx);
		}
		if (IsKeyPressed(KEY_Z)){
			if (meta->selected_char < 70){
				if (meta->entered_name.size() < 7){
					meta->entered_name += meta->name_entry_chars[meta->selected_char];
					PlaySound(dec_sfx);
				} else {
					PlaySound(bep_sfx);
				}
			} else {
				if (meta->selected_char == 70){
					if (meta->entered_name.size() < 7){
						meta->entered_name += ' ';
						PlaySound(dec_sfx);
					} else {
						PlaySound(bep_sfx);
					}
				} else if (meta->selected_char == 74 && (!meta->entered_name.empty() && meta->entered_name != "nodata" && ctx.player.get_name() == "nodata")){
					ctx.player.set_name(meta->entered_name);
					PlaySound(dec2_sfx);
				}
			}
		}

		if (ctx.player.get_name() != "nodata"){
			switch_state(ctx, STATE::JUNIPER_LETTER, 100);
			if (!meta) return;
			if (meta->music_volume > 0.0f){
				meta->music_volume -= GetFrameTime()*0.5f;
			}
		}

		if (IsKeyPressed(KEY_X)){
			if (!meta->entered_name.empty() && ctx.player.get_name() == "nodata"){
				meta->entered_name.pop_back();
				PlaySound(can_sfx);
			} else {
				PlaySound(bep_sfx);
			}
		}


		meta->char_cursor_blinker++;
		if (meta->char_cursor_blinker > 40) meta->char_cursor_blinker = 0;
		meta->screen_x += 1.0f;
		if (meta->screen_x >= 1416) meta->screen_x = 0;


		if (ctx.player.get_name() == "nodata"){
			if (IsKeyPressed(KEY_DOWN)){
				if (meta->selected_char < 52){
					if (meta->selected_char < 52){
						meta->selected_char += 13;
					} else {
						meta->selected_char %= 13;
					}
				} else {
					meta->selected_char = 70;
				}
			} else if (IsKeyPressed(KEY_UP)){
				if (meta->selected_char > 12){
					meta->selected_char -= 13;
				} else {
					meta->selected_char = 52 + (meta->selected_char % 13);
				}
			} else if (IsKeyPressed(KEY_LEFT)){
				if (meta->selected_char < 70){
					if (meta->selected_char > 0){
						meta->selected_char--;
					} else {
						meta->selected_char = 64;
					}
				} else {
					if (meta->selected_char == 74) meta->selected_char = 70;
					else meta->selected_char = 74;
				}
			} else if (IsKeyPressed(KEY_RIGHT)){
				if (meta->selected_char < 70){
					if (meta->selected_char < 64){
						meta->selected_char++;
					} else {
						meta->selected_char = 0;
					}
				} else {
					if (meta->selected_char == 70) meta->selected_char = 74;
					else meta->selected_char = 70;
				}
			}
		}

		SetMusicVolume(ctx.current_music, meta->music_volume);

	}

	void render(st_ctx_t& ctx){
		if (!meta){
			return;
		}
		float wave_pos = std::sinf(GetTime()) * 10;
		for (int i = -1; i < 2; i++){
			DrawTexturePro(ctx.assets.get_tx("your-name-bg.png"), {0, 0, 257, 194}, {i * 1416.0f + meta->screen_x, 0, 1416, 1080}, {0, 0}, 0, WHITE);
		}
		draw_text_seq(ctx.assets, meta->writer, 10, 1000, 50, WHITE, 1);
		DrawTextureEx(ctx.assets.get_tx("text-entry.png"), {300, 20 + wave_pos}, 0, 5, WHITE);
		draw_text(ctx.assets, meta->entered_name, 710, 80 + wave_pos, 80, WHITE, {50, 50, 50, 255}, 70);
		ctx.player.draw_overworld_preview(ctx.assets, {325, 30 + wave_pos});
		auto char_cursor = ctx.assets.get_tx(meta->selected_char < 70 ? "char-select.png" : "char-select-wide.png");
		if (meta->char_cursor_blinker < 20){
			DrawTextureEx(char_cursor, {408.0f + ((meta->selected_char % 13) * 80), 200.0f + ((static_cast<u8>(meta->selected_char / 13)) * 120) + wave_pos}, 0, 5.1f, WHITE);
		}
	}

	void clear(){
		meta.reset();
	}
};
