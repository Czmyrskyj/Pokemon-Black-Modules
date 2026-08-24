#include "assets.hpp"
#include <fstream>
#include <filesystem>
#include "pokedex.hpp"

namespace fs = std::filesystem;
const fs::path assets_pakg{"assets.PAKG"};

std::expected<void, std::string_view> assets_t::load_assets(){
	if (!fs::exists(assets_pakg)){
		return std::unexpected("assets.PAKG is absent");
	}
	std::ifstream in(assets_pakg, std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open assets.PAKG");
	}
	u32 assets_count;
	in.read(reinterpret_cast<char*>(&assets_count), sizeof(u32));
	asset_registry.clear();
	asset_registry.reserve(assets_count);
	for (usz i = 0; i < assets_count; i++){
		u32 path_len = 0;
		in.read(reinterpret_cast<char*>(&path_len), sizeof(u32));
		std::string path(path_len, '\0');
		in.read(path.data(), path_len);

		u32 size;
		u32 offset;
		in.read(reinterpret_cast<char*>(&size), sizeof(u32));
		in.read(reinterpret_cast<char*>(&offset), sizeof(u32));

		asset_registry[path] = {
			.name = path,
			.size = size,
			.offset = offset,
		};
	}
	return {};
}

std::vector<u8> assets_t::read_bytes(std::string_view name) const{
	auto it = asset_registry.find(std::string(name));
	if (it == asset_registry.end()) return {};

	std::ifstream in(assets_pakg, std::ios::binary);
	if (!in.is_open()){
		return {};
	}
	in.seekg(it->second.offset, std::ios::beg);
	std::vector<u8> buffer(it->second.size);
	in.read(reinterpret_cast<char*>(buffer.data()), it->second.size);
	return buffer;
}

Texture2D assets_t::get_tx(std::string_view name) {
	std::string key(name);
	if (!texture_cache.contains(key)){
		auto contents = read_bytes(key);
		if (contents.empty()) return Texture2D{};
		Image img = LoadImageFromMemory(".png", contents.data(), contents.size());
		Texture2D texture = LoadTextureFromImage(img);
		UnloadImage(img);
		texture_cache[key] = texture;
	}
	return texture_cache[key];
}

Sound assets_t::get_sfx(std::string_view name) {
	std::string key(name);
	if (!sound_cache.contains(key)){
		auto contents = read_bytes(key);
		if (contents.empty()) return Sound{};
		Wave wave = LoadWaveFromMemory(".wav", contents.data(), contents.size());
		Sound sound = LoadSoundFromWave(wave);
		UnloadWave(wave);
		sound_cache[key] = sound;
	}
	return sound_cache[key];
}

Sound assets_t::get_cry(u16 id) {
	return get_sfx(std::format("{}.wav", id));
}

Music assets_t::get_mus(std::string_view name) {
	std::string key(name);
	if (!music_cache.contains(key)){
		music_buffer[key] = read_bytes(key);
		const auto& contents = music_buffer[key];
		if (contents.empty()) return Music{};
		Music music = LoadMusicStreamFromMemory(".mp3", contents.data(), contents.size());
		music_cache[key] = music;
	}
	return music_cache[key];
}


Font assets_t::get_font(std::string_view name) {
	std::string key(name);
	if (!font_cache.contains(key)){
		auto contents = read_bytes(key);
		if (contents.empty()) return Font{};
		const auto codepoints = std::to_array<int>({ 
			'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
			'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
			'0','1','2','3','4','5','6','7','8','9',' ', '!', '?', ',', '.', '\n', '-', '\'',
			0x00E9
		});
		Font font = LoadFontFromMemory(".otf", contents.data(), contents.size(), 50, codepoints.data(), codepoints.size());
		font_cache[key] = font;
	}
	return font_cache[key];
}

void assets_t::unload_assets(){
	for (auto& [name, tx] : texture_cache){
		UnloadTexture(tx);
	}
	for (auto& [name, sfx] : sound_cache){
		UnloadSound(sfx);
	}
	for (auto& [name, mus] : music_cache){
		UnloadMusicStream(mus);
	}
	for (auto& [name, font] : font_cache){
		UnloadFont(font);
	}
	texture_cache.clear();
	sound_cache.clear();
	music_cache.clear();
	font_cache.clear();
}
