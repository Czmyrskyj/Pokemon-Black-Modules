#pragma once
#include "common.hpp"

inline u32 get_hash(std::string_view str){
	u32 crc = 0xffffffff;
	for (u8 c : str){
		crc ^= c;
		for (usz i = 0; i < 8; i++){
			crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
		}
	}
	return ~crc;
}
