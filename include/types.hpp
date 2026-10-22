#pragma once

#include "common.hpp"
#include <string_view>
enum class TYPE : u8 {
	NONE = 0,
	NORMAL,
	FIRE,
	WATER,
	ELECTRIC,
	GRASS,
	ICE,
	FIGHTING,
	POISON,
	GROUND,
	FLYING,
	PSYCHIC,
	BUG,
	ROCK,
	GHOST,
	DRAGON,
	DARK,
	STEEL,
	COUNT,
};

constexpr std::array<std::string_view, static_cast<u8>(TYPE::COUNT)> type_strings{
	"None",
		"Normal",
		"Fire",
		"Water",
		"Electric",
		"Grass",
		"Ice",
		"Fighting",
		"Poison",
		"Ground",
		"Flying",
		"Psychic",
		"Bug",
		"Rock",
		"Ghost",
		"Dragon",
		"Dark",
		"Steel",
};

constexpr Color get_type_color(TYPE type){
    switch (type){
        case TYPE::NORMAL:   return Color{168, 167, 122, 255};
        case TYPE::FIRE:     return Color{238, 129,  48, 255};
        case TYPE::WATER:    return Color{ 99, 144, 240, 255};
        case TYPE::ELECTRIC: return Color{247, 208,  44, 255};
        case TYPE::GRASS:    return Color{122, 199,  76, 255};
        case TYPE::ICE:      return Color{150, 217, 214, 255};
        case TYPE::FIGHTING: return Color{194,  46,  40, 255};
        case TYPE::POISON:   return Color{163,  62, 161, 255};
        case TYPE::GROUND:   return Color{226, 191, 101, 255};
        case TYPE::FLYING:   return Color{169, 143, 243, 255};
        case TYPE::PSYCHIC:  return Color{249,  85, 135, 255};
        case TYPE::BUG:      return Color{166, 185,  26, 255};
        case TYPE::ROCK:     return Color{182, 161,  54, 255};
        case TYPE::GHOST:    return Color{115,  91, 151, 255};
        case TYPE::DRAGON:   return Color{111,  53, 252, 255};
        case TYPE::DARK:     return Color{112,  87,  70, 255};
        case TYPE::STEEL:    return Color{183, 183, 206, 255};
        default: return {};
    }
}
