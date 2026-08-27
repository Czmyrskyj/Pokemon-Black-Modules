#pragma once

struct st_ctx_t;
class trainer_t;
namespace StarterSelected{
	void update(st_ctx_t& ctx, trainer_t& player, trainer_t& cheren);
	void render(st_ctx_t& ctx);
	void clear();
};
