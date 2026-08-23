#pragma once

#include "common.hpp"
#include <unordered_map>
#include <expected>
#include <string>


namespace Locale{
	enum class TYPE{
		UI,
		DIALOG,
	};
	struct locale_entry_t{
		u32 key;
		u32 len;
		u32 offset;
	};
	class locale_t{
		std::unordered_map<u32, std::string> ui_cache;
		std::unordered_map<u32, std::string> dialog_cache;
		public:
		std::expected<void, std::string_view> load_locale_files();
		std::string_view get(TYPE type, std::string_view key);
	};
}
