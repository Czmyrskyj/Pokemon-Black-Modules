#pragma once

#include "common.hpp"
#include <string>

struct trainer_t;
namespace fmt{
	extern std::string format(std::string_view txt, const trainer_t* trainer = nullptr);
}
