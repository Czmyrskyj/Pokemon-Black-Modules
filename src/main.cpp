#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"

constexpr int wnd_w{1920};
constexpr int wnd_h{1080};

int main(){
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(wnd_w, wnd_h, "Pokemon Black Modules");
	SetTargetFPS(60);
	trainer_t trainer;
	trainer_t trainer2;
	battle_t battle{trainer, trainer2};
	while (!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
