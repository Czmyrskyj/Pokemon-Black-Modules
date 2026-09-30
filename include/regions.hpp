#pragma once
#include <string_view>
#include "common.hpp"

enum class REGION{
	NUVEMA,
	ROUTE_1,
	ACCUMULA,
	STRIATON,
	NACREN,
	CASTELIA,
	NIMBASA,
	DRIFTVEIL,
	MISTRALTON,
	ICIRRUS,
	OPELUCID,
	BLACK_CITY,
	LACUNOSA,
	UNDELLA,
	ANVILLE,
	ROUTE_2,
	ROUTE_3,
	ROUTE_4,
	ROUTE_5,
	ROUTE_6,
	ROUTE_7,
	ROUTE_8,
	ROUTE_9,
	ROUTE_10,
	ROUTE_11,
	ROUTE_12,
	ROUTE_13,
	ROUTE_14,
	ROUTE_15,
	ROUTE_16,
	ROUTE_17,
	ROUTE_18,
	SKYARROW_BRIDGE,
	DRIFTVEIL_DRAWBRIDGE,
	TUBELINE_BRIDGE,
	MARVELOUS_BRIDGE,
	VILLAGE_BRIDGE,
	MARINE_TUBE,
	DREAMYARD,
	WELLSPRING_CAVE,
	PINWHEEL_FOREST,
	DESERT_RESORT,
	RELIC_CASTLE,
	BATTLE_SUBWAY,
	LOSTLORN_FOREST,
	COLD_STORAGE,
	CHARGESTONE_CAVE,
	CELESTIAL_TOWER,
	TWIST_MOUNTAIN,
	DRAGONSPIRAL_TOWER,
	MOOR_OF_ICIRRUS,
	CHALLENGERS_CAVE,
	VICTORY_ROAD,
	POKEMON_LEAGUE,
	NS_CASTLE,
	ROYAL_UNOVA,
	GIANT_CHASM,
	ABUNDANT_SHRINE,
	UNDELLA_BAY,
	ABYSSAL_RUINS,
	P2_LABORATORY,
	LIBERTY_GARDEN,
	WHITE_FOREST,
	ENTRALINK,
	ENTREE_FOREST,
	UNITY_TOWER,
};

constexpr std::string_view get_region_str(REGION r) {
	switch (r) {
		case REGION::NUVEMA:               return "Nuvema Town";
		case REGION::ACCUMULA:             return "Accumula Town";
		case REGION::STRIATON:             return "Striaton City";
		case REGION::NACREN:               return "Nacrene City";
		case REGION::CASTELIA:             return "Castelia City";
		case REGION::NIMBASA:              return "Nimbasa City";
		case REGION::DRIFTVEIL:            return "Driftveil City";
		case REGION::MISTRALTON:           return "Mistralton City";
		case REGION::ICIRRUS:              return "Icirrus City";
		case REGION::OPELUCID:             return "Opelucid City";
		case REGION::BLACK_CITY:           return "Black City";
		case REGION::LACUNOSA:             return "Lacunosa Town";
		case REGION::UNDELLA:              return "Undella Town";
		case REGION::ANVILLE:              return "Anville Town";
		case REGION::ROUTE_1:              return "Route 1";
		case REGION::ROUTE_2:              return "Route 2";
		case REGION::ROUTE_3:              return "Route 3";
		case REGION::ROUTE_4:              return "Route 4";
		case REGION::ROUTE_5:              return "Route 5";
		case REGION::ROUTE_6:              return "Route 6";
		case REGION::ROUTE_7:              return "Route 7";
		case REGION::ROUTE_8:              return "Route 8";
		case REGION::ROUTE_9:              return "Route 9";
		case REGION::ROUTE_10:             return "Route 10";
		case REGION::ROUTE_11:             return "Route 11";
		case REGION::ROUTE_12:             return "Route 12";
		case REGION::ROUTE_13:             return "Route 13";
		case REGION::ROUTE_14:             return "Route 14";
		case REGION::ROUTE_15:             return "Route 15";
		case REGION::ROUTE_16:             return "Route 16";
		case REGION::ROUTE_17:             return "Route 17";
		case REGION::ROUTE_18:             return "Route 18";
		case REGION::SKYARROW_BRIDGE:      return "Skyarrow Bridge";
		case REGION::DRIFTVEIL_DRAWBRIDGE: return "Driftveil Drawbridge";
		case REGION::TUBELINE_BRIDGE:     return "Tubeline Bridge";
		case REGION::MARVELOUS_BRIDGE:    return "Marvelous Bridge";
		case REGION::VILLAGE_BRIDGE:      return "Village Bridge";
		case REGION::MARINE_TUBE:          return "Marine Tube";
		case REGION::DREAMYARD:            return "Dreamyard";
		case REGION::WELLSPRING_CAVE:      return "Wellspring Cave";
		case REGION::PINWHEEL_FOREST:     return "Pinwheel Forest";
		case REGION::DESERT_RESORT:        return "Desert Resort";
		case REGION::RELIC_CASTLE:         return "Relic Castle";
		case REGION::BATTLE_SUBWAY:        return "Battle Subway";
		case REGION::LOSTLORN_FOREST:      return "Lostlorn Forest";
		case REGION::COLD_STORAGE:         return "Cold Storage";
		case REGION::CHARGESTONE_CAVE:     return "Chargestone Cave";
		case REGION::CELESTIAL_TOWER:      return "Celestial Tower";
		case REGION::TWIST_MOUNTAIN:       return "Twist Mountain";
		case REGION::DRAGONSPIRAL_TOWER:   return "Dragonspiral Tower";
		case REGION::MOOR_OF_ICIRRUS:      return "Moor of Icirrus";
		case REGION::CHALLENGERS_CAVE:     return "Challenger's Cave";
		case REGION::VICTORY_ROAD:         return "Victory Road";
		case REGION::POKEMON_LEAGUE:       return "Pokémon League";
		case REGION::NS_CASTLE:            return "N's Castle";
		case REGION::ROYAL_UNOVA:          return "Royal Unova";
		case REGION::GIANT_CHASM:          return "Giant Chasm";
		case REGION::ABUNDANT_SHRINE:      return "Abundant Shrine";
		case REGION::UNDELLA_BAY:          return "Undella Bay";
		case REGION::ABYSSAL_RUINS:        return "Abyssal Ruins";
		case REGION::P2_LABORATORY:        return "P2 Laboratory";
		case REGION::LIBERTY_GARDEN:       return "Liberty Garden";
		case REGION::WHITE_FOREST:         return "White Forest";
		case REGION::ENTRALINK:            return "Entralink";
		case REGION::ENTREE_FOREST:        return "Entree Forest";
		case REGION::UNITY_TOWER:          return "Unity Tower";
	}
	return "Unknown";
}

constexpr std::array<usz, static_cast<u8>(REGION::UNITY_TOWER) + 1> encounters_count{
	2,
	5,
};
