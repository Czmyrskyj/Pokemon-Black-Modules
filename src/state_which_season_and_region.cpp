#include "st-ctx.hpp"
#include "time.hpp"

which_season_and_region_t::which_season_and_region_t(Color& bg_color, Locale::locale_t& locale, Time::time_t& time){
	bg_color = BLACK;
	season_label = locale.get(Locale::TYPE::UI,
			[&]() -> std::string{
			switch (time.get_season()){
			case Time::SEASON::WINTER: return "WINTER";
			case Time::SEASON::SPRING: return "SPRING";
			case Time::SEASON::SUMMER: return "SUMMER";
			case Time::SEASON::AUTUMN: return "AUTUMN";
			default: return "SUMMER";
			}
			}()
			);
	season_label_color = [&]() -> Color{
		switch (time.get_season()){
			case Time::SEASON::WINTER: return {160, 217, 219, 255};
			case Time::SEASON::SPRING: return {249, 129, 243, 255};
			case Time::SEASON::SUMMER: return {137, 249, 129, 255};
			case Time::SEASON::AUTUMN: return {201, 111, 32, 255};
			default: return {137, 249, 129, 255};
		}
	}();
	std::string hh_mm_unova = std::format("{}:{}, {}", time.get_time().hours().count(), time.get_time().minutes().count(), locale.get(Locale::TYPE::UI, "UNOVA"));
	writer.set_text(hh_mm_unova, 0.08f);
}

void which_season_and_region_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
		ctx.halt_music = true;
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	if (ctx.set_state_meta){
		Sound char_sfx = ctx.assets.get_sfx("SEQ_SE_TB_KARA.wav");
		Sound start_sfx = ctx.assets.get_sfx("SEQ_SE_TB_KON.wav");
		writer.update(&char_sfx, &start_sfx);
	}
}
void which_season_and_region_t::render(st_ctx_t& ctx){
	draw_text(ctx.assets, season_label, 500, 300, 150, season_label_color, Color{u8(season_label_color.r - 30),
			u8(season_label_color.g - 30),
			u8(season_label_color.b - 30),
			season_label_color.a});
	if (ctx.set_state_meta){
		draw_text_seq(ctx.assets, writer, 500, 600, 100, WHITE);
	}
}

