#pragma once
#include "common.hpp"
#include <expected>
#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>
#include <raylib.h>

struct asset_entry_t{
	std::string name;
	usz size;
	usz offset;
};


class assets_t{
	std::unordered_map<std::string, asset_entry_t> asset_registry;

	std::unordered_map<std::string, Texture2D> texture_cache;
	std::unordered_map<std::string, Sound> sound_cache;
	std::unordered_map<std::string, Music> music_cache;
	std::unordered_map<std::string, Font> font_cache;
	std::unordered_map<std::string, Model> model_cache;

	std::unordered_map<std::string, std::vector<u8>> music_buffer;

	public:
	~assets_t(){
		unload_assets();
	}
	std::expected<void, std::string_view> load_assets();
	std::vector<u8> read_bytes(std::string_view name) const;

	void init_file_callback();

	Texture2D get_tx(std::string_view);
	Sound get_sfx(std::string_view);
	Sound get_cry(u16 id);
	Music get_mus(std::string_view);
	Font get_font(std::string_view);
	Model get_model(std::string_view);
	void unload_texture(std::string_view);
	void unload_track(std::string_view);
	void unload_assets();
};
