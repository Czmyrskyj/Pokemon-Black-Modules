#pragma once
#include "common.hpp"
#include <vector>
#include <expected>

#pragma pack(push, 1)
struct module_header_t{
	u8 rarity;
	u16 cost;
	u8 effect_count;
};
struct module_effect_header_t{
	u8 trigger, condition, cond_param, action;
	i16 value;
	u8 stat, target;
};
#pragma pack(pop)

enum class MODULE_RARITY : u8{
	COMMON,
	UNCOMMON,
	RARE,
	EXCEPTIONAL,
};

namespace MOD_EFF{

	enum class TRIGGER : u8{

	};
	enum class ACTION : u8{

	};
	enum class STAT : u8{

	};
	enum class CONDITION : u8{

	};
	enum class TARGET : u8{

	};

};
struct module_effect_t{
	MOD_EFF::TRIGGER trigger;
	MOD_EFF::CONDITION condition;
	u8 cond_param;
	MOD_EFF::ACTION action;
	i16 value;
	MOD_EFF::STAT stat;
	MOD_EFF::TARGET target;
};

struct module_t{
	std::string name;
	std::string description;
	MODULE_RARITY rarity;
	u16 cost;
	std::vector<module_effect_t> effects;
};

class moduledex_t{
	std::vector<module_t> module_registry;
	public:
	std::expected<void, std::string_view> load_moduledex();
	const module_t& get_module(u16 ID){
		return module_registry[ID];
	}
};
