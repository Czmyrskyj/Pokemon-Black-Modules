#include "state_choose_a_starter.hpp"
#include "st-ctx.hpp"
#include "dialog.hpp"
#include "formatter.hpp"

namespace ChooseAStarter{
	static dialog_t dialog;
	static writer_t writer;
	static bool initialized = false;
	static std::string current_line;
	void init(st_ctx_t& ctx){
		dialog.init({
			"CHAR_CHEREN_1_1",
			"CHAR_BIANCA_1_1",
			"CHAR_CHEREN_1_2",
			"CHAR_BIANCA_1_2",
			});
	}
	void update(st_ctx_t& ctx, Locale::locale_t& locale, trainer_t& trainer){
		if (!initialized){
			init(ctx);
			current_line = fmt::format(dialog.step(ctx.assets, writer, locale), &trainer);
			writer.set_text(current_line);
			initialized = true;
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			ctx.resume_state_alpha(ctx.transition_alpha);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		if (dialog.update(ctx.assets, writer) && writer.finished){
			current_line = fmt::format(dialog.step(ctx.assets, writer, locale), &trainer);
			writer.set_text(current_line);
		}
		writer.update();
	}
	void render(st_ctx_t& ctx){
		draw_text_seq(ctx.assets, writer, dialog.txtbox_x, dialog.txtbox_y, 50, WHITE, 1);
		dialog.draw_speaker(ctx.assets);
	}
};
