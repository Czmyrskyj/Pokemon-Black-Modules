#pragma once

#include <string>
#include "pokedex.hpp"

struct trainer_t;
namespace fmt{
	extern std::string format(std::string_view txt, const trainer_t* trainer = nullptr, POKEDEX pokemon = MISSINGNO, std::string_view custom = "");
}
