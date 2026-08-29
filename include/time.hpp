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
		public:
			SEASON get_season();
			chrono::hh_mm_ss<chrono::seconds> get_time();
	};
}
