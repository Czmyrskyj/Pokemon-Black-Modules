#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include "random.hpp"
#include "font.hpp"
#include <print>

constexpr int wnd_w{1920};
constexpr int wnd_h{1080};

constexpr float alpha_delta{300.0f};

STATE state = STATE::TITLE;
bool set_state_meta = false;
void switch_state(STATE new_state, unsigned char& alpha, float delta = alpha_delta){
	float current_alpha = static_cast<float>(alpha);
	if (state != new_state){
		current_alpha+=delta*GetFrameTime();
		if (alpha >= 255.0f){
			current_alpha = 255;
			state = new_state;
			set_state_meta = false;
		}
	}
	alpha = static_cast<unsigned char>(current_alpha);
}

void resume_state_alpha(unsigned char& alpha){
	float current_alpha = static_cast<float>(alpha);
	if (current_alpha > 5.0f){
		current_alpha -= alpha_delta*GetFrameTime();
	} else {
		current_alpha = 0.0f;
	}
	alpha = static_cast<unsigned char>(current_alpha);
}

void play_music(const Music& mus){
	if (!IsMusicStreamPlaying(mus)){
		PlayMusicStream(mus);
	}
	UpdateMusicStream(mus);
}

i8 control_selection(u8& opt, u8 min, u8 max, const Sound& sel, const Sound& dec){
	if (IsKeyPressed(KEY_DOWN)){
		opt++;
		if (opt >= max) opt = 0;
		PlaySound(sel);
	} else if (IsKeyPressed(KEY_UP)){
		if (opt > 0){
			opt--;
		} else {
			opt = max - 1;
		}
		PlaySound(sel);
	}
	if (IsKeyPressed(KEY_Z)){
		PlaySound(dec);
		return opt;
	}
	return -1;
}

void draw_title_screen(assets_t& assets, bool pressed){
	constexpr float logo_scale{0.3f};
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
	auto strip = assets.get_tx("title-strip.png");
	auto logo = assets.get_tx("pokemon-logo.png");
	for (usz i = 0; i < 43; i++){
		DrawTexture(strip, i * strip.width + strip_pos, 0, WHITE);
	}
	DrawTextureEx(logo, {wnd_w / 2.0f - (logo.width * logo_scale) / 2.0f, 90.0f}, 0.0f, logo_scale, WHITE);
	auto verfont = assets.get_font("version.otf");
	DrawTextEx(verfont, "black modules", {400.0f, 500.0f}, 120.0f, 1.0f, WHITE);
	DrawTextEx(verfont, "version", {730.0f, 650.0f}, 80.0f, 1.0f, WHITE);
	if (press_enter_timer < 25 && !pressed){
		DrawText("press ENTER", 750, 950, 50.0f, WHITE);
	}
}

std::vector<std::string_view> menu_options{
	"NEW GAME",
	"CONNECT VIA WIFI"
};

int main(){
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(wnd_w, wnd_h, "Pokemon Black Modules");
	InitAudioDevice();
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

	Music current_music{};
	unsigned char black_rect_alpha = 0;
	bool halt_music = false;
	bool title_pressed_enter = false;
	u8 menu_index = rnd::get_val<int>(1, 17);
	u8 selected_option = 0;
	i8 switch_state_in_main_menu = -1;
	while (!WindowShouldClose()){
		if (!halt_music){
			play_music(current_music);
		}
		BeginDrawing();
		ClearBackground(BLACK);
		switch (state){
			case STATE::TITLE:
				{
					if (!set_state_meta){
						current_music = assets.get_mus("Title.mp3");
						set_state_meta = true;
					}
				}
				draw_title_screen(assets, halt_music);
				if (IsKeyPressed(KEY_ENTER) && !title_pressed_enter){
					title_pressed_enter = true;
					StopMusicStream(current_music);
					PlaySound(assets.get_cry(POKEDEX::RESHIRAM));
				}
				if (title_pressed_enter){
					halt_music = true;
					switch_state(STATE::MAIN_MENU, black_rect_alpha, 200.0f);
				}
				break;
			case STATE::MAIN_MENU:
				{
					if (!set_state_meta && black_rect_alpha > 0.0f){
						resume_state_alpha(black_rect_alpha);
					}
					if (!set_state_meta && black_rect_alpha <= 0.0f){
						set_state_meta = true;
					}
					auto mntx = assets.get_tx("menu10.png");
					
					DrawTexturePro(mntx, {0, 0, static_cast<float>(mntx.width), static_cast<float>(mntx.height)}, {0, 0, wnd_w, 1537}, {0, 0}, 0.0f, {150, 150, 150, 255});
					u8 selected_button_c = static_cast<u8>(60 + std::sin(GetTime() * 3) * 50);
					Sound sel_sfx = assets.get_sfx("SEQ_SE_SELECT1.wav");
					Sound dec_sfx = assets.get_sfx("SEQ_SE_DECIDE1.wav");
					i8 opt;
					if (black_rect_alpha == 0){
						opt = control_selection(selected_option, 0, menu_options.size(), sel_sfx, dec_sfx);
						if (opt != -1 && switch_state_in_main_menu == -1){
							switch_state_in_main_menu = opt;
						}
					}
					for (usz i = 0; i < menu_options.size(); i++){
						auto button = selected_option == i ? assets.get_tx("button-selected.png") : assets.get_tx("button.png");
						Color c = (selected_option == i) ? Color{static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), 220} : Color{255, 255, 255, 200};
						DrawTextureEx(button, {300, 300 + i * 100.0f}, 0.0f, 4.0f, c);
						draw_text(assets, menu_options[i], 350, 330 + i * 100.0f, 50, WHITE);
					}


					switch (switch_state_in_main_menu){
						case 0:
							switch_state(STATE::JUNIPER_INTRODUCTION, black_rect_alpha);
							break;
						case 1:
							switch_state(STATE::WIFI_CONNECTION, black_rect_alpha);
							break;
						case -1:
						default:
							break;
					}
				}
				break;
			case STATE::WIFI_CONNECTION:
				{
					if (!set_state_meta && black_rect_alpha > 0.0f){
						resume_state_alpha(black_rect_alpha);
					}
					if (!set_state_meta && black_rect_alpha <= 0.0f){
						current_music = assets.get_mus("Wi-Fi Connection.mp3");
						halt_music = false;
						set_state_meta = true;
					}
					auto background = assets.get_tx("wifi-connection-bg.png");
					DrawTexturePro(background, {0, 0, static_cast<float>(background.width), static_cast<float>(background.height)}, {0, 0, wnd_w, wnd_h}, {0, 0}, 0, WHITE);
					draw_text(assets, "WI-FI Connection is not supported yet.", 500, 400, 50, WHITE, DARKBLUE);
				}
				break;
			case STATE::JUNIPER_INTRODUCTION:
				{
				}
				break;
			case STATE::BOY_OR_GIRL:
				{

				}
				break;
			case STATE::WHATS_YOUR_NAME:
				{

				}
				break;
			case STATE::MAIN:
				{

				}
				break;
			case STATE::BATTLE:
				{

				}
				break;
			case STATE::SHOP:
				{

				}
				break;
			case STATE::POKECENTER:
				{

				}
				break;
		}
		DrawRectangle(0, 0, wnd_w, wnd_h, Color{0, 0, 0, black_rect_alpha});
		EndDrawing();
	}
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
