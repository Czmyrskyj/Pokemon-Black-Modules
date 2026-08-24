#include "state_juniper_letter.hpp"
#include "st-ctx.hpp"
#include "writer.hpp"
#include "formatter.hpp"
#include "trainer.hpp"
#include <raylib.h>

namespace JuniperLetter{
	static writer_t writer;
	static writer_t sincerely_writer;
	static std::string formatted_juniper_letter;
	static std::string formatted_sincerely;
	static bool initialized = false;
	void init(st_ctx_t& ctx, trainer_t& player){	
		formatted_juniper_letter = fmt::format(ctx.locale.get(Locale::TYPE::DIALOG, "JUNIPER_LETTER"), &player);
		formatted_sincerely = fmt::format(ctx.locale.get(Locale::TYPE::DIALOG, "SINCERELY_JUNIPER"), &player);
		writer.set_text(formatted_juniper_letter);
		sincerely_writer.set_text(formatted_sincerely);
	}
	void update(st_ctx_t& ctx, trainer_t& player){
		if (!initialized){
			init(ctx, player);
			initialized = true;
		}
		writer.update();
		if (writer.finished){
			sincerely_writer.update();
		}
		 if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			 ctx.resume_state_alpha(ctx.transition_alpha);
		 }
		 if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			 ctx.current_music = ctx.assets.get_mus("Professor Juniper.mp3");
			 ctx.set_state_meta = true;
		 }
	}
	void render(st_ctx_t& ctx){
		DrawTextureEx(ctx.assets.get_tx("mincinno-mail.png"), {300, 100}, 0, 5, WHITE);
		draw_text_seq(ctx.assets, writer, 400, 215, 38, BLACK, 0, BLANK);
		if (writer.finished){
			draw_text_seq(ctx.assets, sincerely_writer, 800, 850, 60, BLACK, 0, BLANK);
		}
	}
};
