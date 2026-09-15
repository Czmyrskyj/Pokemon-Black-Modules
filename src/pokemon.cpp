#include "pokemon.hpp"
#include "assets.hpp"
#include <format>

#include <rlgl.h>

constexpr float frame_width{153.0f};
constexpr float frame_height{118.0f};

constexpr float pokemon_scale{1.0f};

namespace {

void draw_shadow(Texture2D texture, Rectangle src, Vector3 pos, Color tint){

	pos.y += 0.08f;

	rlDisableDepthMask();
	rlDisableBackfaceCulling();
	rlSetTexture(texture.id);

	float u0 = src.x / texture.width;
	float v0 = src.y / texture.height;
	float u1 = (src.x + src.width) / texture.width;
	float v1 = (src.y + src.height) / texture.height;

	rlBegin(RL_QUADS);
	rlColor4ub(tint.r, tint.g, tint.b, tint.a);
	rlTexCoord2f(u0, v0); rlVertex3f(pos.x, pos.y, pos.z + pokemon_scale);
	rlTexCoord2f(u1, v0); rlVertex3f(pos.x + pokemon_scale, pos.y, pos.z + pokemon_scale);
	rlTexCoord2f(u1, v1); rlVertex3f(pos.x + pokemon_scale, pos.y, pos.z);
	rlTexCoord2f(u0, v1); rlVertex3f(pos.x, pos.y, pos.z);
	rlEnd();

	rlSetTexture(0);
	rlEnableBackfaceCulling();
	rlEnableDepthMask();
}
}

void pokemon_t::render(assets_t& assets, POKEMON_RENDER_TYPE type, Vector3 pos, Vector2 scale, Camera3D* camera){
	auto tx = assets.get_tx(std::format("{}.png", static_cast<u16>(pokedex.id)));
	float y = [&]() -> float{
		switch(type){
			case POKEMON_RENDER_TYPE::FRONT: return 0.0f;
			case POKEMON_RENDER_TYPE::BACK: return frame_height;
			case POKEMON_RENDER_TYPE::MENU: return 2.0f * frame_height;
		}
		return 0.0f;
	}();
	frame_timer++;
	if (type != POKEMON_RENDER_TYPE::MENU){
		if (frame_timer >= 5.0f){
			frame++;
			frame_timer = 0.0f;
		}
		if (frame * frame_width >= tx.width){
			frame = 0;
		}
	} else {
		if (frame_timer >= 10.0f){
			menu_y += 5.0f;
			frame_timer = 0.0f;
		}
		if (menu_y >= 10.0f){
			menu_y = 0.0f;
		}
	}
	Rectangle src {
		frame * frame_width,
		      y + menu_y,
		      (type != POKEMON_RENDER_TYPE::MENU) ? frame_width : 40.0f,
		      (type != POKEMON_RENDER_TYPE::MENU) ? frame_height: 30.0f,
	};
	if (!camera){
		DrawTexturePro(tx, src, {pos.x, pos.y, scale.x, scale.y}, {0.0f, 0.0f}, 0.0f, WHITE);
	} else {
		draw_shadow(tx, src, {pos.x, 0.06f, pos.z - 0.2f}, {0, 0, 0, 180});
		DrawBillboardPro(*camera, tx, src, pos, {0, 1, 0}, {scale.x, scale.y}, {0, 0}, 0, WHITE);
	}
}
