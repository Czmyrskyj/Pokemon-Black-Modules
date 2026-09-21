#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "trainer.hpp"
#include "dialog.hpp"
#include <variant>

enum class OPPONENT{
	TRAINER,
	WILD,
	LEGENDARY,
	GYM_LEADER,
};

using opp_entry_t = std::variant<trainer_t, pokemon_t, legendary_t>;

struct foe_t{
	OPPONENT opp;
	opp_entry_t entry;
};

enum class BATTLE_CAMERA_POS{
	DEFAULT,
	ALLY,
	FOE,
	TRAINER,
};

class battle_t{
	trainer_t& player;
	foe_t foe;
	std::vector<std::string> lines;
	u8 current_line = 0;
	u8 txtbox_alpha = 0;
	u8 txtbox_peak_alpha_timer = 0;
	bool __finished_line = false;
	u8 __current_foe_pokemon_idx = 0;
	bool __finished_dialog = false;
	bool __foe_sent_out_pokemon = false;
	float __pokemon_scale = 0.0f;
	float __pokemon_y = 0.5f;
	float __line_under_foe_pokeballs = -800.0f;
	std::array<float, 6> __foe_pokeball_x{};
	u8 __current_foe_pokeball = 0;
	u8 __foe_accent_color = 0;
	u8 __foe_alpha = 255;
	float __foe_z = -0.9f;
	bool __played_foe_cry = false;
	float __foe_hud_x = -620.0f;
	BATTLE_CAMERA_POS __camera_pos = BATTLE_CAMERA_POS::DEFAULT;
	u8 __trainer_alpha = 255;
	public:
	battle_t(trainer_t& p, foe_t& f) : player(p), foe(f){
		if (f.opp == OPPONENT::WILD || f.opp == OPPONENT::LEGENDARY) {
			__foe_sent_out_pokemon = true;
			__pokemon_scale = 0.0f;
			__pokemon_y = 0.0f;
		}
	};
	~battle_t() = default;

	void init_dialog(std::vector<std::string> l){
		lines = std::move(l);
		current_line = 0;
		txtbox_alpha = 0;
		txtbox_peak_alpha_timer = 0;
		__finished_line = false;
	}

	std::string get_dialog_intro();
	std::string get_dialog_trainer_sent_out();
	std::string step_dialog(writer_t& writer, Locale::locale_t& locale);

	void write(assets_t& assets, writer_t& writer);
	void finish_line(bool f){
		__finished_line = f;
	}
	bool finished_line() const{
		return __finished_line;
	}
	bool finished_dialog() const{
		return __finished_dialog;
	}
	u8 current_foe_pokemon_idx() const {
		return __current_foe_pokemon_idx;
	}
	bool foe_sent_out_pokemon() const {
		return __foe_sent_out_pokemon;
	}
	void set_foe_sent_out_pokemon(bool s = true){
		__foe_sent_out_pokemon = s;
	}
	float pokemon_scale() const{
		return __pokemon_scale;
	}
	void inc_pokemon_scale() {
		if (__pokemon_scale < 1.0f) __pokemon_scale += 0.05f;
	}
	float pokemon_y() const {
		return __pokemon_y;
	}
	float line_under_foe_pokeballs() const{
		return __line_under_foe_pokeballs;
	}
	void move_line_under_foe_pokeballs() {
		if (__line_under_foe_pokeballs < 0.0f) __line_under_foe_pokeballs += 40.0f;
	}
	bool control_foe_pokeballs() {
		if (__current_foe_pokeball < 6){
			if (__foe_pokeball_x[__current_foe_pokeball] < (5 - __current_foe_pokeball) * 72.0f + 30.0f){
				__foe_pokeball_x[__current_foe_pokeball] += 60.0f;
			} else {
				__foe_pokeball_x[__current_foe_pokeball] = (5 - __current_foe_pokeball) * 72.0f + 30.0f;
				__current_foe_pokeball++;
				return true;
			}
		}
		return false;
	}
	std::span<const float> foe_pokeball_x() const{
		return __foe_pokeball_x;
	}
	u8 current_foe_pokeball() const{
		return __current_foe_pokeball;
	}
	void pokemon_y_delta(float delta) {
		__pokemon_y += delta;
	}
	u8 foe_accent_color() const{
		return __foe_accent_color;
	}
	u8 foe_alpha() const{
		return __foe_alpha;
	}
	void foe_alpha_delta(float delta){
		__foe_alpha += delta;
	}
	void foe_accent_color_delta(float delta){
		__foe_accent_color += delta;
	}
	float foe_z() const{
		return __foe_z;
	}
	void foe_z_delta(float delta){
		__foe_z += delta;
	}
	bool played_foe_cry() const{
		return __played_foe_cry;
	}
	void set_played_foe_cry(bool p){
		__played_foe_cry = p;
	}
	float foe_hud_x() const{
		return __foe_hud_x;
	}
	void foe_hud_x_delta(float delta){
		__foe_hud_x += delta;
	}
	BATTLE_CAMERA_POS camera_pos(){
		return __camera_pos;
	}
	void set_camera_pos(BATTLE_CAMERA_POS pos){
		__camera_pos = pos;
	}
	u8 trainer_alpha() const{
		return __trainer_alpha;
	}
	void trainer_alpha_delta(float delta){
		__trainer_alpha += delta;
	}
};
