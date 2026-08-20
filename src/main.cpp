#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include <print>

constexpr int wnd_w{1920};
constexpr int wnd_h{1080};

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
	auto contents = assets.get("418.png");
	auto img = LoadImageFromMemory(".png", contents.data(), contents.size());
	auto t = LoadTextureFromImage(img);
	UnloadImage(img);
	usz frame = 0;
	while (!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		frame++;
		if (frame * 153 > t.width){
			frame = 0;
		}
		DrawTexturePro(t, {frame * 153.0f, 0, 153, 89}, {100, 100, 153, 89}, {0, 0}, 0, WHITE);
		EndDrawing();
	}
	UnloadTexture(t);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
