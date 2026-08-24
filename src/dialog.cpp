#include "dialog.hpp"
#include <algorithm>
#include <raylib.h>
#include <format>

std::string dialog_t::tell_whos_talking(){
	std::string_view line = lines[current_line];
	if (line.size() <= 5) return "";
	usz first_idx = line.find("_");
	if (first_idx == std::string_view::npos) return "";
	usz second_idx = line.find("_", first_idx + 1);
	if (second_idx == std::string_view::npos || second_idx <= 5) return "";
	std::string whos_talking = std::string(line.substr(5, second_idx - 5));
	std::transform(whos_talking.begin() + 1, whos_talking.end(), whos_talking.begin() + 1, [](unsigned char c){
				return std::tolower(c);
	});
	return whos_talking;
}

std::string dialog_t::step(writer_t& writer, Locale::locale_t& locale){
	if (current_line >= lines.size()) {
		finished = true;
		return "";
	}
	auto buffer = std::string(locale.get(Locale::TYPE::DIALOG, lines[current_line]));
	if (lines[current_line].starts_with("CHAR_")){
		speaker = tell_whos_talking();
		buffer = std::format("{}: {}", speaker, buffer);
		txtbox_x = 200.0f;
	} else {
		txtbox_x = 10.0f;
	}
	writer.set_text(buffer);
	current_line++;
	return buffer;
}

bool dialog_t::update(){
	if (IsKeyPressed(KEY_Z)){
		return true;
	}
	if (finished){
		if (txtbox_y <= wnd_h + 10.0f){
			txtbox_y += 10.0f;
		}
	} else {
		if (txtbox_y >= wnd_h - 200.0f){
			txtbox_y -= 10.0f;
		}
	}
	return false;
}

void dialog_t::draw_speaker(assets_t& assets){
	Texture2D speaker_tx = assets.get_tx(std::format("{}.png", speaker));
	DrawTexturePro(speaker_tx, {0, 0, 84, 78}, {10, txtbox_y, 170, 157}, {0, 0}, 0, WHITE);
}
