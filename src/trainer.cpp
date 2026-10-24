#include "trainer.hpp"
#include "assets.hpp"
#include <raylib.h>
#include <rlgl.h>
#include <format>
#include <algorithm>
#include <cmath>

constexpr u8 vs_w{128};
constexpr u8 vs_h{64};

constexpr float trainer_scale{0.8f};

void trainer_t::draw_overworld_preview(assets_t& assets, Vector2 pos){
	static float char_frame_timer = 0.0f;
	static u8 char_frame = 0;
	constexpr std::array<int, 4> frame_order{0, 1, 0, 2};
	auto texture = assets.get_tx((m_sex == SEX::MALE) ? "hilbert.png" : "hilda.png");
	char_frame_timer+=3*GetFrameTime();
	if (char_frame_timer > 0.5f){
		char_frame++;
		char_frame_timer = 0;
	}
	char_frame %= 4;
	Rectangle src = {
		frame_order[char_frame] * 32.0f,
		0,
		32,
		static_cast<float>(texture.height)
	};
	DrawTexturePro(texture, src, {pos.x, pos.y, 192, texture.height*6.0f}, {0, static_cast<float>(texture.height)}, 0, WHITE);
}


void draw_shadow(Texture2D texture, Rectangle src, Vector3 pos, Color tint, float z_offset = 0.0f){

	pos.y += 0.08f;

	rlDisableDepthMask();
	rlDisableBackfaceCulling();
	rlSetTexture(texture.id);

	float u0 = src.x / texture.width;
	float v0 = src.y / texture.height;
	float u1 = (src.x + src.width) / texture.width;
	float v1 = (src.y + src.height) / texture.height;

	rlBegin(RL_QUADS);
	rlColor4ub(tint.r, tint.g, tint.b, tint.a);
	rlTexCoord2f(u0, v0); rlVertex3f(pos.x, pos.y, pos.z + trainer_scale + z_offset);
	rlTexCoord2f(u1, v0); rlVertex3f(pos.x + trainer_scale, pos.y, pos.z + trainer_scale + z_offset);
	rlTexCoord2f(u1, v1); rlVertex3f(pos.x + trainer_scale, pos.y, pos.z + z_offset);
	rlTexCoord2f(u0, v1); rlVertex3f(pos.x, pos.y, pos.z + z_offset);
	rlEnd();

	rlSetTexture(0);
	rlEnableBackfaceCulling();
	rlEnableDepthMask();
}

struct speaker_anim_t{
	u8 row;
	std::span<const u8> order;
};

static constexpr std::array<u8, 3> cheren_order{0, 1, 2};
static constexpr std::array<u8, 8> bianca_order{1, 2, 1, 2, 1, 2, 0, 0};
static constexpr std::array<u8, 3> n_order{0, 1, 2};

static const speaker_anim_t* find_anim(std::string_view speaker){
	static constexpr speaker_anim_t cheren{1, cheren_order};
	static constexpr speaker_anim_t bianca{0, bianca_order};
	static constexpr speaker_anim_t n{2, n_order};
	if (speaker == "Cheren") return &cheren;
	if (speaker == "Bianca") return &bianca;
	if (speaker == "N") return &n;
	return nullptr;
}

bool has_anim(std::string_view speaker){
	return find_anim(speaker) != nullptr;
}

void anim_trainer_render_t::update(std::string_view speaker, bool allow_animated){
	const auto* a = find_anim(speaker);
	if (!a) return;
	if (speaker == "Cheren"){
		switch (a->order[frame]){
			case 0: 
				framerate = 2.0f; 
				x_shift = 0.0f; break;
			case 1: 
				framerate = 3.0f;
				if (x_shift < 5.0f){
					x_shift += 2.5f;
				}
				break;
			case 2: 
				framerate = 1.0f;
				if (x_shift > -10.0f){
					x_shift -= 5.0f;
				}
				break;
		}
	} else if (speaker == "Bianca"){
		switch (a->order[frame]){
			case 0: 
				framerate = 5.0f;
				if (x_shift < 40.0f){
					x_shift+=2.5f;
				}
				break;
			case 1:
				framerate = 4.0f;
				x_shift = 0.0f;
				break;
			case 2: 
				framerate = [&]() -> float{
					switch (frame){
						case 1: return 2.5f;
						case 3: return 1.0f;
						case 5: return 4.0f;
						default: return 0.0f;
					}
				}();
				x_shift = std::sinf(GetTime() * 60.0) * 2.0f;
				break;
		}
	} else if (speaker == "N"){
		switch (a->order[frame]){
			case 0: 
				framerate = 1.0f; 
				break;
			case 1: 
				framerate = 3.0f;
				if (x_shift < 5.0f){
					x_shift += 2.5f;
				}
				break;
			case 2: 
				framerate = 2.0f;
				if (x_shift > -10.0f){
					x_shift -= 5.0f;
				}
				break;
		}
	}
	if (allow_animated && frame < a->order.size() - 1){
		frametimer += GetFrameTime()*framerate;
		if (frametimer > 1.0f){
			frame++;
			frametimer = 0.0f;
		}
		if (speaker == "Bianca"){
			y_shift = (frame == 6) ? std::fabs(std::sinf(GetTime() * 20.0) * 25.0f) : 0.0f;
		} else {
			y_shift = 0.0f;
		}
	}
}

void anim_trainer_render_t::draw(assets_t& assets, std::string_view speaker, Vector3 pos, const Camera3D* camera, Color tint) const{
	const auto* a = find_anim(speaker);
	if (!a) return;
	const auto& speaker_tx = assets.get_tx("Animated Trainers.png");
	const Rectangle src{a->order[frame] * 84.0f, a->row * 78.0f, 84, 78};
	if (!camera){
		DrawTexturePro(speaker_tx, src, {pos.x + x_shift, pos.y - y_shift, 204, 190}, {0, 0}, 0, tint);
	} else {
		draw_shadow(speaker_tx, src, {pos.x + x_shift / 84.0f, 0.03f, pos.z + y_shift / 78.0f}, {0, 0, 0, std::min(static_cast<u8>(180), tint.a)});
		DrawBillboardPro(*camera, speaker_tx, src, {pos.x + x_shift / 84.0f, pos.y + y_shift / 78.0f, pos.z}, {0, 1, 0}, {trainer_scale, trainer_scale}, {0, 0}, 0, tint);
	}
}

void anim_trainer_render_t::update_back(){
	static constexpr std::array<float, 3> framerates = {0.3f, 7.0f, 0.0f};
	static constexpr std::array<float, 4> hand_framerates = {0.3f, 7.0f, 3.0f, 0.0f};
	static constexpr std::array<float, 4> hand_rotations = {0.0f, 0.0f, 5.0f, -0.5f};
	framerate = framerates[frame];
	frametimer += GetFrameTime()*framerate;
	if (frame < 2 && frametimer > 1.0f){
		frame++;
		frametimer = 0.0f;
	}
	if (hand_framerate < hand_framerates[hand_frame]) hand_framerate += 0.1f;
	if (hand_framerate > hand_framerates[hand_frame]) hand_framerate -= 0.1f;
	hand_rotation = hand_rotations[hand_frame];
	hand_frametimer += GetFrameTime()*hand_framerate;
	if (hand_frame < 3 && hand_frametimer > 1.0f){
		hand_frame++;
		hand_frametimer = 0.0f;
	}
	if (hand_frame >= 1 && hand_frame <= 2){
		x_shift -= 0.005f;
	} else if (hand_frame > 2 && x_shift_timer < 10.0f){
		x_shift += 0.02f;
		x_shift_timer += 1.0f;
	}
}

bool anim_trainer_render_t::done(std::string_view speaker) const{
	const auto* a = find_anim(speaker);
	return !a || frame >= a->order.size() - 1;
}

constexpr int get_vs_idx(std::string_view name){
	if (name == "Hilbert") return 0;
	if (name == "Hilda") return 1;
	if (name == "Cheren") return 2;
	if (name == "Bianca") return 3;
	if (name == "Chili") return 4;
	if (name == "Cress") return 5;
	if (name == "Cilan") return 6;
	if (name == "Lenora") return 7;
	if (name == "Burgh") return 8;
	if (name == "Elesa") return 9;
	if (name == "Clay") return 10;
	if (name == "Skyla") return 11;
	if (name == "Brycen") return 12;
	if (name == "Iris") return 13;
	if (name == "Drayden") return 14;
	if (name == "Shauntal") return 15;
	if (name == "Grimsley") return 16;
	if (name == "Marshal") return 17;
	if (name == "Caitlin") return 18;
	if (name == "Alder") return 19;
	if (name == "N") return 20;
	if (name == "Ghetsis") return 21;
	return 0;
}

void anim_trainer_render_t::draw_vs(assets_t& assets, std::string_view trainer, Vector2 pos, Color tint) const{
	auto tx = assets.get_tx("vs faces.png");
	int idx = get_vs_idx(trainer);
	float ren_x = (idx % 6) * 128.0f;
	float ren_y = (idx / 6) * 64.0f;
	DrawTexturePro(tx, {ren_x, ren_y, vs_w, vs_h}, {pos.x, pos.y, vs_w*4.0f, vs_h*4.0f}, {0, 0}, 0, tint);
}

void draw_trainer(assets_t& assets, TRAINER_TYPE tt, std::string_view name, Vector3 pos, const Camera3D* camera, Color tint){
	Texture2D speaker_tx = assets.get_tx("all-trainers.png");
	Vector2 src_offset = [&]() -> Vector2{
		if (!name.empty()) {
			if (name == "Hilbert")           return { 0, 0 };
			if (name == "Hilda")             return { 1, 0 };
			if (name == "Cheren")            return { 6, 4 };
			if (name == "Bianca")            return { 2, 0 };
			if (name == "N")                 return { 5, 5 };
			if (name == "Ghetsis")           return { 7, 10 };
			if (name == "Alder")             return { 6, 11 };
			if (name == "Cynthia")           return { 1, 12 };
			if (name == "Colress")           return { 2, 12 };
			if (name == "Shauntal")          return { 3, 10 };
			if (name == "Grimsley")          return { 5, 11 };
			if (name == "Caitlin")           return { 6, 10 };
			if (name == "Marshal")           return { 4, 10 };
			if (name == "Chili")           return { 2, 1 };
			if (name == "Cilan")           return { 3, 1 };
			if (name == "Cress")           return { 4, 1 };
			if (name == "Ingo" || name == "Emmet") return { 5, 11 };
		}
		switch (tt) {
			case TRAINER_TYPE::ACE_TRAINER:      return { 1, 3 };
			case TRAINER_TYPE::BACKPACKER:       return { 3, 3 };
			case TRAINER_TYPE::BATTLE_GIRL:      return { 5, 8 };
			case TRAINER_TYPE::BLACK_BELT:       return { 0, 7 };
			case TRAINER_TYPE::CYCLIST:          return { 0, 12 };
			case TRAINER_TYPE::FISHERMAN:        return { 5, 3 };
			case TRAINER_TYPE::HIKER:            return { 7, 8 };
			case TRAINER_TYPE::LADY:             return { 0, 6 };
			case TRAINER_TYPE::LASS:             return { 5, 0 };
			case TRAINER_TYPE::NURSERY_AIDE:     return { 1, 2 };
			case TRAINER_TYPE::PARASOL_LADY:     return { 6, 8 };
			case TRAINER_TYPE::POKEFAN:          return { 0, 8 };
			case TRAINER_TYPE::POKEMON_BREEDER:  return { 2, 2 };
			case TRAINER_TYPE::POKEMON_RANGER:   return { 0, 3 };
			case TRAINER_TYPE::PRESCHOOLER:      return { 7, 1 };
			case TRAINER_TYPE::PSYCHIC:          return { 3, 7 };
			case TRAINER_TYPE::RICH_BOY:         return { 7, 7 };
			case TRAINER_TYPE::SCHOOL_KID:       return { 4, 0 };
			case TRAINER_TYPE::VETERAN:          return { 5, 7 };
			case TRAINER_TYPE::YOUNGSTER:        return { 2, 0 };

			case TRAINER_TYPE::ARTIST:           return { 1, 4 };
			case TRAINER_TYPE::BAKER:            return { 2, 4 };
			case TRAINER_TYPE::CLERK:            return { 0, 1 };
			case TRAINER_TYPE::DOCTOR:           return { 1, 7 };
			case TRAINER_TYPE::HARLEQUIN:        return { 0, 4 };
			case TRAINER_TYPE::JANITOR:          return { 7, 7 };
			case TRAINER_TYPE::MAID:             return { 4, 11 };
			case TRAINER_TYPE::NURSE:            return { 4, 6 };
			case TRAINER_TYPE::PILOT:            return { 2, 12 };
			case TRAINER_TYPE::POLICEMAN:        return { 1, 6 };
			case TRAINER_TYPE::RANCHER:          return { 6, 2 };
			case TRAINER_TYPE::ROUGHNECK:        return { 4, 8 };
			case TRAINER_TYPE::SCIENTIST:        return { 4, 6 };
			case TRAINER_TYPE::WAITER:           return { 3, 1 };
			case TRAINER_TYPE::WAITRESS:         return { 1, 1 };
			case TRAINER_TYPE::WORKER:           return { 2, 3 };

			case TRAINER_TYPE::BATTER:           return { 6, 8 };
			case TRAINER_TYPE::HOOPSTER:         return { 3, 6 };
			case TRAINER_TYPE::INFIELDER:        return { 6, 8 };
			case TRAINER_TYPE::LINEBACKER:       return { 7, 0 };
			case TRAINER_TYPE::PITCHER:          return { 6, 8 };
			case TRAINER_TYPE::SMASHER:          return { 6, 0 };
			case TRAINER_TYPE::STRIKER:          return { 2, 7 };
			case TRAINER_TYPE::TENNIS_PLAYER:    return { 6, 0 };

			case TRAINER_TYPE::BACKERS:          return { 1, 9 };
			case TRAINER_TYPE::DOUBLE_TEAM:      return { 2, 9 };
			case TRAINER_TYPE::HOOLIGANS:        return { 4, 8 };
			case TRAINER_TYPE::TWINS:            return { 0, 2 };
			case TRAINER_TYPE::YOUNG_COUPLE:     return { 1, 9 };

			case TRAINER_TYPE::BOSS:             return { 7, 10 };
			case TRAINER_TYPE::CHAMPION:         return { 6, 11 };
			case TRAINER_TYPE::ELITE_FOUR:       return { 3, 10 };
			case TRAINER_TYPE::GYM_LEADER:       return { 2, 1 };
			case TRAINER_TYPE::PKMN_TRAINER:     return { 0, 0 };
			case TRAINER_TYPE::SUBWAY_BOSS:      return { 5, 11 };
			case TRAINER_TYPE::TEAM_PLASMA:      return { 3, 5 };
			case TRAINER_TYPE::TEAM_PLASMA_GRUNT:return { 3, 5 };
			default:                            return { 0, 0 };
		}
	}();
	if (!camera){
		DrawTexturePro(speaker_tx, {src_offset.x * 86, src_offset.y * 128, 86, 128}, {pos.x , pos.y, 204, 303}, {0, 0}, 0, tint);
	} else {
		draw_shadow(speaker_tx, {src_offset.x * 86, src_offset.y * 128, 86, 128}, {pos.x, 0.03f, pos.z / 64.0f}, {0, 0, 0, std::min(static_cast<u8>(180), tint.a)}, -1.0f);
		DrawBillboardPro(*camera, speaker_tx, {src_offset.x * 86, src_offset.y * 128, 86, 128}, {pos.x, pos.y / 128.0f + 0.1f, pos.z}, {0, 1, 0}, {trainer_scale, trainer_scale * 1.5f}, {0, 0}, 0, tint);
	}
}

void anim_trainer_render_t::draw_back(assets_t& assets, std::string_view name, Vector3 pos, const Camera3D& camera, Color tint) const{
	const auto& back_tx = assets.get_tx(std::format("{}-back.png", name));
	const auto& hand_tx = assets.get_tx(std::format("{}-back-hands.png", name));
	float tx_third_w = back_tx.width / 3.0f;
	std::array<Vector2, 4> hand_offsets = {
		{
			{0, 0},
			{-0.05f, 0.135f},
			{-0.05f, 0.135f},
			{-0.09f, -0.03f},
		}
	};
	DrawBillboardPro(camera, back_tx, {frame * tx_third_w, 0.0f, tx_third_w, static_cast<float>(back_tx.height)}, {(pos.x / tx_third_w) + x_shift, pos.y / back_tx.height + 0.1f, pos.z}, {0, 1, 0}, {0.3f, 0.5f}, {0, 0}, 0, tint);
	DrawBillboardPro(camera, hand_tx, {hand_frame * 46.0f, 0.0f, 46.0f, static_cast<float>(hand_tx.height)}, {(pos.x / 46.0f + 0.06f + hand_offsets[hand_frame].x) + x_shift + ((name == "hilda") ? 0.06f : 0.0f), pos.y / hand_tx.height + 0.1f + hand_offsets[hand_frame].y, pos.z + 0.02f}, {0, 1, 0}, {0.2f, 0.3f}, {0, 0}, hand_rotation, tint);
}
