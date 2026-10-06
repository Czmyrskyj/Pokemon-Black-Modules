#include "pokemon.hpp"
#include "assets.hpp"
#include "spritedex.hpp"
#include <format>

#include <rlgl.h>

namespace {

	void draw_shadow(Texture2D texture, Rectangle src, Vector3 pos, Vector2 scale, Color tint){

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
		rlTexCoord2f(u0, v0); rlVertex3f(pos.x, pos.y, pos.z + scale.y);
		rlTexCoord2f(u1, v0); rlVertex3f(pos.x + scale.x, pos.y, pos.z + scale.y);
		rlTexCoord2f(u1, v1); rlVertex3f(pos.x + scale.x, pos.y, pos.z);
		rlTexCoord2f(u0, v1); rlVertex3f(pos.x, pos.y, pos.z);
		rlEnd();

		rlSetTexture(0);
		rlEnableBackfaceCulling();
		rlEnableDepthMask();
	}
}

void pokemon_t::render(assets_t& assets, spritedex_t& spritedex, POKEMON_RENDER_TYPE type, Vector3 pos, Vector2 scale, Camera3D* camera){
	auto tx = assets.get_tx(std::format("{}.png", static_cast<u16>(pokedex.id)));
	auto s_entry = spritedex.get_spritedex_entry(pokedex.id);

	float frame_width = 0.0f;
	float frame_height = 0.0f;
	float y_offset = 0.0f;
	float frame_count = 0.0f;



	switch (type){
		case POKEMON_RENDER_TYPE::FRONT:
			frame_width = static_cast<float>(s_entry.front_w);
			frame_height = static_cast<float>(s_entry.front_h);
			y_offset = 0.0f;
			frame_count = static_cast<float>(s_entry.front_frames);
			break;
		case POKEMON_RENDER_TYPE::BACK:
			frame_width = static_cast<float>(s_entry.back_w);
			frame_height = static_cast<float>(s_entry.back_h);
			y_offset = static_cast<float>(s_entry.front_h);
			frame_count = static_cast<float>(s_entry.back_frames);
			break;
		case POKEMON_RENDER_TYPE::MENU:
			frame_width = 68.0f;
			frame_height = 56.0f;
			y_offset = static_cast<float>(s_entry.front_h + s_entry.back_h);
			frame_count = 0.0f;
			break;
	}

	frame_timer++;
	if (type != POKEMON_RENDER_TYPE::MENU){
		if (!m_frames_blocked){
			if (frame_timer >= 5.0f){
				frame++;
				frame_timer = 0.0f;
			}
			if (frame >= frame_count){
				frame = 0;
			}
		} else {
			frame = 0;
		}
	} else {
		frame = 0;
		if (frame_timer >= 10.0f){
			menu_y += 10.0f;
			frame_timer = 0.0f;
		}
		if (menu_y >= 20.0f){
			menu_y = 0.0f;
		}
	}
	Rectangle src {
		frame * frame_width,
		      y_offset,
		      frame_width,
		      frame_height
	};
	if (!camera){
		DrawTexturePro(tx, src, {pos.x, pos.y - ((type == POKEMON_RENDER_TYPE::MENU) ? menu_y : 0.0f), scale.x, scale.y}, {0.0f, 0.0f}, 0.0f, WHITE);
	} else {
		constexpr float pixels_per_unit = 128.0f;
		Vector2 world_size = {
			(frame_width / pixels_per_unit) * scale.x,
			(frame_height / pixels_per_unit) * scale.y,
		};
		if (pos.y >= 0.0f){
			draw_shadow(tx, src, {pos.x, 0.0f, pos.z - 0.2f}, world_size, {0, 0, 0, 180});
		}
		DrawBillboardPro(*camera, tx, src, pos, {0, 1, 0}, world_size, {0, 0}, 0, WHITE);
	}
}
