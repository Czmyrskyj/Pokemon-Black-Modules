#pragma once

#include <chrono>

namespace chrono = std::chrono;
namespace Time{

	enum class SEASON{
		WINTER,
		SPRING,
		SUMMER,
		AUTUMN,
	};

	class time_t{
		const chrono::time_zone* tz = nullptr;
		public:
			time_t(){
				try { tz = chrono::current_zone(); }
				catch (const std::exception&) { tz = nullptr; }
			}
			SEASON get_season();
			chrono::hh_mm_ss<chrono::seconds> get_time();
	};
}
