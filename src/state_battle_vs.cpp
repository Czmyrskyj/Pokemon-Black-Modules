#include "states.hpp"
#include "st-ctx.hpp"
#include "random.hpp"
#include "music_name.hpp"
#include <format>


battle_vs_t::battle_vs_t(st_ctx_t& ctx){
	ctx.bg_color = BLACK;
	ctx.music_volume = 1.0f;
	ctx.set_music(get_music_name(ctx));
}

void battle_vs_t::update(st_ctx_t& ctx){
	if (!ctx.set_state_meta && ctx.transition_alpha > 0){
		ctx.resume_state_alpha(ctx.transition_alpha);
	}
	if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
		ctx.set_state_meta = true;
	}
	if (ctx.set_state_meta){
		if (flash_count < 3){
			if (!flashed){
				if (flash_alpha < 250){
					flash_alpha += 25;
				} else {
					flash_count++;
					flashed = true;
				}
			} else {
				if (flash_alpha > 0){
					flash_alpha -= 25;
				} else {
					flashed = false;
				}
			}
		} else {
			if (after_three_flashes_timer < 50){
				after_three_flashes_timer++;
			} else {
				if (banner_x < 0.0f){
					if (flash_alpha > 0.0f){
						flash_alpha -= 25;
					}
					banner_x += 40.0f;
				} else {
					if (portrait_vel >= 40.0f) { shake.y = rnd::get_val<float>(-8.0f, 8.0f);
					} else {
						shake.y = 0.0f;
					}
					banner_x = 0.0f;
					if (portrait_vel >= -20.0f){
						portrait_x += portrait_vel;
						portrait_vel -= 2.5f;
					} else {
						if (!flashed_last_time){
							if (flash_alpha < 250){
								flash_alpha += 25;
							} else {
								PlaySound(ctx.assets.get_sfx("SEQ_SE_SHDEMO_04.wav"));
								flashed_last_time = true;
							}
						} else if (flash_alpha > 0){
							flash_alpha -= 25;
						}
						after_flashed_last_time_timer++;
						if (after_flashed_last_time_timer > 40 && after_flashed_last_time_timer < 80){
							shake.x = rnd::get_val<float>(-4.0f, 4.0f);
							shake.y = rnd::get_val<float>(-4.0f, 4.0f);
						} else if (after_flashed_last_time_timer > 80){
							if (!played_vs_whoosh){
								PlaySound(ctx.assets.get_sfx("SEQ_SE_TDEMO_003.wav"));
								played_vs_whoosh = true;
							}
							shake = {0, 0};
							if (vs_scale < 45.0f){
								vs_scale += 1.0f;
							} else {
								ctx.switch_state(STATE::BATTLE, 600);
								return;
							}
						}
					}
				}
				source_x+=4.0f;
				if (source_x >= 128.0f){
					source_x = 0.0f;
				}
			}
		}
	}
}

constexpr float vs_w{61.0f};
constexpr float vs_h{54.0f};

void battle_vs_t::render(st_ctx_t& ctx){
	DrawTexturePro(ctx.assets.get_tx("banner1.png"), {source_x, 0, bn_w, bn_h}, {wnd_w - bn_w*7.5f - banner_x, 400 + (!flashed_last_time ? shake.y : 0.0f), bn_w * 8.0f, bn_h * 4.0f}, {0, 0}, 0, WHITE);
	DrawTexturePro(ctx.assets.get_tx("banner0.png"), {-source_x, 0, bn_w, bn_h}, {banner_x, 400 + (!flashed_last_time ? shake.y : 0.0f), bn_w * 7.5f, bn_h * 4.0f}, {0, 0}, 0, WHITE);
	anim_trainer_render.draw_vs(ctx.assets, ctx.player.sex() == SEX::MALE ? "Hilbert" : "Hilda", {portrait_x, 400}, WHITE);
	anim_trainer_render.draw_vs(ctx.assets, ctx.bt_info.name, {wnd_w - portrait_x - bn_w*4.0f, 400}, flashed_last_time ? WHITE : BLACK);
	if (flashed_last_time){
		draw_text(ctx.assets, ctx.player.name(), portrait_x + ((bn_w*4.0f) / 2.0f), 400 + bn_h*4.0f + 5.0f, 50, WHITE, BLACK);
		draw_text(ctx.assets, ctx.bt_info.name, wnd_w - portrait_x - ((bn_w*4.0f) / 2.0f), 400 + bn_h*4.0f + 5.0f, 50, WHITE, BLACK);
		DrawTextureEx(ctx.assets.get_tx("vs.png"), {wnd_w / 2.0f - vs_w*vs_scale / 2.0f + shake.x, (400 - vs_h*vs_scale / 2.0f) + 150.0f + shake.y}, 0, vs_scale, WHITE);
	}
	DrawRectangle(0, 0, wnd_w, wnd_h, {255, 255, 255, flash_alpha});
}
