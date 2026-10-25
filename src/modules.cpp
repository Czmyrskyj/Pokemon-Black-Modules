#include "modules.hpp"
#include <filesystem>
#include <fstream>
#include <raylib.h>
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
	u16 next_slot[4] = {};
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
			.idx = next_slot[module_h.rarity]++,
		});
	}

	return {};
}

constexpr float mod_w{50};
constexpr float mod_h{50};
constexpr usz row_count{10};

#include "assets.hpp"

void module_t::render(assets_t& assets, Rectangle dest) const{
	const std::string spritesheet = [&]() -> std::string{
		switch (rarity){
			case MODULE_RARITY::COMMON: return "common-modules.png";
			case MODULE_RARITY::UNCOMMON: return "uncommon-modules.png";
			case MODULE_RARITY::RARE: return "rare-modules.png";
			case MODULE_RARITY::EXCEPTIONAL: return "exceptional-modules.png";
		}
		return "";
	}();
	const Rectangle src{static_cast<usz>((idx % row_count)) * mod_w, static_cast<usz>((idx / row_count)) * mod_h, mod_w, mod_h};
	DrawTexturePro(assets.get_tx(spritesheet), src, dest, {0, 0}, 0, WHITE);
}
