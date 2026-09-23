#include "pokeball.hpp"
#include <raylib.h>

void thrown_pokeball_renderer_t::draw(const Texture2D& texture, POKEBALL pokeball, Vector3 pos, const Camera3D& camera){
	Rectangle src = {static_cast<u8>(pokeball) * 40.0f, frame * 40.0f, 40.0f, 40.0f};
	DrawBillboardPro(camera, texture, src, pos, {0, 1, 0}, {0.2f, 0.2f}, {0, 0}, 0.0f, WHITE);
}
