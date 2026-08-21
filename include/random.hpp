#pragma once
#include <random>


namespace rnd{
	static std::random_device rd;
	static std::mt19937 gen{rd()};
	template <typename T>
		requires std::integral<T> || std::floating_point<T>
		T get_val(T min, T max){
			if constexpr (std::is_integral_v<T>){
				return std::uniform_int_distribution<T>{min, max}(gen);
			} else if constexpr (std::is_floating_point_v<T>){
				return std::uniform_real_distribution<T>{min, max}(gen);
			}
		}
};
