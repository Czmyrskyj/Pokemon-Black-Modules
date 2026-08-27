#pragma once

#include "types.hpp"
enum class CATEGORY{
	PHYSICAL,
	SPECIAL,
	STATUS,
};

enum class MOVE_EFFECT{
};

struct move_t{
	MOVE_EFFECT move_effect;
	CATEGORY category;
	u8 power;
	TYPE type;
	u8 accuracy;
	u8 pp;
	u8 effect_chance;
	u8 target;
	i8 priority;
	u8 flags;
};
