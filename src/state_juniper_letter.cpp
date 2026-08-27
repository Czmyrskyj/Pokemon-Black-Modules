#include "state_juniper_letter.hpp"
#include "st-ctx.hpp"
#include "writer.hpp"
#include "formatter.hpp"
#include "trainer.hpp"
#include "switch_state.hpp"

#include <memory>

namespace JuniperLetter{
	struct state_meta_t{
		writer_t writer;
		writer_t sincerely_writer;
		std::string formatted_juniper_letter;
		std::string formatted_sincerely;
	};
	static std::unique_ptr<state_meta_t> meta;
	void init(st_ctx_t& ctx, trainer_t& player){
		if (!meta){
			meta = std::make_unique<state_meta_t>();
		}
		meta->formatted_juniper_letter = fmt::format(ctx.locale.get(Locale::TYPE::DIALOG, "JUNIPER_LETTER"), &player);
		meta->formatted_sincerely = fmt::format(ctx.locale.get(Locale::TYPE::DIALOG, "SINCERELY_JUNIPER"), &player);
		meta->writer.set_text(meta->formatted_juniper_letter, 0.03f);
		meta->sincerely_writer.set_text(meta->formatted_sincerely, 0.03f);
	}
	void update(st_ctx_t& ctx, trainer_t& player){
		if (!meta){
			init(ctx, player);
		}
		meta->writer.update();
		if (meta->writer.finished){
			meta->sincerely_writer.update();
		}
		if (meta->sincerely_writer.finished){
			switch_state(ctx, STATE::CHOOSE_STARTER);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			ctx.current_music = ctx.assets.get_mus("Professor Juniper.mp3");
			ctx.resume_state_alpha(ctx.transition_alpha);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
	}
	void render(st_ctx_t& ctx){
		if (!meta){
			return;
		}
		DrawTextureEx(ctx.assets.get_tx("mincinno-mail.png"), {300, 100}, 0, 5, WHITE);
		draw_text_seq(ctx.assets, meta->writer, 390, 215, 38, BLACK, 0, BLANK);
		if (meta->writer.finished){
			draw_text_seq(ctx.assets, meta->sincerely_writer, 800, 850, 60, BLACK, 0, BLANK);
		}
	}
	void clear(){
		meta.reset();
	}
};
