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
	POKEMON_RESEARCH_LAB,
	BATTLE,
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
	float music_volume = 1.0f;
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
	const std::array<std::string_view, 3> starter_type_labels{
		"Grass",
			"Fire",
			"Water",
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
	bool initialized_meet_msg{false};
	bool allow_fight{false};
	bool ended_battle{false};
	std::string current_line;
	usz index = 0;
	float encounter_info_txtbox_y{wnd_h};
	std::vector<cloud_t> clouds{};
	battle_entry_t bt_info;
	Color sky_color{SKYBLUE};
	void update(st_ctx_t& ctx);
	void render(st_ctx_t& ctx);
	explicit main_state_t(st_ctx_t& ctx);
};

using active_state_t = std::variant<std::monostate, title_state_t, main_menu_t, boy_or_girl_t, wifi_connection_t, whats_your_name_t, juniper_letter_t, choose_a_starter_t, starter_selected_t, which_season_and_region_t, main_state_t>;
