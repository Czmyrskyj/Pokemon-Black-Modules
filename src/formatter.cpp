#include "formatter.hpp"
#include "trainer.hpp"
#include "names.hpp"

namespace fmt{
	std::string format(std::string_view txt, const trainer_t* trainer, POKEDEX pokemon, std::string_view custom){
		std::string buffer(txt);
		if (trainer != nullptr){
			usz pos;
			while ((pos = buffer.find("[PLAYER]")) != std::string::npos){
				std::string_view name = trainer->get_name();
				buffer.replace(pos, 8, name);
				pos += name.size();
			}
		}
		if (pokemon != MISSINGNO){
			usz pos;
			while ((pos = buffer.find("[POKEMON]")) != std::string::npos){
				std::string_view name = names[static_cast<u16>(pokemon)];
				buffer.replace(pos, 9, name);
				pos += name.size();
			}
		}
		if (!custom.empty()){
			usz pos;
			while ((pos = buffer.find("[]")) != std::string::npos){
				buffer.replace(pos, 2, custom);
				pos += custom.size();
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
