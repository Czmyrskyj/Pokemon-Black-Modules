#include "st-ctx.hpp"

void wifi_connection_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
		ctx.resume_state_alpha(ctx.transition_alpha);
		ctx.set_music("Wi-Fi Connection.ogg");
		ctx.halt_music = false;
		ctx.music_volume = 1.0f;
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
	Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
	SetMusicVolume(ctx.current_music, ctx.music_volume);
	if (return_to_main_menu){
		ctx.switch_state(STATE::MAIN_MENU);
		if (ctx.music_volume > 0.0f) ctx.music_volume -= 0.05f;
		else StopMusicStream(ctx.current_music);
		return;
	}
	switch (state){
		case WIFI_CONNECTION_STATE::CONNECTION_SERVICES:
			{
				if (IsKeyPressed(KEY_X)){
					return_to_main_menu = true;
					return;
				}
				sel_opt = ctx.control_selection(opt, 1, sel_sfx, dec_sfx, true);
			}
			break;
	}
}
void wifi_connection_t::render(st_ctx_t& ctx){
	auto background = ctx.assets.get_tx("wifi-connection-bg.png");
	auto bluebar = ctx.assets.get_tx("bluebar.png");
	auto con_serv_btn = ctx.assets.get_tx("con-serv-btn.png");
	DrawTexturePro(background, {0, 0, static_cast<float>(background.width), static_cast<float>(background.height)}, {0, 0, wnd_w, wnd_h}, {0, 0}, 0, WHITE);
	DrawTexturePro(bluebar, FULL_TX(bluebar), {0, 0, wnd_w, 100}, {0, 0}, 0, WHITE);
	draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "WIFI_CON_LABEL"), 10, 30, 50, WHITE, DARKBLUE);
	switch (state){
		case WIFI_CONNECTION_STATE::CONNECTION_SERVICES:
			{
				draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "CON_SERV"), wnd_w - 500, 30, 50, WHITE, DARKGREEN);
				for (usz i = 0; i < 2; i++){
					DrawTexturePro(con_serv_btn, {i * 100.0f, 0, 100, 80}, {200 + i * 800.0f, 300, (opt == i) ? 650.0f : 600.0f, (opt == i) ? 541.0f : 500.0f}, {0, 0}, 0, WHITE);
				}
			}
			break;
	}
}
