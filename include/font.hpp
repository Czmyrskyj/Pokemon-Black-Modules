#pragma once
#include "assets.hpp"
#include <raylib.h>
#include <string_view>
#include <format>

inline void draw_text(assets_t& assets, std::string_view str, float x, float y, unsigned size, Color tint, Color shadow_tint = {50, 50, 50, 255}, usz spacing = 0){
    if (str.empty()) return;
    Font font = assets.get_font("main.otf");
    int codepoints_count = 0;
    int* codepoints = LoadCodepoints(str.data(), &codepoints_count);
    usz count_to_draw = std::min(str.size(), static_cast<usz>(codepoints_count));

    if (spacing == 0){
	DrawTextCodepoints(font, codepoints, count_to_draw, {x + 2, y + 2}, static_cast<float>(size), 0, shadow_tint); 
	DrawTextCodepoints(font, codepoints, count_to_draw, {x, y}, static_cast<float>(size), 0, tint);
    } else {
	for (usz i = 0; i < str.size(); i++){
		DrawTextEx(font, (char[2]){str[i], 0}, {x + 2 + (i * spacing), y + 2}, static_cast<float>(size), 0, shadow_tint); 
		DrawTextEx(font, (char[2]){str[i], 0}, {x + (i * spacing), y}, static_cast<float>(size), 0, tint);
	}
    }
    UnloadCodepoints(codepoints);
}

inline bool draw_text_seq(assets_t& assets, std::string_view str, float x, float y, unsigned size, u16& current_index, float& seq_timer, Color tint, usz txtbox_id = 0, Color shadow_tint = {100, 100, 100, 255}, float speed = 0.05f){
	Texture2D txtbox;
	if (txtbox_id != 0){
		txtbox = assets.get_tx(std::format("txtbox{}.png", txtbox_id));
		DrawTexture(txtbox, x - 10, y - 10, {255, 255, 255, tint.a});
	}
	if (current_index < str.size()){
		seq_timer += GetFrameTime();
		if (seq_timer >= speed){
			current_index++;
			seq_timer = 0;
		}
	}
	std::string_view buffer = str.substr(0, current_index);
	draw_text(assets, buffer, x, y, size, tint, shadow_tint);
	return current_index == str.size();
}
