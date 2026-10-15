#include "item_effects.hpp"
#include "random.hpp"

namespace {

	bool is_moon_family(POKEDEX p){
		switch (p){
			case NIDORAN_F: case NIDORINA: case NIDOQUEEN:
			case NIDORAN_M: case NIDORINO: case NIDOKING:
			case CLEFFA: case CLEFAIRY: case CLEFABLE:
			case IGGLYBUFF: case JIGGLYPUFF: case WIGGLYTUFF:
			case SKITTY: case DELCATTY:
			case MUNNA: case MUSHARNA:
				return true;
			default: return false;
		}
	}

	float ball_modifier(POKEBALL ball, const pokemon_t& foe, const catch_ctx_t& c){
		const auto& dex = foe.get_pokedex();
		switch (ball){
			case POKEBALL::ULTRA_BALL:	return 2.0f;
			case POKEBALL::GREAT_BALL:
			case POKEBALL::SAFARI_BALL:
			case POKEBALL::SPORT_BALL:
			case POKEBALL::PARK_BALL:	return 1.5f;
			case POKEBALL::NET_BALL:
							return (dex.type1 == TYPE::WATER || dex.type2 == TYPE::WATER ||
									dex.type1 == TYPE::BUG   || dex.type2 == TYPE::BUG) ? 3.5f : 1.0f;
			case POKEBALL::DIVE_BALL:	return c.underwater ? 3.5f : 1.0f;
			case POKEBALL::NEST_BALL:
							return foe.lvl() < 30 ? std::max(1.0f, (41.0f - foe.lvl()) / 10.0f) : 1.0f;
			case POKEBALL::REPEAT_BALL:	return c.owned_species ? 3.0f : 1.0f;
			case POKEBALL::TIMER_BALL:	return std::min(4.0f, 1.0f + c.turn * (1229.0f / 4096.0f));
			case POKEBALL::QUICK_BALL:	return c.turn == 1 ? 4.0f : 1.0f;
			case POKEBALL::DUSK_BALL:	return c.dark ? 3.5f : 1.0f;
			case POKEBALL::FAST_BALL:	return dex.base_speed >= 100 ? 4.0f : 1.0f;
			case POKEBALL::LURE_BALL:	return c.fishing ? 3.0f : 1.0f;
			case POKEBALL::LEVEL_BALL:{
							  if (c.ally_level > foe.lvl() * 4) return 8.0f;
							  if (c.ally_level > foe.lvl() * 2) return 4.0f;
							  if (c.ally_level > foe.lvl())     return 2.0f;
							  return 1.0f;
						  }
			case POKEBALL::MOON_BALL:	return is_moon_family(dex.id) ? 4.0f : 1.0f;
			case POKEBALL::LOVE_BALL:
							return (dex.id == c.ally_species &&
									c.ally_sex != SEX::NONE && foe.sex() != SEX::NONE &&
									c.ally_sex != foe.sex()) ? 8.0f : 1.0f;
			case POKEBALL::HEAVY_BALL:
			default:			return 1.0f;
		}
	}

}

bool can_use_item(const item_t& item, const pokemon_t& target){
	switch (item.effect){
		case ITEM_EFFECT::HEAL: return target.hp() > 0 && target.hp() < target.max_hp();
		case ITEM_EFFECT::REVIVE: return target.hp() <= 0;
		case ITEM_EFFECT::CATCH: return target.hp() > 0;
		case ITEM_EFFECT::NONE:
		default: return false;
	}
}

item_result_t attempt_catch(POKEBALL ball, const pokemon_t& foe, const catch_ctx_t& cctx){
	if (ball == POKEBALL::MASTER_BALL) return {ITEM_RESULT::CAUGHT, 0, 4};
	
	const float max_hp = foe.max_hp();
	const float hp = foe.hp();

	float a = ((3.0f * max_hp - 2.0f * hp) * foe.get_pokedex().catch_rate * ball_modifier(ball, foe, cctx)) / (3.0f * max_hp);
	a = std::max(a, 1.0f);

	if (a >= 255.0f) return {ITEM_RESULT::CAUGHT, 0, 4};

	const float b = 1048560.0f / std::sqrt(std::sqrt(16711680.0f / a));
	u8 shakes = 0;
	for (; shakes < 4; shakes++){
		if (rnd::get_val<u32>(0, 65535) >= static_cast<u32>(b)) break;
	}
	return {shakes == 4 ? ITEM_RESULT::CAUGHT : ITEM_RESULT::BROKE_FREE, 0, shakes};
}

item_result_t apply_item(const item_t& item, pokemon_t& target, bool wild_battle, const catch_ctx_t& cctx){
	if (!can_use_item(item, target)) return {ITEM_RESULT::CANT_USE};
	const bool full = (item.effect_flags & ITEM_FLAG_FULL) != 0;
	switch (item.effect){
		case ITEM_EFFECT::HEAL:
			{
				const u16 before = static_cast<u16>(target.hp());
				target.hp_heal(full ? static_cast<u16>(target.max_hp()) : item.effect_arg);
				return {ITEM_RESULT::HEALED, static_cast<u16>(target.hp() - before), 0};
			}
		case ITEM_EFFECT::CATCH:
			{
				if (!wild_battle) return {ITEM_RESULT::TRAINER_BLOCK};
				return attempt_catch(static_cast<POKEBALL>(item.effect_arg), target, cctx);
			}
		case ITEM_EFFECT::REVIVE:
			{
				//target.revive(full);
				return {ITEM_RESULT::REVIVED, static_cast<u16>(target.hp()), 0};	
			}
		case ITEM_EFFECT::NONE:
		default: return {ITEM_RESULT::CANT_USE};
	}
}
