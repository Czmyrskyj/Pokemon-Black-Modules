#pragma once
#include <random>
#include <type_traits>
#include <concepts>

namespace rnd{
	static std::random_device rd;
	static std::mt19937 gen{rd()};
	namespace detail{
		template <typename T>
		using dist_int_t = std::conditional_t<(sizeof(T) < sizeof(short)), std::conditional_t<std::is_signed_v<T>, short, unsigned short>, T>;
	};

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
			} else if constexpr (std::is_integral_v<T>){
				using wide_t = detail::dist_int_t<T>;
				return static_cast<T>(std::uniform_int_distribution<wide_t>{static_cast<wide_t>(min), static_cast<wide_t>(max)}(gen));
			} else if constexpr (std::is_floating_point_v<T>){
				return std::uniform_real_distribution<T>{min, max}(gen);
			}
		}
};
