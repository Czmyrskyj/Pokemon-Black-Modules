#include "state_title.hpp"
#include "common.hpp"
#include "st-ctx.hpp"

constexpr float logo_scale{0.3f};

namespace TitleState{
	static float strip_pos = 0.0f;
	static u8 press_enter_timer = 0;
	static bool title_pressed_enter = false;
	void update(st_ctx_t& ctx){
		if (!ctx.set_state_meta && ctx.transition_alpha > 0){
			ctx.resume_state_alpha(ctx.transition_alpha);
			ctx.current_music = ctx.assets.get_mus("Title.mp3");
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0){
			ctx.set_state_meta = true;
		}

		if (IsKeyPressed(KEY_ENTER) && !title_pressed_enter){
			title_pressed_enter = true;
			StopMusicStream(ctx.current_music);
			PlaySound(ctx.assets.get_cry(POKEDEX::RESHIRAM));
		}
		if (title_pressed_enter){
			ctx.halt_music = true;
			ctx.switch_state(STATE::MAIN_MENU, 200.0f);
		}
	}
	void render(st_ctx_t& ctx){
		static float strip_pos = 0.0f;
		static u8 press_enter_timer = 0;
		press_enter_timer++;
		if (press_enter_timer >= 50){
			press_enter_timer = 0;
		}
		strip_pos -= 100.0f*GetFrameTime();
		if (strip_pos < -wnd_w){
			strip_pos = -1.0f;
		}
		auto strip = ctx.assets.get_tx("title-strip.png");
		auto logo = ctx.assets.get_tx("pokemon-logo.png");
		for (usz i = 0; i < 43; i++){
			DrawTexture(strip, i * strip.width + strip_pos, 0, WHITE);
		}
		DrawTextureEx(logo, {wnd_w / 2.0f - (logo.width * logo_scale) / 2.0f, 90.0f}, 0.0f, logo_scale, WHITE);
		auto verfont = ctx.assets.get_font("version.otf");
		DrawTextEx(verfont, "black modules", {400.0f, 500.0f}, 120.0f, 1.0f, WHITE);
		DrawTextEx(verfont, "version", {730.0f, 650.0f}, 80.0f, 1.0f, WHITE);
		if (press_enter_timer < 25 && !title_pressed_enter){
			DrawText("press ENTER", 750, 950, 50.0f, WHITE);
		}

	}
};
