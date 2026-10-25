#include "modules.hpp"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

std::expected<void, std::string_view> moduledex_t::load_moduledex(){
	if (!fs::exists("MODULEDEX")){
		return std::unexpected("MODULEDEX is absent.");
	}
	std::ifstream in("MODULEDEX", std::ios::binary);
	if (!in.is_open()){
		return std::unexpected("failed to open MODULEDEX");
	}
	
	u32 count;
	in.read(reinterpret_cast<char*>(&count), sizeof(u32));

	if (count >= 100'000){
		return std::unexpected("bad module count");
	}
	for (u32 i = 0; i < count; i++){
		module_header_t module_h;
		in.read(reinterpret_cast<char*>(&module_h), sizeof(module_header_t));
		std::vector<module_effect_header_t> effects_h(module_h.effect_count);
		in.read(reinterpret_cast<char*>(effects_h.data()), module_h.effect_count * sizeof(module_effect_header_t));
		std::vector<module_effect_t> effects;
		effects.reserve(module_h.effect_count);
		for (const auto& e : effects_h){
			effects.push_back(module_effect_t{
				.trigger = static_cast<MOD_EFF::TRIGGER>(e.trigger),
				.condition = static_cast<MOD_EFF::CONDITION>(e.condition),
				.cond_param = e.cond_param,
				.action = static_cast<MOD_EFF::ACTION>(e.action),
				.value = e.value,
				.stat = static_cast<MOD_EFF::STAT>(e.stat),
				.target = static_cast<MOD_EFF::TARGET>(e.target),
				
			});
		}
		u8 name_len;
		in.read(reinterpret_cast<char*>(&name_len), sizeof(u8));
		std::string name(name_len, '\0');
		in.read(name.data(), name_len);

		u16 desc_len;
		in.read(reinterpret_cast<char*>(&desc_len), sizeof(u16));
		std::string desc(desc_len, '\0');
		in.read(desc.data(), desc_len);

		module_registry.push_back(module_t{
			.name = std::move(name),
			.description = std::move(desc),
			.rarity = static_cast<MODULE_RARITY>(module_h.rarity),
			.cost = module_h.cost,
			.effects = std::move(effects),
		});
	}

	return {};
}
