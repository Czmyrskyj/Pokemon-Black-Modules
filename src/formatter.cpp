#include "formatter.hpp"
#include "trainer.hpp"
#include <algorithm>

namespace fmt{
	std::string format(std::string_view txt, const trainer_t* trainer){
		std::string buffer(txt);
		if (trainer != nullptr){
			usz pos;
			while ((pos = buffer.find("[PLAYER]")) != std::string::npos){
				std::string_view name = trainer->get_name();
				buffer.replace(pos, 8, name);
				pos += name.size();
			}
		}
		usz pos = 0;
		while ((pos = buffer.find("[WAIT]")) != std::string::npos){
			buffer.replace(pos, 6, "\xEE\x80\x80");
			pos += 3;
		}

		return buffer;
	}
}
