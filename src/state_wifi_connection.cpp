#include "st-ctx.hpp"

void wifi_connection_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
		ctx.resume_state_alpha(ctx.transition_alpha);
		ctx.set_music("Wi-Fi Connection.ogg");
		ctx.halt_music = false;
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
}
void wifi_connection_t::render(st_ctx_t& ctx){
	auto background = ctx.assets.get_tx("wifi-connection-bg.png");
	auto bluebar = ctx.assets.get_tx("bluebar.png");
	DrawTexturePro(background, {0, 0, static_cast<float>(background.width), static_cast<float>(background.height)}, {0, 0, wnd_w, wnd_h}, {0, 0}, 0, WHITE);
	DrawTexturePro(bluebar, FULL_TX(bluebar), {0, 0, wnd_w, 100}, {0, 0}, 0, WHITE);
	draw_text(ctx.assets, ctx.locale.get(Locale::TYPE::UI, "WIFI_CON_LABEL"), 10, 30, 50, WHITE, DARKBLUE);
}
