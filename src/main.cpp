#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include <print>

constexpr int wnd_w{1920};
constexpr int wnd_h{1080};

STATE state = STATE::TITLE;
void switch_state(STATE new_state, unsigned char& alpha){
	if (state != new_state){
		alpha+=5;
		if (alpha == 255){
			state = new_state;
		}
	} else {
		alpha-=5;
	}
}

void play_music(const Music& mus){
	if (!IsMusicStreamPlaying(mus)){
		PlayMusicStream(mus);
	}
	UpdateMusicStream(mus);
}

int main(){
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(wnd_w, wnd_h, "Pokemon Black Modules");
	InitAudioDevice();
	SetTargetFPS(60);

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

	while (!WindowShouldClose()){
		play_music(current_music);
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
		EndDrawing();
	}
	UnloadMusicStream(current_music);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
