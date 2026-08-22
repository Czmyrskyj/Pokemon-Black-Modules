#pragma once
#include "assets.hpp"
#include <raylib.h>
#include <string_view>

inline void draw_text(assets_t& assets, std::string_view str, float x, float y, unsigned size, Color tint, Color shadow_tint = {50, 50, 50, 255}){
    if (str.empty()) return;
    Font font = assets.get_font("main.otf");
    int codepoints_count = 0;
    int* codepoints = LoadCodepoints(str.data(), &codepoints_count);
    usz count_to_draw = std::min(str.size(), static_cast<usz>(codepoints_count));

    DrawTextCodepoints(font, codepoints, count_to_draw, {x + 2, y + 2}, static_cast<float>(size), 0, shadow_tint);
    DrawTextCodepoints(font, codepoints, count_to_draw, {x, y}, static_cast<float>(size), 0, tint);

    UnloadCodepoints(codepoints);
}

inline void draw_text_seq(assets_t& assets, std::string_view str, float x, float y, unsigned size, u16& current_index, float& seq_timer, Color tint, Color shadow_tint = {100, 100, 100, 255}, float speed = 0.05f){
	if (current_index < str.size()){
		seq_timer += GetFrameTime();
		if (seq_timer >= speed){
			current_index++;
			seq_timer = 0;
		}
	}
	std::string_view buffer = str.substr(0, current_index);
	draw_text(assets, buffer, x, y, size, tint, shadow_tint);
}
