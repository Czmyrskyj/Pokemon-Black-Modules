#pragma once
#include "common.hpp"
#include "assets.hpp"
#include "trainer.hpp"
#include "locale.hpp"
#include "regions.hpp"
#include "battle_info.hpp"
#include "formatter.hpp"
#include "camera.hpp"
#include "spritedex.hpp"

struct st_ctx_t;

#include "states.hpp"

constexpr float alpha_delta{300.0f};
struct st_ctx_t{
	assets_t& assets;
	trainer_t player{"nodata", ""};
	trainer_t cheren{"Cheren", "[PKMN_SIGN] Trainer"};
	trainer_t bianca{"Bianca", "[PKMN_SIGN] Trainer"};
	Locale::locale_t& locale;
	pokedex_t& pokedex;
	spritedex_t& spritedex;
	movedex_t& movedex;
	Time::time_t& time;
	battle_info_t& battle_info;
	formatter_t& formatter;
	camera_t& camera;
	battle_entry_t bt_info{};
	STATE current_state = STATE::TITLE;
	REGION region = REGION::NUVEMA;
	Color bg_color = BLACK;
	active_state_t state_data = title_state_t{};
	Color transition_color = BLACK;
	u8 transition_alpha = 255;
	bool set_state_meta{false};
	bool halt_music{false};
	Music current_music{};
	std::string music_name{};
	u8 selected_option = 0;
	bool music_started{false};
	float music_volume = 1.0f;
	void resume_state_alpha(unsigned char& alpha){
		float current_alpha = static_cast<float>(alpha);
		if (current_alpha > 5.0f){
			current_alpha -= alpha_delta*GetFrameTime();
		} else {
			current_alpha = 0.0f;
		}
		alpha = static_cast<unsigned char>(current_alpha);
	}
	i8 control_selection(u8& opt, u8 max, const Sound& sel, const Sound& dec, bool horizontal = false){
		if (IsKeyPressed(!horizontal ? KEY_DOWN : KEY_RIGHT)){
			opt++;
			if (opt > max) opt = 0;
			PlaySound(sel);
		} else if (IsKeyPressed(!horizontal ? KEY_UP : KEY_LEFT)){
			if (opt > 0){
				opt--;
			} else {
				opt = max;
			}
			PlaySound(sel);
		}
		if (IsKeyPressed(KEY_Z)){
			PlaySound(dec);
			return opt;
		}
		return -1;
	}
	void switch_data(STATE new_state){
		std::visit([this](auto& active){
				if constexpr (requires { active.clear(assets); }){
				active.clear(assets);
				}
				}, state_data);

		switch (new_state){
			case STATE::TITLE:
				state_data = title_state_t{};
				break;
			case STATE::MAIN_MENU:
				state_data.emplace<main_menu_t>(*this);
				break;
			case STATE::BOY_OR_GIRL:
				state_data.emplace<boy_or_girl_t>(*this);
				break;
			case STATE::WIFI_CONNECTION:
				state_data = wifi_connection_t{};
				break;
			case STATE::WHATS_YOUR_NAME:
				state_data.emplace<whats_your_name_t>(*this);
				break;
			case STATE::JUNIPER_LETTER:
				state_data.emplace<juniper_letter_t>(*this);
				break;
			case STATE::CHOOSE_STARTER:
				state_data.emplace<choose_a_starter_t>(*this);
				break;
			case STATE::STARTER_SELECTED:
				state_data.emplace<starter_selected_t>(*this);
				break;
			case STATE::WHICH_SEASON_AND_REGION:
				state_data.emplace<which_season_and_region_t>(*this);
				break;
			case STATE::MAIN:
				state_data.emplace<main_state_t>(*this);
				break;
			case STATE::BATTLE_VS:
				state_data.emplace<battle_vs_t>(*this);
				break;
			case STATE::BATTLE:
				state_data.emplace<battle_state_t>(*this);
				break;
			default:
				break;
		}
	}

	void switch_state(STATE new_state, float delta = alpha_delta){
		float current_alpha = static_cast<float>(transition_alpha);
		if (current_state != new_state){
			current_alpha+=delta*GetFrameTime();
			if (current_alpha >= 255.0f){
				current_alpha = 255.0f;
				current_state = new_state;
				switch_data(new_state);
				selected_option = 0;
				set_state_meta = false;
			}
		}
		transition_alpha = static_cast<unsigned char>(current_alpha);
	}
	void set_music(std::string_view mus){
		if (music_name == mus) return;
		music_name = mus;
		current_music = assets.get_mus(music_name);
		music_started = false;
	}
	// WARN: only for debugging!
	void set_temp_data(){
		current_state = STATE::MAIN;
		region = REGION::ROUTE_1;
		state_data.emplace<main_state_t>(*this);
		player.set_name("Dvoriak");
		player.set_sex(SEX::MALE);
		pokemon_t players{
			pokedex.get_pokedex_entry(OSHAWOTT).value(),
			"OSHAWOTT",
			NATURE::CALM,
			SEX::MALE,
			5,
			{1, 0, 0, 0},
		};
		pokemon_t cherens{
			pokedex.get_pokedex_entry(SNIVY).value(),
			"SNIVY",
			NATURE::TIMID,
			SEX::MALE,
			5,
			{1, 0, 0, 0},
		};
		pokemon_t biancas{
			pokedex.get_pokedex_entry(TEPIG).value(),
			"TEPIG",
			NATURE::LONELY,
			SEX::MALE,
			5,
			{1, 0, 0, 0},
		};
		pokemon_t purrloin{
			pokedex.get_pokedex_entry(PURRLOIN).value(),
			"PURRLOIN",
			NATURE::IMPISH,
			SEX::MALE,
			5,
			{1, 0, 0, 0},
		};
		pokemon_t buizel{
			pokedex.get_pokedex_entry(BUIZEL).value(),
			"BUIZEL",
			NATURE::CALM,
			SEX::MALE,
			5,
			{1, 0, 0, 0},
		};
		pokemon_t serperior{
			pokedex.get_pokedex_entry(SERPERIOR).value(),
			"SERPERIOR",
			NATURE::LONELY,
			SEX::MALE,
			5,
			{1, 0, 0, 0},
		};
		pokemon_t emolga{
			pokedex.get_pokedex_entry(EMOLGA).value(),
			"EMOLGA",
			NATURE::CALM,
			SEX::FEMALE,
			5,
			{1, 0, 0, 0},
		};
		formatter.feed("[PLAYER]", std::string(player.get_name()));
		formatter.feed("[PLAYER-STARTER]", "OSHAWOTT");
		formatter.feed("[BIANCA-STARTER]", "TEPIG");
		formatter.feed("[CHEREN-STARTER]", "SNIVY");
		player.push_pokemon(serperior);
		player.push_pokemon(players);
		player.push_pokemon(purrloin);
		player.push_pokemon(buizel);
		player.push_pokemon(emolga);
		cheren.push_pokemon(cherens);
		bianca.push_pokemon(biancas);
		bianca.push_pokemon(purrloin);
		bianca.push_pokemon(buizel);
	}
};
