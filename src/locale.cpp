#include "locale.hpp"
#include "hash.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace Locale{
	std::expected<void, std::string_view> locale_t::load_locale_files(){
		if (!fs::exists("ui.LOCALE")){
			return std::unexpected("ui.LOCALE is absent");
		}
		
		if (!fs::exists("dialog.LOCALE")){
			return std::unexpected("dialog.LOCALE is absent");
		}

		std::ifstream ui_locale("ui.LOCALE", std::ios::binary | std::ios::ate);
		if (!ui_locale.is_open()){
			return std::unexpected("failed to open ui.LOCALE");
		}
		const std::streamsize ui_locale_file_size = ui_locale.tellg();
		if (ui_locale_file_size == std::streamsize(-1)){
			return std::unexpected("failed to determine ui.LOCALE file size");
		}
		ui_locale.seekg(0, std::ios::beg);
		
		std::vector<u8> ui_locale_contents(static_cast<usz>(ui_locale_file_size));
		ui_locale.read(reinterpret_cast<char*>(ui_locale_contents.data()), static_cast<std::streamsize>(ui_locale_contents.size()));

		u32 ui_locale_entries_amount = *reinterpret_cast<u32*>(ui_locale_contents.data());
		
		ui_cache.reserve(ui_locale_entries_amount);
		locale_entry_t* entry_table = reinterpret_cast<locale_entry_t*>(ui_locale_contents.data() + 4);

		for (u32 i = 0; i < ui_locale_entries_amount; i++){
			u32 hash = entry_table[i].key;
			u32 len = entry_table[i].len;
			u32 offset = entry_table[i].offset;
			ui_cache[hash] = std::string(reinterpret_cast<const char*>(ui_locale_contents.data() + offset), len);
		}

		std::ifstream dialog_locale("dialog.LOCALE", std::ios::binary | std::ios::ate);
		if (!dialog_locale.is_open()){
			return std::unexpected("failed to open dialog.LOCALE");
		}
		const std::streamsize dialog_locale_file_size = dialog_locale.tellg();
		if (dialog_locale_file_size == std::streamsize(-1)){
			return std::unexpected("failed to determine dialog.LOCALE file size");
		}
		dialog_locale.seekg(0, std::ios::beg);
		
		std::vector<u8> dialog_locale_contents(static_cast<usz>(dialog_locale_file_size));
		dialog_locale.read(reinterpret_cast<char*>(dialog_locale_contents.data()), static_cast<std::streamsize>(dialog_locale_contents.size()));

		u32 dialog_locale_entries_amount = *reinterpret_cast<u32*>(dialog_locale_contents.data());
		
		dialog_cache.reserve(dialog_locale_entries_amount);
		locale_entry_t* dialog_entry_table = reinterpret_cast<locale_entry_t*>(dialog_locale_contents.data() + 4);

		for (u32 i = 0; i < dialog_locale_entries_amount; i++){
			u32 hash = dialog_entry_table[i].key;
			u32 len = dialog_entry_table[i].len;
			u32 offset = dialog_entry_table[i].offset;
			dialog_cache[hash] = std::string(reinterpret_cast<const char*>(dialog_locale_contents.data() + offset), len);
		}

		return {};
	}
	std::string_view locale_t::get(TYPE t, std::string_view key){
		u32 hash = get_hash(key);
		if (t == TYPE::UI){
			auto it = ui_cache.find(hash);
			if (it != ui_cache.end()){
				return it->second;
			}
		} else {
			auto it = dialog_cache.find(hash);
			if (it != dialog_cache.end()){
				return it->second;
			}

		}
		return "[MISSING STRING]";
	}
}
