#pragma once

#include <string>
#include <vector>
#include "common.hpp"
#include "items.hpp"
#include "pokemon.hpp"
#include "pokeball.hpp"

struct catch_ctx_t{
	u8 turn = 1;
	bool dark = false;
	bool underwater = false;
	bool fishing = false;
	bool owned_species = false;
	u8 ally_level = 1;
	SEX ally_sex = SEX::NONE;
	POKEDEX ally_species = MISSINGNO;
};

enum class ITEM_RESULT : u8{
	CANT_USE,
	HEALED,
	REVIVED,
	CAUGHT,
	BROKE_FREE,
	TRAINER_BLOCK,
};

struct item_result_t{
	ITEM_RESULT kind = ITEM_RESULT::CANT_USE;
	u16 amount = 0;
	u8 shakes = 0;
	bool consumed() const { return kind != ITEM_RESULT::CANT_USE; }
};

bool can_use_item(const item_t& item, const pokemon_t& target);
item_result_t apply_item(const item_t& item, pokemon_t& target, bool wild_battle, const catch_ctx_t& cctx);
item_result_t attempt_catch(POKEBALL ball, const pokemon_t& foe, const catch_ctx_t& cctx);
