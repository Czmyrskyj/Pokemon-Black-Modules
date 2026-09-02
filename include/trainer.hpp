#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include <string>
#include <cassert>
#include <span>

class party_t{
	std::array<pokemon_t, 6> data_;
	usz size_ = 0;
	public:
	void push_back(pokemon_t pokemon){
		assert(size_ < (data_.size() - 1) && "pokemon party index exceeds 6");
		data_[size_++] = std::move(pokemon);
	}
	std::span<pokemon_t> data(){
		return data_;
	}
	usz size() const{
		return size_;
	}
	bool empty() const{
		return size_ == 0;
	}
};

enum class SEX : i8{
	NONE = -1,
	MALE = 0,
	FEMALE = 1,
};
class assets_t;
struct Vector2;

class trainer_t{
	std::string name = "nodata";
	std::string prefix = "";
	SEX sex = SEX::NONE;
	party_t party{};
	std::vector <pokemon_t> pc_party{};
	public:
	trainer_t() = default;
	trainer_t(std::string n, std::string p) : name(std::move(n)), prefix(std::move(p)) {}
	std::string_view get_name() const{
		return name;
	}
	std::string_view get_prefix() const{
		return prefix;
	}
	void set_sex(SEX s){
		sex = s;
	}
	SEX get_sex() const{
		return sex;
	}
	void set_name(std::string n){
		name = std::move(n);
	}
	void draw_overworld_preview(assets_t& assets, Vector2 pos);
	void push_pokemon(pokemon_t pokemon){
		if (party.size() < 6){
			party.push_back(pokemon);
		} else {
			pc_party.push_back(pokemon);
		}
	}
	party_t& get_party(){
		return party;
	}
};
struct anim_trainer_render_t{
	u8 frame{0};
	float frametimer{0.0f};
	float framerate{2.0f};
	float x_shift{0.0f};
	float y_shift{0.0f};
	void draw(assets_t& assets, std::string_view speaker, Vector2 pos);
};
struct gym_leader_t{
	trainer_t trainer;
};
