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
	auto texture = assets.get_tx((sex == SEX::MALE) ? "hilbert.png" : "hilda.png");
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

bool anim_trainer_render_t::draw(assets_t& assets, std::string_view speaker, Vector3 pos, const Camera3D* camera, Color tint, bool allow_animated){
	Texture2D speaker_tx = assets.get_tx("Animated Trainers.png");
	Vector2 src_offset{0.0f, 0.0f};
	std::vector<u8> frameorder{};
	if (speaker == "Cheren"){
		src_offset = {0, 1};
		frameorder = {0, 1, 2};
		switch (frameorder[frame]){
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
		src_offset = {0, 0};
		frameorder = {1, 2, 1, 2, 1, 2, 0, 0};
		switch (frameorder[frame]){
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
		src_offset = {0, 2};
		frameorder = {0, 1, 2};
		switch (frameorder[frame]){
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
	if (allow_animated && frame < frameorder.size() - 1){
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
	src_offset.x = frameorder[frame];
	if (!camera){
		DrawTexturePro(speaker_tx, {src_offset.x * 84, src_offset.y * 78, 84, 78}, {pos.x + x_shift, pos.y - y_shift, 204, 190}, {0, 0}, 0, tint);
	} else {
		draw_shadow(speaker_tx, {src_offset.x * 84, src_offset.y * 78, 84, 78}, {pos.x + x_shift / 84.0f, 0.03f, pos.z + y_shift / 78.0f}, {0, 0, 0, std::min(static_cast<u8>(180), tint.a)});
		DrawBillboardPro(*camera, speaker_tx, {src_offset.x * 84, src_offset.y * 78, 84, 78}, {pos.x + x_shift / 84.0f, pos.y + y_shift / 78.0f, pos.z}, {0, 1, 0}, {trainer_scale, trainer_scale}, {0, 0}, 0, tint);
	}
	return frame >= frameorder.size() - 1;
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

void anim_trainer_render_t::draw_vs(assets_t& assets, std::string_view trainer, Vector2 pos, Color tint){
	auto tx = assets.get_tx("vs faces.png");
	int idx = get_vs_idx(trainer);
	float ren_x = (idx % 6) * 128.0f;
	float ren_y = (idx / 6) * 64.0f;
	DrawTexturePro(tx, {ren_x, ren_y, vs_w, vs_h}, {pos.x, pos.y, vs_w*4.0f, vs_h*4.0f}, {0, 0}, 0, tint);
}

void draw_trainer(assets_t& assets, std::string_view prefix, std::string_view trainer, Vector3 pos, const Camera3D* camera, Color tint){
	Texture2D speaker_tx = assets.get_tx("all-trainers.png");
	Vector2 src_offset = [&]() -> Vector2{
		if (trainer == "Hilbert") return Vector2{0, 0};
		else if (trainer == "Hilda") return Vector2{1, 0};
		else if (trainer == "Youngster") return Vector2{2, 0};
		else if (prefix == "Lass") return Vector2{3, 0};
		else if (prefix == "School Kid") return Vector2{4, 0};
		else if (prefix == "School Kid") return Vector2{5, 0};
		else if (prefix == "Lady") return Vector2{6, 0};
		else if (prefix == "Linebacker") return Vector2{7, 0};
		else if (prefix == "Waiter") return Vector2{0, 1};
		else if (prefix == "Waitress") return Vector2{1, 1};
		else if (trainer == "Chili") return Vector2{2, 1};
		else if (trainer == "Cress") return Vector2{3, 1};
		else if (trainer == "Cilan") return Vector2{4, 1};
		else if (prefix == "Nursery") return Vector2{5, 1};
		else if (prefix == "Preschooler") return Vector2{6, 1};
		else if (prefix == "Preschooler") return Vector2{7, 1};
		else if (prefix == "Twins") return Vector2{0, 2};
		else if (prefix == "Janitor") return Vector2{1, 2};
		else if (prefix == "Backpacker") return Vector2{2, 2};
		else if (trainer == "Lenora") return Vector2{3, 2};
		else if (trainer == "Elesa") return Vector2{5, 2};
		else if (trainer == "Clay") return Vector2{6, 2};
		else if (trainer == "Skyla") return Vector2{7, 2};
		else if (prefix == "Hoopster") return Vector2{0, 3};
		else if (prefix == "Clerk") return Vector2{1, 3};
		else if (prefix == "Worker") return Vector2{2, 3};
		else if (prefix == "Backpacker") return Vector2{3, 3};
		else if (prefix == "Hiker") return Vector2{4, 3};
		else if (prefix == "Fisherman") return Vector2{5, 3};
		else if (prefix == "Musician") return Vector2{6, 3};
		else if (prefix == "Breakdancer") return Vector2{7, 3};
		else if (prefix == "Harlequin") return Vector2{0, 4};
		else if (prefix == "Artist") return Vector2{1, 4};
		else if (prefix == "Baker") return Vector2{2, 4};
		else if (prefix == "Psychic Male") return Vector2{3, 4};
		else if (prefix == "Psychic Female") return Vector2{4, 4};
		else if (trainer == "Brycen") return Vector2{0, 5};
		else if (prefix == "Team Plasma Grunt") return Vector2{3, 5};
		else if (trainer == "Iris") return Vector2{4, 7};
		else if (prefix == "Rich Boy") return Vector2{0, 6};
		else if (prefix == "Depot Agent") return Vector2{1, 6};
		else if (prefix == "Worker") return Vector2{2, 6};
		else if (prefix == "Infielder") return Vector2{3, 6};
		else if (prefix == "Scientist Male") return Vector2{4, 6};
		else if (prefix == "Scientist Female") return Vector2{5, 6};
		else if (prefix == "Ace Trainer Female") return Vector2{6, 6};
		else if (prefix == "Ace Trainer Male") return Vector2{7, 6};
		else if (prefix == "Black Belt") return Vector2{0, 7};
		else if (prefix == "Doctor") return Vector2{1, 7};
		else if (prefix == "Striker") return Vector2{2, 7};
		else if (trainer == "Drayden") return Vector2{3, 7};
		else if (trainer == "Shauntal") return Vector2{3, 10};
		else if (trainer == "Marshal") return Vector2{5, 7};
		else if (trainer == "Grimsley") return Vector2{6, 7};
		else if (trainer == "Caitlin") return Vector2{6, 10};
		else if (prefix == "Pokéfan Male (Child)") return Vector2{0, 8};
		else if (prefix == "Pokéfan Female") return Vector2{1, 8};
		else if (prefix == "Nurse") return Vector2{2, 8};
		else if (prefix == "Nurse (Alternative)") return Vector2{3, 8};
		else if (prefix == "Biker") return Vector2{4, 8};
		else if (prefix == "Battle Girl") return Vector2{5, 8};
		else if (prefix == "Parasol Lady") return Vector2{6, 8};
		else if (prefix == "Clerk Male") return Vector2{7, 8};
		else if (prefix == "Gentleman") return Vector2{0, 9};
		else if (prefix == "Smuggler") return Vector2{1, 9};
		else if (prefix == "Cheerleaders") return Vector2{2, 9};
		else if (prefix == "Roughneck") return Vector2{3, 9};
		else if (prefix == "Madame") return Vector2{4, 9};
		else if (prefix == "Cyclist") return Vector2{5, 9};
		else if (prefix == "Pitcher") return Vector2{6, 9};
		else if (prefix == "Hiker") return Vector2{7, 9};
		else if (prefix == "Socialite") return Vector2{0, 10};
		else if (prefix == "Veteran Male") return Vector2{1, 10};
		else if (prefix == "Veteran Female") return Vector2{2, 10};
		else if (prefix == "Shadow Triad") return Vector2{3, 10};
		else if (prefix == "Alder") return Vector2{4, 10};
		else if (prefix == "Ingo") return Vector2{5, 10};
		else if (prefix == "Ghetsis") return Vector2{6, 10};
		else if (prefix == "Ghetsis (Battle)") return Vector2{7, 10};
		else if (prefix == "Officer") return Vector2{0, 11};
		else if (prefix == "Swimmer Male") return Vector2{1, 11};
		else if (prefix == "Swimmer Female") return Vector2{2, 11};
		else if (prefix == "Police Officer") return Vector2{3, 11};
		else if (prefix == "Maid") return Vector2{4, 11};
		else if (trainer == "Emmet") return Vector2{5, 11};
		else if (trainer == "Alder") return Vector2{6, 11};
		else if (prefix == "Cyclist Male") return Vector2{7, 11};
		else if (prefix == "Cyclist Female") return Vector2{0, 12};
		else if (trainer == "Cynthia") return Vector2{1, 12};
		else if (trainer == "Colress") return Vector2{2, 12};
		return Vector2{-1, -1};
	}();
	if (!camera){
		DrawTexturePro(speaker_tx, {src_offset.x * 86, src_offset.y * 128, 86, 128}, {pos.x , pos.y, 204, 303}, {0, 0}, 0, tint);
	} else {
		draw_shadow(speaker_tx, {src_offset.x * 86, src_offset.y * 128, 86, 128}, {pos.x, 0.03f, pos.z / 64.0f}, {0, 0, 0, std::min(static_cast<u8>(180), tint.a)}, -1.0f);
		DrawBillboardPro(*camera, speaker_tx, {src_offset.x * 86, src_offset.y * 128, 86, 128}, {pos.x, pos.y / 128.0f + 0.1f, pos.z}, {0, 1, 0}, {trainer_scale, trainer_scale * 1.5f}, {0, 0}, 0, tint);
	}
}

bool anim_trainer_render_t::draw_back(assets_t& assets, std::string_view name, Vector3 pos, const Camera3D& camera, Color tint){
	static constexpr std::array<float, 3> framerates = {0.4f, 7.0f, 0.0f};
	framerate = framerates[frame];
	frametimer += GetFrameTime()*framerate;
	if (frametimer > 1.0f){
		frame++;
		frametimer = 0.0f;
	}
	static constexpr std::array<float, 4> hand_framerates = {0.5f, 7.0f, 3.0f, 0.0f};
	static constexpr std::array<float, 4> hand_rotations = {0.0f, 0.0f, 5.0f, -0.5f};
	std::array<Vector2, 4> hand_offsets = {
		{
			{0, 0},
			{-0.05f, 0.135f},
			{-0.05f, 0.135f},
			{-0.09f, -0.03f},
		}
	};
	if (hand_framerate < hand_framerates[hand_frame]) hand_framerate += 0.1f;
	if (hand_framerate > hand_framerates[hand_frame]) hand_framerate -= 0.1f;
	hand_rotation = hand_rotations[hand_frame];
	hand_frametimer += GetFrameTime()*hand_framerate;
	if (hand_frametimer > 1.0f){
		hand_frame++;
		hand_frametimer = 0.0f;
	}
	const auto& back_tx = assets.get_tx(std::format("{}-back.png", name));
	const auto& hand_tx = assets.get_tx(std::format("{}-back-hands.png", name));
	float tx_third_w = back_tx.width / 3.0f;
	if (hand_frame >= 1 && hand_frame <= 2){
		x_shift -= 0.005f;
	} else if (hand_frame > 2 && x_shift_timer < 10.0f){
		x_shift += 0.02f;
		x_shift_timer += 1.0f;
	}
	DrawBillboardPro(camera, back_tx, {frame * tx_third_w, 0.0f, tx_third_w, static_cast<float>(back_tx.height)}, {(pos.x / tx_third_w) + x_shift, pos.y / back_tx.height + 0.1f, pos.z}, {0, 1, 0}, {0.3f, 0.5f}, {0, 0}, 0, tint);
	DrawBillboardPro(camera, hand_tx, {hand_frame * 46.0f, 0.0f, 46.0f, static_cast<float>(hand_tx.height)}, {(pos.x / 46.0f + 0.06f + hand_offsets[hand_frame].x) + x_shift + ((name == "hilda") ? 0.06f : 0.0f), pos.y / hand_tx.height + 0.1f + hand_offsets[hand_frame].y, pos.z + 0.02f}, {0, 1, 0}, {0.2f, 0.3f}, {0, 0}, hand_rotation, tint);
	return hand_frame == 3;
}
