#pragma once

struct st_ctx_t;
struct trainer_t;
namespace ChooseAStarter{
	void update(st_ctx_t& ctx, trainer_t& trainer, trainer_t& cheren, trainer_t& bianca);
	void render(st_ctx_t& ctx);
	void clear();
};
