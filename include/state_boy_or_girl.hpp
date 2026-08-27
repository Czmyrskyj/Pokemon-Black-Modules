#pragma once

struct st_ctx_t;
namespace BoyOrGirl{
	void init(st_ctx_t& ctx);
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	void clear();
};
