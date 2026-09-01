#include "pokemon.hpp"
#include "assets.hpp"
#include <format>

constexpr float frame_width{153.0f};
constexpr float frame_height{118.0f};

void pokemon_t::render(assets_t& assets, POKEMON_RENDER_TYPE type, Rectangle dest){
	auto tx = assets.get_tx(std::format("{}.png", static_cast<u16>(pokedex.id)));
	float y = [&]() -> float{
		switch(type){
			case POKEMON_RENDER_TYPE::FRONT: return 0.0f;
			case POKEMON_RENDER_TYPE::BACK: return frame_height;
			case POKEMON_RENDER_TYPE::MENU: return 2.0f * frame_height;
		}
		return 0.0f;
	}();
	if (type != POKEMON_RENDER_TYPE::MENU){
		frame_timer++;
		if (frame_timer >= 5.0f){
			frame++;
			frame_timer = 0.0f;
		}
		if (frame * frame_width >= tx.width){
			frame = 0;
		}
	}
	Rectangle src {
		frame * frame_width,
		      y,
		      (type != POKEMON_RENDER_TYPE::MENU) ? frame_width : 40.0f,
		      (type != POKEMON_RENDER_TYPE::MENU) ? frame_height: 30.0f,
	};
	DrawTexturePro(tx, src, dest, {0.0f, 0.0f}, 0.0f, WHITE);
}
