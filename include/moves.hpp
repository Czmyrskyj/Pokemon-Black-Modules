#pragma once

#include "types.hpp"
#include <expected>

enum class CATEGORY{
	PHYSICAL,
	SPECIAL,
	STATUS,
};

#pragma pack(push, 1)
struct move_header_t{
	u16 id;
	TYPE type;
	u8 power;
	u8 pp;
	u8 accuracy;
	u8 priority;
	u8 damage_class;
	u8 name_len;
};
#pragma pack(pop)

struct move_t{
	u16 id;
	TYPE type;
	u8 power;
	u8 pp;
	u8 accuracy;
	i8 priority;
	u8 damage_class;
	std::string name;
};

class movedex_t{
	std::vector<move_t> move_registry{};
	public:
	std::expected<void, std::string_view> load_movedex();
	const move_t& get_move(u16 ID) const { return move_registry[ID]; }
};
