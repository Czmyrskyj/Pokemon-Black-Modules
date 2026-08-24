#pragma once

struct st_ctx_t;
namespace Locale{
	struct locale_t;
}
struct trainer_t;
namespace ChooseAStarter{
	void update(st_ctx_t& ctx, Locale::locale_t& locale, trainer_t& trainer);
	void render(st_ctx_t& ctx);
};
