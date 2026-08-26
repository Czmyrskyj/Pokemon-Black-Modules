#pragma once

#include <string>
#include <algorithm>

inline void from_all_caps_to_capitalized(std::string& str){
	std::transform(str.begin() + 1, str.end(), str.begin() + 1, [str](unsigned char c){
			return std::tolower(c);
			});
}
inline std::string new_all_caps_to_capitalized(std::string_view view){
	std::string str(view);
	std::transform(str.begin() + 1, str.end(), str.begin() + 1, [str](unsigned char c){
			return std::tolower(c);
			});
	return str;
}
