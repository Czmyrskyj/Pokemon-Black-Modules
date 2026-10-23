#include <raylib.h>
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include "battle_info.hpp"
#include "st-ctx.hpp"
#include "locale.hpp"
#include "pokedex.hpp"
#include "time.hpp"
#include "formatter.hpp"
#include "music-offset.hpp"
#include "moves.hpp"
#include "items.hpp"

#include <print>

constexpr u16 sample_rate{44100};

int main(){
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(wnd_w, wnd_h, "Pokemon Black Modules");
	InitAudioDevice();
	ToggleFullscreen();
	SetTargetFPS(60);
	HideCursor();
	{
		assets_t assets;
		auto assets_load_res = assets.load_assets();
		if (!assets_load_res.has_value()){
			std::println(stderr, "error: {}", assets_load_res.error());
			CloseAudioDevice();
			CloseWindow();
			return -1;
		}
		
		SetTextureFilter(assets.get_font("main.otf").texture, TEXTURE_FILTER_POINT);
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
		Time::time_t time;
		battle_info_t battle_info;
		auto battle_info_load_res = battle_info.load_battle_info();
		if (!battle_info_load_res.has_value()){
			std::println(stderr, "error: {}", battle_info_load_res.error());
			return -1;
		}
		formatter_t formatter;
		music_offset_t music_offset;
		auto music_offset_load_res = music_offset.load();
		if (!music_offset_load_res.has_value()){
			std::println(stderr, "error: {}", music_offset_load_res.error());
			return -1;
		}

		spritedex_t spritedex;
		auto spritedex_load_res = spritedex.load_spritedex();
		if (!spritedex_load_res.has_value()){
			std::println(stderr, "error: {}", spritedex_load_res.error());
			return -1;
		}

		movedex_t movedex;
		auto movedex_load_res = movedex.load_movedex();
		if (!movedex_load_res.has_value()){
			std::println(stderr, "error: {}", movedex_load_res.error());
			return -1;
		}

		itemdex_t itemdex;
		auto itemdex_load_res = itemdex.load_itemdex();
		if (!itemdex_load_res.has_value()){
			std::println(stderr, "error: {}", itemdex_load_res.error());
			return -1;
		}

		camera_t camera;

		st_ctx_t ctx{
			.assets = assets,
			.locale = locale,
			.pokedex = pokedex,
			.spritedex = spritedex,
			.movedex = movedex,
			.itemdex = itemdex,
			.time = time,
			.battle_info = battle_info,
			.formatter = formatter,
			.camera = camera,
		};

		// WARN: delete after testing!
		ctx.set_temp_data();

		while (!WindowShouldClose()){
			ctx.validate_music();
			if (!ctx.halt_music && ctx.current_music.stream.buffer != nullptr){
				bool playing = IsMusicStreamPlaying(ctx.current_music);
				if (!playing && !ctx.music_started){
					PlayMusicStream(ctx.current_music);
					ctx.music_started = true;
				} else if (!playing && ctx.music_started) {
					u32 moff = music_offset.get(ctx.music_name);
					PlayMusicStream(ctx.current_music);
					if (moff >= 1){
						SeekMusicStream(ctx.current_music, static_cast<float>(moff) / sample_rate);
					}
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
