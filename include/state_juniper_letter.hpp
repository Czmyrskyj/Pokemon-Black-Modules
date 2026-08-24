#pragma once

struct st_ctx_t;
class trainer_t;
namespace JuniperLetter{
	void init(st_ctx_t& ctx, trainer_t& player);
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
};
