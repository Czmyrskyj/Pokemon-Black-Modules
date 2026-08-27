#pragma once
#include <random>


namespace rnd{
	static std::random_device rd;
	static std::mt19937 gen{rd()};
	template <typename T>
		requires std::integral<T> || std::floating_point<T> || std::is_enum_v<T>
		T get_val(T min, T max){
			if constexpr (std::is_enum_v<T>){
				using underlying = std::underlying_type_t<T>;
				auto raw_val = get_val<underlying>(
					static_cast<underlying>(min),
					static_cast<underlying>(max)
				);
				return static_cast<T>(raw_val);
			}
			if constexpr (std::is_integral_v<T>){
				return std::uniform_int_distribution<T>{min, max}(gen);
			} else if constexpr (std::is_floating_point_v<T>){
				return std::uniform_real_distribution<T>{min, max}(gen);
			}
		}
};
