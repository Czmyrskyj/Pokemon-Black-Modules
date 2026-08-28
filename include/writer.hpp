#pragma once
#include "assets.hpp"
#include <string_view>

struct writer_t{
	std::string target{};
	std::string visible_buffer{};
	usz current_idx = 0;
	float timer = 0.0f;
	float speed = 0.0f;
	bool finished = false;
	
	void set_text(std::string_view txt, float char_speed = 0.01f);
	void update();
};

extern void draw_text(assets_t& assets, std::string_view str, float x, float y, unsigned size, Color tint, Color shadow_tint = {150, 150, 150, 255}, usz spacing = 0, usz txtbox_id = 0);
extern void draw_text_seq(assets_t& assets, const writer_t& writer, float x, float y, unsigned size, Color tint, usz txtbox_id = 0, Color shadow_tint = {50, 50, 50, 255});
