#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include "random.hpp"
#include "writer.hpp"
#include "st-ctx.hpp"
#include "locale.hpp"
#include "pokedex.hpp"
#include "time.hpp"

#include <print>

int main(){
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(wnd_w, wnd_h, "Pokemon Black Modules");
	InitAudioDevice();
	ToggleFullscreen();
	SetTargetFPS(60);
	{
		assets_t assets;
		auto assets_load_res = assets.load_assets();
		if (!assets_load_res.has_value()){
			std::println(stderr, "error: {}", assets_load_res.error());
			CloseAudioDevice();
			CloseWindow();
			return -1;
		}
		
		SetTextureFilter(assets.get_font("font.otf").texture, TEXTURE_FILTER_POINT);
		Locale::locale_t locale;
		auto locale_load_res = locale.load_locale_files();
		if (!locale_load_res.has_value()){
			std::println(stderr, "error: {}", locale_load_res.error());
			return -1;
		}
		pokedex_t pokedex;
		auto pokedex_load_res = pokedex.load_pokedex();
		if (!pokedex_load_res.has_value()){
			std::println(stderr, "error: {}", pokedex_load_res.error());
			return -1;
		}
		st_ctx_t ctx{
			.assets = assets,
			.locale = locale,
			.pokedex = pokedex,
		};

		Time::time_t time;
		auto season = time.get_season();
	
		active_state_t state;

		while (!WindowShouldClose()){
			auto tm = time.get_time();
			if (!ctx.halt_music){
				if (!IsMusicStreamPlaying(ctx.current_music)){
					PlayMusicStream(ctx.current_music);
				}
				UpdateMusicStream(ctx.current_music);
			}

			std::visit([&ctx](auto& state){
				using T = std::decay_t<decltype(state)>;
				if constexpr (!std::is_same_v<T, std::monostate>){
					state.update(ctx);
				}
			}, ctx.state_data);			

			BeginDrawing();
			ClearBackground(ctx.bg_color);

			std::visit([&ctx](auto& state){
				using T = std::decay_t<decltype(state)>;
				if constexpr (!std::is_same_v<T, std::monostate>){
					state.render(ctx);
				}
			}, ctx.state_data);			

			DrawRectangle(0, 0, wnd_w, wnd_h, Color{ctx.transition_color.r, ctx.transition_color.g, ctx.transition_color.b, ctx.transition_alpha});
			EndDrawing();
		}
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
