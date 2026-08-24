#pragma once

struct st_ctx_t;
class trainer_t;
namespace JuniperLetter{
	void update(st_ctx_t& ctx, trainer_t& player);
	void render(st_ctx_t& ctx);
};
