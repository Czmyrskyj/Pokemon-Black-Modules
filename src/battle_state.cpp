#include "states.hpp"
#include "st-ctx.hpp"

#include <cmath>

battle_state_t::battle_state_t(st_ctx_t& ctx){
	ctx.camera.camera = {
		.position = {0.5f, 0.2f, 1},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
}

void battle_state_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	bg_rotation = std::sinf(GetTime()) * 10.0f;
}
void battle_state_t::render(st_ctx_t& ctx){
	BeginMode3D(ctx.camera.camera);
	DrawModelEx(ctx.assets.get_model("batt_bg01.glb"), {0, 0, 0}, {0, 0, 0}, bg_rotation, {0.05f, 0.05f, 0.05f}, WHITE);
	EndMode3D();
	draw_text(ctx.assets, "This is battle", 10, 10, 50, WHITE);
}
