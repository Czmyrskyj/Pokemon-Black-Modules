#pragma once

#include "common.hpp"
#include "writer.hpp"
#include "locale.hpp"
#include <string>
#include <vector>
class trainer_t;
struct dialog_t{
	std::vector<std::string> lines;
	usz current_line{0};
	bool finished{false};
	void init(std::vector<std::string> l) { 
		lines = std::move(l);
		current_line = 0;
	}
	float txtbox_x{10.0f};
	float txtbox_y{wnd_h + 10.0f};
	std::string speaker{""};
	bool hid_txtbox() const { return txtbox_y >= wnd_h; }
	std::string tell_whos_talking();
	std::string step(assets_t& assets, writer_t& writer, Locale::locale_t& locale);
	void draw_speaker(assets_t& assets);
	bool update(assets_t& assets, writer_t& writer);
};
