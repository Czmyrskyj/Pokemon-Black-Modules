#include "state_main_menu.hpp"
#include "st-ctx.hpp"
#include "writer.hpp"
#include <cmath>
namespace MainMenu{
	static i8 switch_state_in_main_menu = -1;
	static std::vector<std::string_view> menu_options;
	void init(st_ctx_t& ctx){
		menu_options.clear();
		menu_options.push_back(ctx.locale.get(Locale::TYPE::UI, "MM_NEW"));
		menu_options.push_back(ctx.locale.get(Locale::TYPE::UI, "MM_WIFI"));
		switch_state_in_main_menu = -1; 
	}
	void update(st_ctx_t& ctx){
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
			opt = ctx.control_selection(ctx.selected_option, 0, menu_options.size() - 1, sel_sfx, dec_sfx);
			if (opt != -1 && switch_state_in_main_menu == -1){
				switch_state_in_main_menu = opt;
			}
		}


		switch (switch_state_in_main_menu){
			case 0:
				ctx.switch_state(STATE::BOY_OR_GIRL, ctx.transition_alpha);
				break;
			case 1:
				ctx.switch_state(STATE::WIFI_CONNECTION, ctx.transition_alpha);
				break;
			case -1:
			default:
				break;
		}
	}
	void render(st_ctx_t& ctx){
		auto mntx = ctx.assets.get_tx("menu10.png");
		DrawTexturePro(mntx, {0, 0, static_cast<float>(mntx.width), static_cast<float>(mntx.height)}, {0, 0, wnd_w, 1537}, {0, 0}, 0.0f, {150, 150, 150, 255});
		u8 selected_button_c = static_cast<u8>(60 + std::sin(GetTime() * 3) * 50);
		for (usz i = 0; i < menu_options.size(); i++){
			auto button = ctx.selected_option == i ? ctx.assets.get_tx("button-selected.png") : ctx.assets.get_tx("button.png");
			Color c = (ctx.selected_option == i) ? Color{static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), 220} : Color{255, 255, 255, 200};
			DrawTextureEx(button, {300, 300 + i * 100.0f}, 0.0f, 4.0f, c);
			draw_text(ctx.assets, menu_options[i], 350, 330 + i * 100.0f, 50, WHITE);
		}
	}
}
