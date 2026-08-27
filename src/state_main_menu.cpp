#include "state_main_menu.hpp"
#include "st-ctx.hpp"
#include "writer.hpp"
#include "switch_state.hpp"
#include <memory>
#include <cmath>
namespace MainMenu{
	struct state_meta_t{
		i8 switch_state_in_main_menu = -1;
		std::vector<std::string_view> menu_options;
	};
	static std::unique_ptr<state_meta_t> meta;

	void init(st_ctx_t& ctx){
		meta = std::make_unique<state_meta_t>();
		meta->menu_options.clear();
		meta->menu_options.push_back(ctx.locale.get(Locale::TYPE::UI, "MM_NEW"));
		meta->menu_options.push_back(ctx.locale.get(Locale::TYPE::UI, "MM_WIFI"));
		meta->switch_state_in_main_menu = -1; 
	}
	void update(st_ctx_t& ctx){
		if (!meta){
			init(ctx);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha > 0.0f){
			ctx.resume_state_alpha(ctx.transition_alpha);
		}
		if (!ctx.set_state_meta && ctx.transition_alpha <= 0.0f){
			ctx.set_state_meta = true;
		}
		Sound sel_sfx = ctx.assets.get_sfx("SEQ_SE_SELECT1.wav");
		Sound dec_sfx = ctx.assets.get_sfx("SEQ_SE_DECIDE1.wav");
		i8 opt;
		if (ctx.transition_alpha == 0){
			opt = ctx.control_selection(ctx.selected_option, meta->menu_options.size() - 1, sel_sfx, dec_sfx);
			if (opt != -1 && meta->switch_state_in_main_menu == -1){
				meta->switch_state_in_main_menu = opt;
			}
		}


		switch (meta->switch_state_in_main_menu){
			case 0:
				switch_state(ctx, STATE::BOY_OR_GIRL);
				break;
			case 1:
				switch_state(ctx, STATE::WIFI_CONNECTION);
				break;
			case -1:
			default:
				break;
		}
	}
	void render(st_ctx_t& ctx){
		if (!meta){
			return;
		}
		auto mntx = ctx.assets.get_tx("menu10.png");
		DrawTexturePro(mntx, {0, 0, static_cast<float>(mntx.width), static_cast<float>(mntx.height)}, {0, 0, wnd_w, 1537}, {0, 0}, 0.0f, {150, 150, 150, 255});
		u8 selected_button_c = static_cast<u8>(60 + std::sin(GetTime() * 3) * 50);
		for (usz i = 0; i < meta->menu_options.size(); i++){
			auto button = ctx.selected_option == i ? ctx.assets.get_tx("button-selected.png") : ctx.assets.get_tx("button.png");
			Color c = (ctx.selected_option == i) ? Color{static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), 220} : Color{255, 255, 255, 200};
			DrawTextureEx(button, {300, 300 + i * 100.0f}, 0.0f, 4.0f, c);
			draw_text(ctx.assets, meta->menu_options[i], 350, 330 + i * 100.0f, 50, WHITE);
		}
	}
	void clear(){
		meta.reset();
	}
}
