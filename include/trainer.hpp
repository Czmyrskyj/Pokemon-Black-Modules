#pragma once

#include "common.hpp"
#include "pokemon.hpp"
#include "sex.hpp"
#include "trainer_type_prefixes.hpp"
#include "items.hpp"
#include <string>
#include <cassert>
#include <span>

class party_t{
	std::array<pokemon_t, 6> data_;
	usz size_ = 0;
	public:
	void push_back(pokemon_t pokemon){
		assert(size_ < data_.size() && "pokemon party index exceeds 6");
		data_[size_++] = std::move(pokemon);
	}
	std::span<pokemon_t> data(){
		return std::span<pokemon_t>(data_.data(), size_);
	}
	std::span<const pokemon_t> data() const{
		return std::span<const pokemon_t>(data_.data(), size_);
	}
	usz size() const{
		return size_;
	}
	bool empty() const{
		return size_ == 0;
	}
	auto begin() { return data_.begin(); }
	auto end() { return data_.begin() + size_; }
	auto begin() const { return data_.begin(); }
	auto end() const { return data_.begin() + size_; }
	pokemon_t& operator[](usz idx){
		return data_[idx];
	}
};

class assets_t;
struct Vector2;

enum class BAG : u8{
	ITEMS = 0,
	KEY,
	BERRIES,
	TMHM,
	MEDICINE,
	COUNT
};

using inventory_t = std::array<std::vector<item_t>, static_cast<u8>(BAG::COUNT)>;

class trainer_t{
	std::string m_name = "nodata";
	TRAINER_TYPE m_trainer_type = TRAINER_TYPE::NONE;
	SEX m_sex = SEX::NONE;
	party_t party{};
	std::vector <pokemon_t> pc_party{};
	u8 m_conscious_count = 0;
	inventory_t m_inventory{};
	public:
	trainer_t() = default;
	trainer_t(std::string n, TRAINER_TYPE t) : m_name(std::move(n)), m_trainer_type(t) {}
	std::string_view name() const{
		return m_name;
	}
	std::string_view get_prefix() const{
		return trainer_type_prefixes[static_cast<u8>(m_trainer_type)];
	}
	void set_sex(SEX s){
		m_sex = s;
	}
	SEX sex() const{
		return m_sex;
	}
	void set_name(std::string n){
		m_name = std::move(n);
	}
	TRAINER_TYPE trainer_type() const {
		return m_trainer_type;
	}
	void draw_overworld_preview(assets_t& assets, Vector2 pos);
	void push_pokemon(pokemon_t pokemon){
		if (party.size() < 6){
			party.push_back(pokemon);
			m_conscious_count++;
		} else {
			pc_party.push_back(pokemon);
		}
	}
	u8 conscious_count() const {
		return m_conscious_count;
	}
	void set_conscious_count(u8 count){
		m_conscious_count = count;
	}
	party_t& get_party(){
		return party;
	}
	void push_item(itemdex_t& itemdex, u16 id){
		item_t item = itemdex.get_item(id);
		m_inventory[item.category].push_back(item);
	}
	void pop_item(u8 category, u8 n){
		m_inventory[category].erase(m_inventory[category].begin() + n);
	}
	const inventory_t& inventory() const{
		return m_inventory;
	}
	void use_item(itemdex_t& itemdex, u16 id, u8 target){
		item_t item = itemdex.get_item(id);
		switch (item.category){
			case 0:
				
				break;
			case 1:

				break;
			case 2:

				break;
			case 3:

				break;
			case 4:

				break;
			case 5:
				// party[target].hp_heal(item.power);
				break;
		}
	}
};
void draw_trainer(assets_t& assets, TRAINER_TYPE trainer_type, std::string_view trainer, Vector3 pos, const Camera3D* camera = nullptr, Color tint = WHITE);
struct anim_trainer_render_t{
	u8 frame{0};
	float frametimer{0.0f};
	float framerate{2.0f};
	float x_shift{0.0f};
	float y_shift{0.0f};
	u8 hand_frame{0};
	float hand_frametimer{0.0f};
	float hand_framerate{2.0f};
	float hand_rotation{0.0f};
	float x_shift_timer{0.0f};
	bool draw(assets_t& assets, std::string_view trainer, Vector3 pos, const Camera3D* camera = nullptr, Color tint = WHITE, bool allow_animated = true);
	void draw_vs(assets_t& assets, std::string_view trainer, Vector2 pos, Color tint = WHITE);
	bool draw_back(assets_t& assets, std::string_view trainer, Vector3 pos, const Camera3D& camera, Color tint = WHITE);
};
