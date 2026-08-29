#include "st-ctx.hpp"
#include "time.hpp"

which_season_and_region_t::which_season_and_region_t(st_ctx_t& ctx){
	ctx.bg_color = BLACK;
	season_label = ctx.locale.get(Locale::TYPE::UI,
			[&]() -> std::string{
			switch (ctx.time.get_season()){
			case Time::SEASON::WINTER: return "WINTER";
			case Time::SEASON::SPRING: return "SPRING";
			case Time::SEASON::SUMMER: return "SUMMER";
			case Time::SEASON::AUTUMN: return "AUTUMN";
			default: return "SUMMER";
			}
			}()
			);
	season_label_color = [&]() -> Color{
		switch (ctx.time.get_season()){
			case Time::SEASON::WINTER: return {160, 217, 219, 255};
			case Time::SEASON::SPRING: return {249, 129, 243, 255};
			case Time::SEASON::SUMMER: return {137, 249, 129, 255};
			case Time::SEASON::AUTUMN: return {201, 111, 32, 255};
			default: return {137, 249, 129, 255};
		}
	}();
	std::string hh_mm_unova = std::format("{}:{}, {}", ctx.time.get_time().hours().count(), ctx.time.get_time().minutes().count(), ctx.locale.get(Locale::TYPE::UI, "UNOVA"));
	writer.set_text(hh_mm_unova, 0.08f);
	region_writer.set_text(get_region_str(ctx.region), 0.12f);
}

void which_season_and_region_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.current_music = ctx.assets.get_mus(std::format("{}.ogg", get_region_str(ctx.region)));
		ctx.set_state_meta = true;
	}
	Sound char_sfx = ctx.assets.get_sfx("SEQ_SE_TB_KARA.wav");
	Sound start_sfx = ctx.assets.get_sfx("SEQ_SE_TB_KON.wav");
	if (ctx.set_state_meta){
		writer.update(&char_sfx, &start_sfx);
	}
	if (writer.finished){
		region_writer.update(&char_sfx, nullptr);
		ctx.halt_music = false;
	}
	if (region_writer.finished){
		ctx.switch_state(STATE::MAIN, 200);
	}
}
void which_season_and_region_t::render(st_ctx_t& ctx){
	draw_text(ctx.assets, season_label, 600, 300, 200, season_label_color, Color{u8(season_label_color.r - 30),
			u8(season_label_color.g - 30),
			u8(season_label_color.b - 30),
			season_label_color.a});
	if (ctx.set_state_meta){
		draw_text_seq(ctx.assets, writer, 600, 600, 50, WHITE);
		draw_text_seq(ctx.assets, region_writer, 600, 670, 80, WHITE);
	}
}

