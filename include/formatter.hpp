#pragma once

#include <string>
#include <unordered_map>
#include "pokedex.hpp"
struct trainer_t;
class formatter_t{
	std::unordered_map<std::string, std::string> format_variables;
	public:
	void feed(std::string key, std::string value);
	std::string format(std::string_view txt, POKEDEX pokemon = MISSINGNO, std::string_view freestanding = "");
};
