#pragma once
#include "assets.hpp"
#include <raylib.h>
#include <string_view>
inline void draw_text(assets_t& assets, std::string_view str, float x, float y, unsigned size, Color tint, Color shadow_tint = {100, 100, 100, 255}){
	Font font = assets.get_font("main.otf");
	DrawTextEx(font, str.data(), {x + 2, y + 2}, size, 0, shadow_tint);
	DrawTextEx(font, str.data(), {x, y}, size, 0, tint);
}
