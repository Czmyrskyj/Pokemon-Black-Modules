#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "trainer.hpp"
#include "dialog.hpp"
#include "pokeball.hpp"
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

enum class BATTLE_PHASE{
	INTRO,
	FOE_SEND_OUT,
	ALLY_SEND_OUT,
	MENU,
};

enum class BATTLE_MENU_STATE{
	NONE,
	OPTIONS,
	MOVES,
	MODULES,
	BAG,
	POKEMON,
};

class battle_t{
	trainer_t& player;
	foe_t foe;
	std::vector<std::string> lines;
	u8 current_line = 0;
	u8 txtbox_alpha = 0;
	u8 txtbox_peak_alpha_timer = 0;
	u8 m_current_foe_pokemon_idx = 0;
	bool m_finished_dialog = false;
	float m_foe_pokemon_scale = 0.0f;
	float m_foe_pokemon_y = 0.5f;
	float m_line_under_foe_pokeballs = -800.0f;
	std::array<float, 6> m_foe_pokeball_x{};
	u8 m_current_foe_pokeball = 0;
	u8 m_foe_accent_color = 0;
	u8 m_foe_alpha = 255;
	float m_foe_z = -0.9f;
	bool m_played_foe_cry = false;
	float m_foe_hud_x = -620.0f;
	BATTLE_CAMERA_POS m_camera_pos = BATTLE_CAMERA_POS::DEFAULT;
	u8 m_trainer_alpha = 255;
	float m_line_under_trainer_pokeballs = wnd_w;
	u8 m_current_trainer_pokeball = 0;
	std::array<float, 6> m_trainer_pokeball_x{};
	float m_ally_pokemon_scale = 0.0f;
	float m_ally_pokemon_y = 0.5f;
	u8 m_current_trainer_pokemon_idx = 0;
	float m_ally_hud_x = wnd_w;
	BATTLE_PHASE m_phase = BATTLE_PHASE::INTRO;
	BATTLE_MENU_STATE m_menu_state = BATTLE_MENU_STATE::OPTIONS;
	public:
	battle_t(trainer_t& p, foe_t& f) : player(p), foe(f){
		if (f.opp == OPPONENT::WILD || f.opp == OPPONENT::LEGENDARY) {
			m_foe_pokemon_scale = 0.0f;
			m_foe_pokemon_y = 0.0f;
		}
		m_trainer_pokeball_x.fill(wnd_w);
	};
	~battle_t() = default;

	void init_dialog(std::vector<std::string> l){
		lines = std::move(l);
		current_line = 0;
		txtbox_alpha = 0;
		txtbox_peak_alpha_timer = 0;
	}

	std::string get_dialog_intro();
	std::string get_dialog_foe_sent_out();
	std::string step_dialog(writer_t& writer, Locale::locale_t& locale);

	void write(assets_t& assets, writer_t& writer);
	bool finished_dialog() const{
		return m_finished_dialog;
	}
	u8 current_foe_pokemon_idx() const {
		return m_current_foe_pokemon_idx;
	}
	float foe_pokemon_scale() const{
		return m_foe_pokemon_scale;
	}
	void inc_foe_pokemon_scale() {
		if (m_foe_pokemon_scale < 1.0f) m_foe_pokemon_scale += 0.05f;
	}
	float foe_pokemon_y() const {
		return m_foe_pokemon_y;
	}
	float line_under_foe_pokeballs() const{
		return m_line_under_foe_pokeballs;
	}
	void move_line_under_foe_pokeballs() {
		if (m_line_under_foe_pokeballs < 0.0f) m_line_under_foe_pokeballs += 40.0f;
	}
	bool control_foe_pokeballs() {
		if (m_current_foe_pokeball < 6){
			if (m_foe_pokeball_x[m_current_foe_pokeball] < (5 - m_current_foe_pokeball) * 72.0f + 30.0f){
				m_foe_pokeball_x[m_current_foe_pokeball] += 60.0f;
			} else {
				m_foe_pokeball_x[m_current_foe_pokeball] = (5 - m_current_foe_pokeball) * 72.0f + 30.0f;
				m_current_foe_pokeball++;
				return true;
			}
		}
		return false;
	}
	std::span<const float> foe_pokeball_x() const{
		return m_foe_pokeball_x;
	}
	u8 current_foe_pokeball() const{
		return m_current_foe_pokeball;
	}
	void foe_pokemon_y_delta(float delta) {
		m_foe_pokemon_y += delta;
	}
	u8 foe_accent_color() const{
		return m_foe_accent_color;
	}
	u8 foe_alpha() const{
		return m_foe_alpha;
	}
	void foe_alpha_delta(u8 delta){
		m_foe_alpha += delta;
	}
	void foe_accent_color_delta(u8 delta){
		m_foe_accent_color += delta;
	}
	float foe_z() const{
		return m_foe_z;
	}
	void foe_z_delta(float delta){
		m_foe_z += delta;
	}
	bool played_foe_cry() const{
		return m_played_foe_cry;
	}
	void set_played_foe_cry(bool p){
		m_played_foe_cry = p;
	}
	float foe_hud_x() const{
		return m_foe_hud_x;
	}
	void foe_hud_x_delta(float delta){
		m_foe_hud_x += delta;
	}
	BATTLE_CAMERA_POS camera_pos(){
		return m_camera_pos;
	}
	void set_camera_pos(BATTLE_CAMERA_POS pos){
		m_camera_pos = pos;
	}
	u8 trainer_alpha() const{
		return m_trainer_alpha;
	}
	void trainer_alpha_delta(u8 delta){
		m_trainer_alpha += delta;
	}
	float line_under_trainer_pokeballs() const{
		return m_line_under_trainer_pokeballs;
	}
	void move_line_under_trainer_pokeballs() {
		if (m_line_under_trainer_pokeballs > 1120.0f) m_line_under_trainer_pokeballs -= 40.0f;
	}
	bool control_trainer_pokeballs() {
		if (m_current_trainer_pokeball < 6){
			if (m_trainer_pokeball_x[m_current_trainer_pokeball] > m_current_trainer_pokeball* 72.0f + (wnd_w - 500.0f)){
				m_trainer_pokeball_x[m_current_trainer_pokeball] -= 60.0f;
			} else {
				m_trainer_pokeball_x[m_current_trainer_pokeball] = m_current_trainer_pokeball * 72.0f + (wnd_w - 500.0f);
				m_current_trainer_pokeball++;
				return true;
			}
		}
		return false;
	}
	std::span<const float> trainer_pokeball_x() const{
		return m_trainer_pokeball_x;
	}
	u8 current_trainer_pokeball() const{
		return m_current_trainer_pokeball;
	}
	float ally_pokemon_scale() const{
		return m_ally_pokemon_scale;
	}
	void inc_ally_pokemon_scale() {
		if (m_ally_pokemon_scale < 1.0f) m_ally_pokemon_scale += 0.05f;
	}
	float ally_pokemon_y() const{
		return m_ally_pokemon_y;
	}
	void ally_pokemon_y_delta(float delta) {
		m_ally_pokemon_y += delta;
	}
	u8 current_trainer_pokemon_idx() const{
		return m_current_trainer_pokemon_idx;
	}
	float ally_hud_x() const{
		return m_ally_hud_x;
	}
	void ally_hud_x_delta(float delta){
		m_ally_hud_x += delta;
	}
	BATTLE_PHASE phase() const {
		return m_phase;
	}
	void set_phase(assets_t& assets, BATTLE_PHASE p){
		m_phase = p;
		switch (p){
			case BATTLE_PHASE::FOE_SEND_OUT: 
				{
					PlaySound(assets.get_sfx("SEQ_SE_NAGERU.wav"));
				}
				break;
			case BATTLE_PHASE::MENU:
				{
					PlaySound(assets.get_sfx("SEQ_SE_OPEN2.wav"));
				}
				break;
			default: break;
		}
	}
	BATTLE_MENU_STATE menu_state() const{
		return m_menu_state;
	}
	void set_menu_state(BATTLE_MENU_STATE s){
		m_menu_state = s;
	}
};
