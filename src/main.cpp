#include <raylib.h>
#include "battle.hpp"
#include "trainer.hpp"
#include "assets.hpp"
#include "states.hpp"
#include "random.hpp"
#include "font.hpp"
#include <print>

constexpr int wnd_w{1920};
constexpr int wnd_h{1080};

constexpr float alpha_delta{300.0f};

STATE state = STATE::TITLE;
bool set_state_meta = false;
void switch_state(STATE new_state, unsigned char& alpha, float delta = alpha_delta){
	float current_alpha = static_cast<float>(alpha);
	if (state != new_state){
		current_alpha+=delta*GetFrameTime();
		if (alpha >= 255.0f){
			current_alpha = 255;
			state = new_state;
			set_state_meta = false;
		}
	}
	alpha = static_cast<unsigned char>(current_alpha);
}

void resume_state_alpha(unsigned char& alpha){
	float current_alpha = static_cast<float>(alpha);
	if (current_alpha > 5.0f){
		current_alpha -= alpha_delta*GetFrameTime();
	} else {
		current_alpha = 0.0f;
	}
	alpha = static_cast<unsigned char>(current_alpha);
}

void play_music(const Music& mus){
	if (!IsMusicStreamPlaying(mus)){
		PlayMusicStream(mus);
	}
	UpdateMusicStream(mus);
}

i8 control_selection(u8& opt, u8 min, u8 max, const Sound& sel, const Sound& dec, bool horizontal = false){
	if (IsKeyPressed(!horizontal ? KEY_DOWN : KEY_RIGHT)){
		opt++;
		if (opt > max) opt = 0;
		PlaySound(sel);
	} else if (IsKeyPressed(!horizontal ? KEY_UP : KEY_LEFT)){
		if (opt > 0){
			opt--;
		} else {
			opt = max - 1;
		}
		PlaySound(sel);
	}
	if (IsKeyPressed(KEY_Z)){
		PlaySound(dec);
		return opt;
	}
	return -1;
}

void draw_title_screen(assets_t& assets, bool pressed){
	constexpr float logo_scale{0.3f};
	static float strip_pos = 0.0f;
	static u8 press_enter_timer = 0;
	press_enter_timer++;
	if (press_enter_timer >= 50){
		press_enter_timer = 0;
	}
	strip_pos -= 100.0f*GetFrameTime();
	if (strip_pos < -wnd_w){
		strip_pos = -1.0f;
	}
	auto strip = assets.get_tx("title-strip.png");
	auto logo = assets.get_tx("pokemon-logo.png");
	for (usz i = 0; i < 43; i++){
		DrawTexture(strip, i * strip.width + strip_pos, 0, WHITE);
	}
	DrawTextureEx(logo, {wnd_w / 2.0f - (logo.width * logo_scale) / 2.0f, 90.0f}, 0.0f, logo_scale, WHITE);
	auto verfont = assets.get_font("version.otf");
	DrawTextEx(verfont, "black modules", {400.0f, 500.0f}, 120.0f, 1.0f, WHITE);
	DrawTextEx(verfont, "version", {730.0f, 650.0f}, 80.0f, 1.0f, WHITE);
	if (press_enter_timer < 25 && !pressed){
		DrawText("press ENTER", 750, 950, 50.0f, WHITE);
	}
}

std::vector<std::string_view> menu_options{
	"NEW GAME",
	"CONNECT VIA WIFI"
};

static constexpr std::string_view name_entry_chars{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789., "};

int main(){
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(wnd_w, wnd_h, "Pokemon Black Modules");
	InitAudioDevice();
	ToggleFullscreen();
	SetTargetFPS(60);
	{
		assets_t assets;
		auto assets_load_res = assets.load_assets();
		if (!assets_load_res.has_value()){
			std::println(stderr, "error: {}", assets_load_res.error());
			CloseAudioDevice();
			CloseWindow();
			return -1;
		}
		
		SetTextureFilter(assets.get_font("font.otf").texture, TEXTURE_FILTER_POINT);

		Music current_music{};
		unsigned char black_rect_alpha = 0;
		bool halt_music = false;
		bool title_pressed_enter = false;
		u8 menu_index = rnd::get_val<int>(1, 17);
		u8 selected_option = 0;
		i8 switch_state_in_main_menu = -1;
		float screen_x = 0.0f;
		u16 current_txt_index = 0;
		float txt_seq_timer = 0.0f;
		u8 char_frame_count = 0;
		u8 char_frame_count_timer = 0;

		trainer_t player{"nodata", ""};
		u8 selected_char = 0;
		u8 char_cursor_blinker = 0;
		std::string entered_name = "";

		while (!WindowShouldClose()){
			if (!halt_music){
				play_music(current_music);
			}
			BeginDrawing();
			ClearBackground(BLACK);
			switch (state){
				case STATE::TITLE:
					{
						if (!set_state_meta){
							current_music = assets.get_mus("Title.mp3");
							set_state_meta = true;
						}
					}
					draw_title_screen(assets, halt_music);
					if (IsKeyPressed(KEY_ENTER) && !title_pressed_enter){
						title_pressed_enter = true;
						StopMusicStream(current_music);
						PlaySound(assets.get_cry(POKEDEX::RESHIRAM));
					}
					if (title_pressed_enter){
						halt_music = true;
						switch_state(STATE::MAIN_MENU, black_rect_alpha, 200.0f);
					}
					break;
				case STATE::MAIN_MENU:
					{
						if (!set_state_meta && black_rect_alpha > 0.0f){
							resume_state_alpha(black_rect_alpha);
						}
						if (!set_state_meta && black_rect_alpha <= 0.0f){
							set_state_meta = true;
						}
						auto mntx = assets.get_tx("menu10.png");

						DrawTexturePro(mntx, {0, 0, static_cast<float>(mntx.width), static_cast<float>(mntx.height)}, {0, 0, wnd_w, 1537}, {0, 0}, 0.0f, {150, 150, 150, 255});
						u8 selected_button_c = static_cast<u8>(60 + std::sin(GetTime() * 3) * 50);
						Sound sel_sfx = assets.get_sfx("SEQ_SE_SELECT1.wav");
						Sound dec_sfx = assets.get_sfx("SEQ_SE_DECIDE1.wav");
						i8 opt;
						if (black_rect_alpha == 0){
							opt = control_selection(selected_option, 0, menu_options.size() - 1, sel_sfx, dec_sfx);
							if (opt != -1 && switch_state_in_main_menu == -1){
								switch_state_in_main_menu = opt;
							}
						}
						for (usz i = 0; i < menu_options.size(); i++){
							auto button = selected_option == i ? assets.get_tx("button-selected.png") : assets.get_tx("button.png");
							Color c = (selected_option == i) ? Color{static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), static_cast<u8>(255 - selected_button_c), 220} : Color{255, 255, 255, 200};
							DrawTextureEx(button, {300, 300 + i * 100.0f}, 0.0f, 4.0f, c);
							draw_text(assets, menu_options[i], 350, 330 + i * 100.0f, 50, WHITE);
						}


						switch (switch_state_in_main_menu){
							case 0:
								switch_state(STATE::BOY_OR_GIRL, black_rect_alpha);
								break;
							case 1:
								switch_state(STATE::WIFI_CONNECTION, black_rect_alpha);
								break;
							case -1:
							default:
								break;
						}
					}
					break;
				case STATE::WIFI_CONNECTION:
					{
						if (!set_state_meta && black_rect_alpha > 0.0f){
							resume_state_alpha(black_rect_alpha);
						}
						if (!set_state_meta && black_rect_alpha <= 0.0f){
							current_music = assets.get_mus("Wi-Fi Connection.mp3");
							halt_music = false;
							set_state_meta = true;
						}
						auto background = assets.get_tx("wifi-connection-bg.png");
						DrawTexturePro(background, {0, 0, static_cast<float>(background.width), static_cast<float>(background.height)}, {0, 0, wnd_w, wnd_h}, {0, 0}, 0, WHITE);
						draw_text(assets, "WI-FI Connection is not supported yet.", 500, 400, 50, WHITE, DARKBLUE);
					}
					break;
				case STATE::BOY_OR_GIRL:
					{
						if (!set_state_meta && black_rect_alpha > 0.0f){
							resume_state_alpha(black_rect_alpha);
							current_music = assets.get_mus("Abandoned Lot of Dreams.mp3");
							halt_music = false;
						}
						if (!set_state_meta && black_rect_alpha <= 0.0f){
							set_state_meta = true;
						}
						constexpr std::array<int, 4> frame_order = {0, 1, 0, 2};
						char_frame_count_timer++;
						usz char_frame;
						if (char_frame_count_timer >= 10){
							char_frame_count++;
							char_frame_count_timer = 0;
						}
						char_frame_count %= 4;
						char_frame = frame_order[char_frame_count];
						auto bg_strip = assets.get_tx("options-screen-strip.png");
						screen_x -= 1.0f;
						if (screen_x < -bg_strip.width * 6.0f) screen_x = 0.0f;
						float alpha = std::sinf(GetTime()) * 40;
						for (usz i = 0; i < 20; i++){
							DrawTextureEx(bg_strip, {0 + i * bg_strip.width * 6.0f + screen_x, 0}, 0, 6.0f + (alpha * 0.03f), Color{255, 255, 255, static_cast<u8>(200 - alpha)});
						}
						i8 opt;
						Sound sel_sfx = assets.get_sfx("SEQ_SE_SELECT1.wav");
						Sound dec_sfx = assets.get_sfx("SEQ_SE_DECIDE1.wav");
						if (draw_text_seq(assets, "Before we can start, tell me about yourself.\nAre you a boy, or a girl?", 10, 100, 50, current_txt_index, txt_seq_timer, WHITE, 1)){

							if (player.get_sex() == SEX::NONE){
								opt = control_selection(selected_option, 0, 1, sel_sfx, dec_sfx, true);
							}

							auto hilbert = assets.get_tx("hilbert.png");
							auto hilda = assets.get_tx("hilda.png");

							Rectangle hilbert_src = {
								selected_option == 0 && player.get_sex() == SEX::NONE ? char_frame * 32.0f : 0,
								0,
								32,
								static_cast<float>(hilbert.height)

							};
							Rectangle hilda_src = {
								selected_option == 1 && player.get_sex() == SEX::NONE ? char_frame * 32.0f : 0,
								0,
								32,
								static_cast<float>(hilda.height)

							};
							Rectangle hilbert_dest = {
								350,
								500,
								selected_option == 0 ? 256.0f : 192.0f,
								selected_option == 0 ? 216.0f : 162.0f,
							};
							Rectangle hilda_dest = {
								1250,
								500,
								selected_option == 1 ? 256.0f : 192.0f,
								selected_option == 1 ? 216.0f : 162.0f,
							};

							DrawTexturePro(hilbert, hilbert_src, hilbert_dest, {0, 0}, 0, selected_option == 0 ? WHITE : Color{200, 200, 200, 100});
							DrawTexturePro(hilda, hilda_src, hilda_dest, {0, 0}, 0, selected_option == 1 ? WHITE : Color{200, 200, 200, 100});

							if (opt != -1 && player.get_sex() == SEX::NONE){
								player.set_sex(static_cast<SEX>(opt));
							} else if (player.get_sex() != SEX::NONE){
								switch_state(STATE::WHATS_YOUR_NAME, black_rect_alpha);
							}
						}
					}
					break;
				case STATE::WHATS_YOUR_NAME:
					{
						if (!set_state_meta && black_rect_alpha > 0.0f){
							resume_state_alpha(black_rect_alpha);
							current_txt_index = 0;
							txt_seq_timer = 0;
						}
						Sound sel_sfx = assets.get_sfx("SEQ_SE_SELECT1.wav");
						Sound dec_sfx = assets.get_sfx("SEQ_SE_DECIDE1.wav");
						Sound can_sfx = assets.get_sfx("SEQ_SE_CANCEL3.wav");
						Sound bep_sfx = assets.get_sfx("SEQ_SE_BEEP.wav");
						Sound dec2_sfx = assets.get_sfx("SEQ_SE_DECIDE2.wav");

						if (!set_state_meta && black_rect_alpha <= 0.0f){
							set_state_meta = true;
						}
						if (player.get_name() == "nodata" && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT))){
							char_cursor_blinker = 0;
							PlaySound(sel_sfx);
						}
						if (IsKeyPressed(KEY_Z)){
							if (selected_char < 70){
								if (entered_name.size() < 7){
									entered_name += name_entry_chars[selected_char];
									PlaySound(dec_sfx);
								} else {
									PlaySound(bep_sfx);
								}
							} else {
								if (selected_char == 70){
									if (entered_name.size() < 7){
										entered_name += ' ';
										PlaySound(dec_sfx);
									} else {
										PlaySound(bep_sfx);
									}
								} else if (selected_char == 74 && (!entered_name.empty() && entered_name != "nodata" && player.get_name() == "nodata")){
									player.set_name(entered_name);
									PlaySound(dec2_sfx);
								}
							}
						}

						if (player.get_name() != "nodata"){
							switch_state(STATE::JUNIPER_INTRODUCTION, black_rect_alpha, 100);
						}

						if (IsKeyPressed(KEY_X)){
							if (!entered_name.empty() && player.get_name() == "nodata"){
								entered_name.pop_back();
								PlaySound(can_sfx);
							} else {
								PlaySound(bep_sfx);
							}
						}

						float wave_pos = std::sinf(GetTime()) * 10;

						char_cursor_blinker++;
						if (char_cursor_blinker > 40) char_cursor_blinker = 0;
						screen_x += 1.0f;
						if (screen_x >= 1416) screen_x = 0;
						for (int i = -1; i < 2; i++){
							DrawTexturePro(assets.get_tx("your-name-bg.png"), {0, 0, 257, 194}, {i * 1416.0f + screen_x, 0, 1416, 1080}, {0, 0}, 0, WHITE);
						}
						draw_text_seq(assets, "Your name?", 10, 1000, 50, current_txt_index, txt_seq_timer, WHITE, 1);
						DrawTextureEx(assets.get_tx("text-entry.png"), {300, 20 + wave_pos}, 0, 5, WHITE);


						auto char_cursor = assets.get_tx(selected_char < 70 ? "char-select.png" : "char-select-wide.png");
						if (char_cursor_blinker < 20){
							DrawTextureEx(char_cursor, {408.0f + ((selected_char % 13) * 80), 200.0f + ((static_cast<u8>(selected_char / 13)) * 120) + wave_pos}, 0, 5.1f, WHITE);
						}
						draw_text(assets, entered_name, 710, 80 + wave_pos, 80, WHITE, {50, 50, 50, 255}, 70);
						player.draw_overworld_preview(assets, {325, 30 + wave_pos});
						if (player.get_name() == "nodata"){
							if (IsKeyPressed(KEY_DOWN)){
								if (selected_char < 52){
									if (selected_char < 52){
										selected_char += 13;
									} else {
										selected_char %= 13;
									}
								} else {
									selected_char = 70;
								}
							} else if (IsKeyPressed(KEY_UP)){
								if (selected_char > 12){
									selected_char -= 13;
								} else {
									selected_char = 52 + (selected_char % 13);
								}
							} else if (IsKeyPressed(KEY_LEFT)){
								if (selected_char < 70){
									if (selected_char > 0){
										selected_char--;
									} else {
										selected_char = 64;
									}
								} else {
									if (selected_char == 74) selected_char = 70;
									else selected_char = 74;
								}
							} else if (IsKeyPressed(KEY_RIGHT)){
								if (selected_char < 70){
									if (selected_char < 64){
										selected_char++;
									} else {
										selected_char = 0;
									}
								} else {
									if (selected_char == 70) selected_char = 74;
									else selected_char = 70;
								}
							}
						}
					}
					break;
				case STATE::JUNIPER_INTRODUCTION:
					{
						if (!set_state_meta && black_rect_alpha > 0.0f){
							resume_state_alpha(black_rect_alpha);
						}
						if (!set_state_meta && black_rect_alpha <= 0.0f){
							current_music = assets.get_mus("Professor Juniper.mp3");
							set_state_meta = true;
						}
					}
					break;
				case STATE::MAIN:
					{

					}
					break;
				case STATE::BATTLE:
					{

					}
					break;
				case STATE::SHOP:
					{

					}
					break;
				case STATE::POKECENTER:
					{

					}
					break;
			}
			DrawRectangle(0, 0, wnd_w, wnd_h, Color{0, 0, 0, black_rect_alpha});
			EndDrawing();
		}
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
