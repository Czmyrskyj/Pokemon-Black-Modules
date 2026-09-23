#pragma once
#include "common.hpp"

enum class POKEBALL{
	MASTER_BALL,
	ULTRA_BALL,
	GREAT_BALL,
	POKE_BALL,
	SAFARI_BALL,
	HEAVY_BALL,
	LEVEL_BALL,
	LURE_BALL,
	FAST_BALL,
	FRIEND_BALL,
	MOON_BALL,
	LOVE_BALL,
	PARK_BALL,
	SPORT_BALL,
	PREMIER_BALL,
	REPEAT_BALL,
	TIMER_BALL,
	NEST_BALL,
	NET_BALL,
	DIVE_BALL,
	LUXURY_BALL,
	HEAL_BALL,
	QUICK_BALL,
	DUSK_BALL,
	CHERISH_BALL
};

struct thrown_pokeball_renderer_t{
	float frame_timer = 0.0f;
	u8 frame = 0;
	u8 spinned = 0;
	bool thrown = false;
	bool opened = false;
	void draw(const Texture2D& texture, POKEBALL pokeball, Vector3 pos, const Camera3D& camera);
};
