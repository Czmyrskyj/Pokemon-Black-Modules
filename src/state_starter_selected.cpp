#include "state_starter_selected.hpp"
#include "st-ctx.hpp"
#include "formatter.hpp"
#include "trainer.hpp"
#include "writer.hpp"
#include "dialog.hpp"

#include <cmath>
#include <memory>

namespace StarterSelected{
	struct state_meta_t{
		bool played_received_starter = false;
		bool init_dialog = false;
		std::string formatted_string;
		std::string current_line;
		writer_t writer;
		dialog_t dialog;
	};
	static std::unique_ptr<state_meta_t> meta;
	void init(st_ctx_t& ctx, trainer_t& player, trainer_t& cheren){
		if (!meta){
			meta = std::make_unique<state_meta_t>();
		}
		meta->dialog.top = true;
		ctx.bg_color = WHITE;
		meta->formatted_string = fmt::format(ctx.locale.get(Locale::TYPE::UI, "RECEIVED_POKEMON"), &player, player.get_party().data()[0].pokedex.id);
		meta->dialog.init({
				"CHAR_BIANCA_2_1",
				"CHAR_CHEREN_2_2",
				"CHAR_CHEREN_2_3",
				});
	}
	void update(st_ctx_t& ctx, trainer_t& player, trainer_t& cheren){
		if (!meta){
			init(ctx, player, cheren);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0){
			ctx.resume_state_alpha(ctx.transition_alpha);
			ctx.halt_music = true;
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		Sound received_starter_sound = ctx.assets.get_sfx("received-starter.wav");
		if (!meta->played_received_starter){
			PlaySound(received_starter_sound);
			meta->played_received_starter = true;
		} else if (!IsSoundPlaying(received_starter_sound)){
			if (!meta->init_dialog){
				meta->current_line = fmt::format(meta->dialog.step(ctx.assets, meta->writer, ctx.locale), nullptr, cheren.get_party().data()[0].pokedex.id);
				meta->writer.set_text(meta->current_line);
				meta->init_dialog = true;
			}
			if (meta->dialog.update(ctx.assets, meta->writer) && meta->writer.finished){
				meta->current_line = fmt::format(meta->dialog.step(ctx.assets, meta->writer, ctx.locale), nullptr, cheren.get_party().data()[0].pokedex.id);
				meta->writer.set_text(meta->current_line);
			}
		}
		meta->writer.update();
	}
	void render(st_ctx_t& ctx){
		if (!meta){
			return;
		}
		for (usz i = 0; i < meta->formatted_string.size(); i++){
			const char c[2]{meta->formatted_string[i], '\0'};
			draw_text(ctx.assets, c, 10 + i * 30, std::sinf((10 + i * 30) + (GetTime() * 2.0f)) * 5.0f + 1000, 50, BLACK);
		}
		draw_text_seq(ctx.assets, meta->writer, meta->dialog.txtbox_x, meta->dialog.txtbox_y, 50, WHITE, 1);
		meta->dialog.draw_speaker(ctx.assets);
	}
	void clear(){
		meta = nullptr;
	}
}
