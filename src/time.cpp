#include "time.hpp"

namespace Time{
	time_t::time_t(){
		const auto now = chrono::system_clock::now();
		const auto now_sec = chrono::floor<chrono::seconds>(now);
		local_now = chrono::zoned_time(chrono::current_zone(), now_sec);
	}
	SEASON time_t::get_season(){
		const auto local_tp = local_now.get_local_time();
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
		const auto local_tp = local_now.get_local_time();
		const auto days_tp = chrono::floor<chrono::days>(local_tp);
		const auto duration{local_tp - days_tp};
		const chrono::hh_mm_ss hhmmss{duration};
		return hhmmss;
	}
}
