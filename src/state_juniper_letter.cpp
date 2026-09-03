#include "st-ctx.hpp"
#include "writer.hpp"
#include "formatter.hpp"
#include "trainer.hpp"

juniper_letter_t::juniper_letter_t(st_ctx_t& ctx){
	formatted_juniper_letter = ctx.formatter.format(ctx.locale.get(Locale::TYPE::DIALOG, "JUNIPER_LETTER"));
	formatted_sincerely = ctx.formatter.format(ctx.locale.get(Locale::TYPE::DIALOG, "SINCERELY_JUNIPER"));
	writer.set_text(formatted_juniper_letter, 0.03f);
	sincerely_writer.set_text(formatted_sincerely, 0.03f);
}

void juniper_letter_t::update(st_ctx_t& ctx){
	writer.update();
	if (writer.finished){
		sincerely_writer.update();
	}
	if (sincerely_writer.finished){
		ctx.switch_state(STATE::CHOOSE_STARTER);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
		ctx.set_music("Professor Juniper.ogg");
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
}
void juniper_letter_t::render(st_ctx_t& ctx){
	DrawTextureEx(ctx.assets.get_tx("mincinno-mail.png"), {300, 100}, 0, 5, WHITE);
	draw_text_seq(ctx.assets, writer, 390, 215, 38, BLACK, 0, BLANK);
	if (writer.finished){
		draw_text_seq(ctx.assets, sincerely_writer, 800, 850, 60, BLACK, 0, BLANK);
	}
}

void juniper_letter_t::clear(assets_t& assets){
	assets.unload_texture("mincinno-mail.png");
}
