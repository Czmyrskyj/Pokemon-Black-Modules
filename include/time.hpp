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
		chrono::zoned_time<chrono::seconds> local_now{};
		public:
		time_t();
		SEASON get_season();
		chrono::hh_mm_ss<chrono::seconds> get_time();
	};
}
