#pragma once

struct st_ctx_t;
namespace WhatsYourName{
	void init(st_ctx_t& ctx);
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	void clear();
}
