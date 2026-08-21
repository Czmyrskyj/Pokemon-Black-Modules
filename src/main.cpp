#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include <print>

constexpr int wnd_w{1920};
constexpr int wnd_h{1080};

constexpr float alpha_delta{300.0f};

STATE state = STATE::TITLE;
void switch_state(STATE new_state, unsigned char& alpha, float delta = alpha_delta){
	float current_alpha = static_cast<float>(alpha);
	if (state != new_state){
		current_alpha+=delta*GetFrameTime();
		if (alpha >= 255.0f){
			current_alpha = 255;
			state = new_state;
		}
	}
	alpha = static_cast<unsigned char>(current_alpha);
}

void resume_state_alpha(unsigned char& alpha){
	float current_alpha = static_cast<float>(alpha);
	if (current_alpha > 0.0f) current_alpha -= alpha_delta*GetFrameTime();
	alpha = static_cast<unsigned char>(current_alpha);
}

void play_music(const Music& mus){
	if (!IsMusicStreamPlaying(mus)){
		PlayMusicStream(mus);
	}
	UpdateMusicStream(mus);
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
	bool set_state_meta = false;

	unsigned char black_rect_alpha = 0;
	bool halt_music = false;
	
	bool title_pressed_enter = false;

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
				if (IsKeyPressed(KEY_ENTER)){
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
