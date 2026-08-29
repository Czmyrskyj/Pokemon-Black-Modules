#include "time.hpp"

namespace Time{
	SEASON time_t::get_season(){
		const auto now = chrono::system_clock::now();
		const auto local_tp = chrono::zoned_time(chrono::current_zone(), chrono::floor<chrono::seconds>(now)).get_local_time();
		const auto days_tp = chrono::floor<chrono::days>(local_tp);
		const chrono::year_month_day ymd{days_tp};
		unsigned month = static_cast<unsigned>(ymd.month());
		if (month <= 2 || month == 12){
			return SEASON::WINTER;
		} else if (month >= 3 && month <= 5){
			return SEASON::SPRING;
		} else if (month >= 6 && month <= 8){
			return SEASON::SUMMER;
		} else if (month >= 9 && month <= 11){
			return SEASON::AUTUMN;
		}
		return SEASON::SUMMER;
	}
	chrono::hh_mm_ss<chrono::seconds> time_t::get_time(){
		const auto now = chrono::system_clock::now();
		const auto local_tp = chrono::zoned_time(chrono::current_zone(), chrono::floor<chrono::seconds>(now)).get_local_time();
		const auto days_tp = chrono::floor<chrono::days>(local_tp);
		const auto duration{local_tp - days_tp};
		const chrono::hh_mm_ss hhmmss{duration};
		return hhmmss;
	}
}
