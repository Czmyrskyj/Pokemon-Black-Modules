#pragma once

struct st_ctx_t;
struct trainer_t;
namespace ChooseAStarter{
	void update(st_ctx_t& ctx, trainer_t& trainer);
	void render(st_ctx_t& ctx);
};
