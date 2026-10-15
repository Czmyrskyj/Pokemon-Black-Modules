#include "formatter.hpp"
#include "names.hpp"

void formatter_t::feed(std::string key, std::string value){
	format_variables[std::move(key)] = std::move(value);
}

std::string formatter_t::format(std::string_view txt, POKEDEX pokemon, std::string_view freestanding){
	std::string buffer(txt);
	usz pos = 0;
	while ((pos = buffer.find('[', pos)) != std::string::npos){
		auto close = buffer.find(']', pos);
		if (close == std::string::npos){
			break;
		}
		std::string key = buffer.substr(pos, close - pos + 1);
		auto it = format_variables.find(key);
		if (it != format_variables.end()){
			const auto& val = it->second;
			buffer.replace(pos, key.size(), val);
			pos += val.size();
		} else {
			pos = close + 1;
		}
	}
	if (pokemon != MISSINGNO){
		pos = 0;
		while ((pos = buffer.find("[POKEMON]")) != std::string::npos){
			std::string_view name = names[pokemon];
			buffer.replace(pos, 9, name);
		}
	}
	if (!freestanding.empty()){
		pos = 0;
		while ((pos = buffer.find("[FREESTANDING]")) != std::string::npos){
			buffer.replace(pos, 14, freestanding);
		}
	}
	pos = 0;
	while ((pos = buffer.find("[WAIT]")) != std::string::npos){
		buffer.replace(pos, 6, "\x02");
	}
	pos = 0;
	while ((pos = buffer.find("[PKMN_SIGN]")) != std::string::npos){
		buffer.replace(pos, 11, "\x01");
	}
	pos = 0;
	while ((pos = buffer.find("[START_CHAR_MUSIC]")) != std::string::npos){
		buffer.replace(pos, 18, "\x03");
	}

	return buffer;
}
