#include "writer.hpp"
#include <format>
#include <raylib.h>

void writer_t::set_text(std::string_view txt, float char_speed){
	target = txt;
	visible_buffer.clear();
	current_idx = 0;
	speed = char_speed;
	finished = false;
}


void draw_text(assets_t& assets, std::string_view str, float x, float y, unsigned size, Color tint, Color shadow_tint, usz spacing, usz txtbox_id){
    if (str.empty()) return;
    std::string s{str};
    Font font = assets.get_font("main.otf");
    int codepoints_count = 0;
    int* codepoints = LoadCodepoints(s.c_str(), &codepoints_count);

    if (txtbox_id != 0){
	    Texture2D txtbox = assets.get_tx(std::format("txtbox{}.png", txtbox_id));
	    DrawTexture(txtbox, (txtbox_id != 1) ? x - 10 : 0, y - 10, ColorAlpha(WHITE, tint.a / 255.0f));
    }

    if (spacing == 0){
	    DrawTextCodepoints(font, codepoints, codepoints_count, {x + 2, y + 2}, static_cast<float>(size), 0, shadow_tint); 
	    DrawTextCodepoints(font, codepoints, codepoints_count, {x, y}, static_cast<float>(size), 0, tint);
    } else {
	    for (usz i = 0; i < str.size(); i++){
		    DrawTextEx(font, (char[2]){str[i], 0}, {x + 2 + (i * spacing), y + 2}, static_cast<float>(size), 0, shadow_tint); 
		    DrawTextEx(font, (char[2]){str[i], 0}, {x + (i * spacing), y}, static_cast<float>(size), 0, tint);
	    }
    }
    UnloadCodepoints(codepoints);
}


void draw_text_seq(assets_t& assets, const writer_t& writer, float x, float y, unsigned size, Color tint, usz txtbox_id, Color shadow_tint){
	if (txtbox_id != 0){
		Texture2D txtbox = assets.get_tx(std::format("txtbox{}.png", txtbox_id));
		DrawTexture(txtbox, (txtbox_id != 1) ? x - 10 : 0, y - 20, ColorAlpha(WHITE, tint.a / 255.0f));
	}
	if (writer.visible_buffer.empty()) return;
	draw_text(assets, writer.visible_buffer, x, y, size, tint, shadow_tint, 0, 0);
}

void writer_t::update(){
	if (finished || target.empty() || current_idx >= target.size()) return;
	bool is_waiting = (current_idx + 3 <= target.size() && std::string_view(&target[current_idx], 3) == "\xEE\x80\x80");
	float step_speed = is_waiting ? (speed * 10.0f) : speed;
	timer += GetFrameTime();
	if (timer >= step_speed){
		timer = 0.0f;
		if (is_waiting){
			current_idx += 3;
		} else {
			int bytes_proc;
			GetCodepointNext(&target[current_idx], &bytes_proc);
			usz step = bytes_proc > 0 ? bytes_proc : 1;
			visible_buffer.append(target.substr(current_idx, step));
			current_idx += step;
		}
		if (current_idx >= target.size()){
			finished = true;
		}
	}
}
