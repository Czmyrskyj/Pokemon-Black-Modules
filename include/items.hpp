#pragma once

#include "common.hpp"
#include <expected>

enum class ITEM_TARGET : u8{
	NONE,
	ALLY,
	FOE,
};

enum class ITEM_EFFECT : u8{
	NONE,
	HEAL,
	REVIVE,
	CATCH,
};

#pragma pack(push, 1)
struct item_header_t{
	u16 id;
	u32 cost;
	u8 fling_power;
	u8 category;
	u8 target;
	u8 effect;
	u16 arg;
	u8 flags;
	u8 name_len;
};
#pragma pack(pop)

struct item_t{
	u16 id;
	u32 cost;
	u8 fling_power;
	u8 category;
	ITEM_TARGET target;
	ITEM_EFFECT effect;
	u16 effect_arg;
	u8 effect_flags;
	std::string name;
};

constexpr u8 ITEM_FLAG_FULL = 1;

class itemdex_t{
	std::vector<item_t> item_registry{};
	public:
	itemdex_t(){
		item_registry.reserve(711);
	}
	std::expected<void, std::string_view> load_itemdex();
	const item_t& get_item(u16 ID) const { return item_registry[ID]; }
};
