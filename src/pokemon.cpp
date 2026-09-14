#include "pokemon.hpp"
#include "assets.hpp"
#include <format>

constexpr float frame_width{153.0f};
constexpr float frame_height{118.0f};

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
		DrawBillboardPro(*camera, tx, src, pos, {0, 1, 0}, {scale.x, scale.y}, {0, 0}, 0, WHITE);
	}
}
