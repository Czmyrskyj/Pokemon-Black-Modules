#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include "random.hpp"
#include "writer.hpp"
#include "st-ctx.hpp"
#include "locale.hpp"

#include "state_title.hpp"
#include "state_main_menu.hpp"
#include "state_wifi_connection.hpp"
#include "state_boy_or_girl.hpp"
#include "state_whats_your_name.hpp"
#include "state_juniper_letter.hpp"
#include "state_choose_a_starter.hpp"

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
		trainer_t player{"Dvoriak", ""};
		Locale::locale_t locale;
		auto locale_load_res = locale.load_locale_files();
		if (!locale_load_res.has_value()){
			std::println(stderr, "error: {}", locale_load_res.error());
			return -1;
		}
		st_ctx_t ctx{
			.assets = assets,
			.player = player,
			.locale = locale,
		};

		MainMenu::init(ctx);
		BoyOrGirl::init(ctx);
		WhatsYourName::init(ctx);

		while (!WindowShouldClose()){
			if (!ctx.halt_music){
				if (!IsMusicStreamPlaying(ctx.current_music)){
					PlayMusicStream(ctx.current_music);
				}
				UpdateMusicStream(ctx.current_music);
			}

			switch (ctx.current_state){
				case STATE::TITLE: TitleState::update(ctx); break;
				case STATE::MAIN_MENU: MainMenu::update(ctx); break;
				case STATE::WIFI_CONNECTION: WIFIConnection::update(ctx); break;
				case STATE::BOY_OR_GIRL: BoyOrGirl::update(ctx); break;
				case STATE::WHATS_YOUR_NAME: WhatsYourName::update(ctx); break;
				case STATE::JUNIPER_LETTER: JuniperLetter::update(ctx, player); break;
				case STATE::CHOOSE_STARTER: ChooseAStarter::update(ctx, locale, player); break;

			}

			BeginDrawing();
			ClearBackground(BLACK);

			switch (ctx.current_state){
				case STATE::TITLE: TitleState::render(ctx); break;
				case STATE::MAIN_MENU: MainMenu::render(ctx); break;
				case STATE::WIFI_CONNECTION: WIFIConnection::render(ctx); break;
				case STATE::BOY_OR_GIRL: BoyOrGirl::render(ctx); break;
				case STATE::WHATS_YOUR_NAME: WhatsYourName::render(ctx); break;
				case STATE::JUNIPER_LETTER: JuniperLetter::render(ctx); break;
				case STATE::CHOOSE_STARTER: ChooseAStarter::render(ctx); break;
			}

			DrawRectangle(0, 0, wnd_w, wnd_h, Color{0, 0, 0, ctx.transition_alpha});
			EndDrawing();
		}
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
