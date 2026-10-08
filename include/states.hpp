#pragma once
#include "common.hpp"
#include "writer.hpp"
#include "dialog.hpp"
#include "pokedex.hpp"
#include <variant>
#include <vector>

enum class STATE{
	TITLE,
	MAIN_MENU,
	WIFI_CONNECTION,
	BOY_OR_GIRL,
	WHATS_YOUR_NAME,
	JUNIPER_LETTER,
	CHOOSE_STARTER,
	STARTER_SELECTED,
	WHICH_SEASON_AND_REGION,
	MAIN,
	BATTLE_VS,
	BATTLE,
	POKEMON_RESEARCH_LAB,
	SHOP,
	POKECENTER,
};

struct st_ctx_t;
constexpr float logo_scale{0.3f};
struct title_state_t{
	float strip_pos = 0.0f;
	u8 press_enter_timer = 0;
	bool title_pressed_enter = false;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	void clear(assets_t& assets);
};

struct main_menu_t{
	i8 switch_state_in_main_menu = -1;
	std::vector<std::string_view> menu_options;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	void clear(assets_t& assets);
	explicit main_menu_t(st_ctx_t& ctx);
};

struct boy_or_girl_t{
	const std::array<int, 4> frame_order = {0, 1, 0, 2};
	float screen_x = 0.0f;
	u8 char_frame_count_timer = 0;
	u8 char_frame_count = 0;
	i8 opt = -1;
	usz char_frame = 0;
	writer_t writer{};
	std::string formatted_prompt;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	explicit boy_or_girl_t(st_ctx_t& ctx);
};

struct wifi_connection_t{
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
};

struct whats_your_name_t{
	u8 selected_char = 0;
	u8 char_cursor_blinker = 0;
	std::string entered_name = "";
	float screen_x = 0.0f;
	const std::string_view name_entry_chars{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789., "};
	writer_t writer;
	std::string formatted_prompt;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	void clear(assets_t& assets);
	explicit whats_your_name_t(st_ctx_t& ctx);
};

struct juniper_letter_t{
	writer_t writer{};
	writer_t sincerely_writer{};
	std::string formatted_juniper_letter;
	std::string formatted_sincerely;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	void clear(assets_t& assets);
	explicit juniper_letter_t(st_ctx_t& ctx);
};

struct choose_a_starter_t{
	dialog_t dialog{};
	writer_t writer{};
	std::string current_line;
	float screen_x = 0.0f;
	u8 current_starter = 1;
	i8 selected_starter = -1;
	u8 current_opt = 0;
	i8 selected_opt = -1;
	bool played_cry = false;
	float volume = 1.0f;
	bool played_demo_03 = false;
	bool handed_pokemon = false;
	i8 cheren_selected_starter = -1;
	i8 bianca_selected_starter = -1;
	std::array<POKEDEX, 3> starters{
		SNIVY,
			TEPIG,
			OSHAWOTT,
	};
	std::array<float, 3> starter_scales{
		2.0f,
			2.0f,
			2.0f,
	};
	std::array<Color, 3> starter_colors{
		Color{45, 45, 45, 200},
			Color{45, 45, 45, 200},
			Color{45, 45, 45, 200},
	};
	std::array<float, 3> starter_positions{
		400.0f,
			900.0f,
			1500.0f,
	};
	float jump_factor = 0.0f;
	std::array<std::string_view, 2> yes_no{
		"YES",
			"NO",
	};
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	explicit choose_a_starter_t(st_ctx_t& ctx);
	void clear(assets_t& assets);
};

struct starter_selected_t{
	bool played_received_starter = false;
	bool init_dialog = false;
	std::string formatted_string;
	std::string current_line;
	writer_t writer{};
	dialog_t dialog{};
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	explicit starter_selected_t(st_ctx_t& ctx);
};
namespace Time{
struct time_t;
};
struct which_season_and_region_t{
	writer_t writer{};
	writer_t region_writer{};
	std::string season_label{};
	Color season_label_color{};
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	explicit which_season_and_region_t(st_ctx_t& ctx);
};

struct cloud_t{
	Vector2 pos;
	float scale;
};

#include "battle_info.hpp"

struct main_state_t{
	writer_t writer{};
	dialog_t predialog{};
	dialog_t postdialog{};
	Vector2 celestial_pos{};
	bool initialized_postdialog{false};
	bool initialized_encounter_info{false};
	bool allow_fight{false};
	bool started_battle{false};
	bool ended_battle{false};
	float encounter_info_txtbox_y{wnd_h};
	std::string current_line;
	// WARN: [!!] AFTER DEBUGGING, SET TO 0
	usz index = 0;
	std::vector<cloud_t> clouds{};
	Color sky_color{SKYBLUE};
	anim_trainer_render_t anim_trainer_render;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	explicit main_state_t(st_ctx_t& ctx);
};

constexpr float bn_w{128.0f};
constexpr float bn_h{64.0f};
struct battle_vs_t{
	u8 flash_count = 0;
	u8 flash_alpha = 0;
	u8 after_three_flashes_timer = 0;
	u16 after_flashed_last_time_timer = 0;
	bool flashed = false;
	bool flashed_last_time = false;
	bool played_vs_whoosh{false};
	float banner_x = -bn_w*8.0f;
	float source_x = 0.0f;
	float portrait_x = -bn_w*8.0f;
	float portrait_vel = 80.0f;
	float vs_scale{4.0f};
	Vector2 shake{0, 0};
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	anim_trainer_render_t anim_trainer_render;
	explicit battle_vs_t(st_ctx_t& ctx);
};

#include "pokeball.hpp"


enum class BATTLE_CAMERA_POS{
	DEFAULT,
	ALLY,
	FOE,
	TRAINER,
};

enum class BATTLE_PHASE{
	INTRO,
	FOE_SEND_OUT,
	ALLY_SEND_OUT,
	MENU,
	FIGHT,
	FOE_ABOUT_TO_SEND,
	END,
};

enum class BATTLE_MENU_STATE{
	NONE,
	OPTIONS,
	MOVES,
	MODULES,
	BAG,
	POKEMON,
};

struct battle_state_t{
	float bg_rotation = 0.0f;
	u8 animation_timer = 0;
	bool animation_done = false;
	bool finished_drawing_back = false;
	bool txted = false;
	float foe_pokemon_scale = 0.0f;
	float foe_pokemon_y = 0.5f;
	float line_under_foe_pokeballs = -800.0f;
	std::array<float, 6> foe_pokeball_x{};
	u8 current_foe_pokeball = 0;
	std::string current_line;
	dialog_t dialog;
	writer_t writer;
	battle_t battle;
	anim_trainer_render_t player_back;
	anim_trainer_render_t anim_trainer_render;
	thrown_pokeball_renderer_t foe_thrown_pokeball_renderer; 
	thrown_pokeball_renderer_t trainer_thrown_pokeball_renderer; 
	float trainer_thrown_pokeball_y = 0.7f;
	float trainer_thrown_pokeball_z = 3.0f;
	float trainer_thrown_pokeball_y_vel = 0.05f;
	float battle_menu_y = wnd_h;
	float foe_z = -0.9f;
	float foe_hud_x = -620.0f;
	float line_under_trainer_pokeballs = wnd_w;
	float ally_pokemon_scale = 0.0f;
	float ally_pokemon_y = 0.5f;
	float ally_hud_x = wnd_w;
	u16 foe_hp_render = 0;
	u16 ally_hp_render = 0;
	BATTLE_CAMERA_POS camera_pos = BATTLE_CAMERA_POS::DEFAULT;
	BATTLE_CAMERA_POS camera_targ = BATTLE_CAMERA_POS::DEFAULT;
	BATTLE_PHASE phase = BATTLE_PHASE::INTRO;
	BATTLE_MENU_STATE menu_state = BATTLE_MENU_STATE::OPTIONS;
	BATTLE_MENU_STATE next_menu_state = BATTLE_MENU_STATE::NONE;
	u8 foe_accent_color = 0;
	u8 foe_alpha = 255;
	u8 trainer_alpha = 255;
	u8 current_trainer_pokeball = 0;
	u8 turn_timer = 0;
	std::array<float, 6> trainer_pokeball_x{};
	u8 opt = 0;
	i8 sel_opt = -1;
	i8 next_pokemon = -1;
	bool running_from_tr_batt = false;
	u8 no_run_from_tr_batt_scrn_alpha = 0;
	u8 no_run_from_tr_batt_scrn_alpha_tim = 0;
	bool critical_hp = false;
	bool foe_defeat_msg = false;
	u8 foe_defeat_timer = 0;
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	bool control_foe_pokeballs();
	bool control_ally_pokeballs();
	void set_phase(assets_t& assets, BATTLE_PHASE p);
	explicit battle_state_t(st_ctx_t& ctx);
};

using active_state_t = std::variant<std::monostate, title_state_t, main_menu_t, boy_or_girl_t, 
      wifi_connection_t, whats_your_name_t, juniper_letter_t, choose_a_starter_t, starter_selected_t,
      which_season_and_region_t, main_state_t, battle_vs_t, battle_state_t>;
