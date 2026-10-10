#pragma once

#include "common.hpp"
#include <expected>

#pragma pack(push, 1)
struct item_header_t{
	u16 id;
	u32 cost;
	u8 fling_power;
	u8 category;
	u8 name_len;
};
#pragma pack(pop)

struct item_t{
	u16 id;
	u32 cost;
	u8 fling_power;
	u8 category;
	std::string name;
};

class itemdex_t{
	std::vector<item_t> item_registry{};
	public:
	itemdex_t(){
		item_registry.reserve(711);
	}
	std::expected<void, std::string_view> load_itemdex();
	const item_t& get_item(u16 ID) const { return item_registry[ID]; }
};
