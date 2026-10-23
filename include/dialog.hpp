#pragma once

#include "common.hpp"
#include "writer.hpp"
#include "locale.hpp"
#include <string>
#include <vector>
class trainer_t;
struct st_ctx_t;
struct dialog_t{
	std::vector<std::string> lines;
	usz current_line{0};
	bool finished{false};
	float txtbox_x{10.0f};
	float txtbox_y{wnd_h + 20.0f};
	bool top{false};
	void init(std::vector<std::string> l) { 
		finished = false;
		speaker = "";
		prev_speaker = "";
		lines = std::move(l);
		current_line = 0;
		txtbox_y = !top ? wnd_h + 20.0f : -200.0f;
	}
	std::string speaker{""};
	std::string prev_speaker{""};
	bool hid_txtbox() const { return txtbox_y >= wnd_h; }
	std::string tell_whos_talking();
	std::string step(assets_t& assets, writer_t& writer, Locale::locale_t& locale);
	void draw_speaker(assets_t& assets);
	bool update(assets_t& assets, writer_t& writer);
	void change_music_if_char(st_ctx_t& ctx);
	std::string get_identifier() {
		return lines[current_line];
	}
};
