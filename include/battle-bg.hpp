#pragma once

#include "common.hpp"
#include "time.hpp"
#include "regions.hpp"
#include <format>
#include <algorithm>

enum class BAT_BG_NAME{
	BG,
	STAGE,
};


inline std::string get_battle_bg_name(REGION reg, Time::SEASON season, BAT_BG_NAME n){
	usz idx = [&]() -> usz {
		switch (reg){
			case REGION::NUVEMA:
			case REGION::ACCUMULA:
			case REGION::STRIATON:
			case REGION::NACREN:
			case REGION::CASTELIA:
			case REGION::NIMBASA:
			case REGION::DRIFTVEIL:
			case REGION::MISTRALTON:
			case REGION::ICIRRUS:
			case REGION::OPELUCID:
			case REGION::BLACK_CITY:
			case REGION::LACUNOSA:
			case REGION::UNDELLA:
			case REGION::ANVILLE:
				return 4;
			case REGION::ROUTE_1:
			case REGION::ROUTE_2:
			case REGION::ROUTE_3:
			case REGION::ROUTE_4:
			case REGION::ROUTE_5:
			case REGION::ROUTE_6:
			case REGION::ROUTE_7:
			case REGION::ROUTE_8:
			case REGION::ROUTE_9:
			case REGION::ROUTE_10:
			case REGION::ROUTE_11:
			case REGION::ROUTE_12:
			case REGION::ROUTE_13:
			case REGION::ROUTE_14:
			case REGION::ROUTE_15:
			case REGION::ROUTE_16:
			case REGION::ROUTE_17:
			case REGION::ROUTE_18:
				return 2;
			case REGION::STRIATON_GYM:
				return 14;
			default: return 0;
		}
	}();
	const char* ssn = [&]() -> const char* {
		switch (season){
			case Time::SEASON::SPRING: return "a";
			case Time::SEASON::SUMMER: return "b";
			case Time::SEASON::AUTUMN: return "c";
			case Time::SEASON::WINTER: return "d";
			default: return "";
		}
	}();
	if (n == BAT_BG_NAME::BG){
		if (std::ranges::contains(std::array{2, 4}, idx)){
			return std::format("batt_bg{:02}{}.glb", idx, ssn);
		} else {

			return std::format("batt_bg{:02}.glb", idx);
		}
	} else {
		if (std::ranges::contains(std::array{2, 4}, idx)){
			return std::format("batt_stage{:02}{}.glb", idx, ssn);
		} else {
			return std::format("batt_stage{:02}.glb", idx);
		}
	}
}
