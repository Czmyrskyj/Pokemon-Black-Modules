#include "trainer.hpp"
#include "assets.hpp"
#include <raylib.h>
#include <cmath>

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



void anim_trainer_render_t::draw(assets_t& assets, std::string_view speaker, Vector2 pos){
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
		frameorder = {1, 2, 1, 2, 0, 0};
		switch (frameorder[frame]){
			case 0: 
				framerate = 5.0f;
				if (x_shift < 20.0f){
					x_shift+=2.5f;
				}
				break;
			case 1:
				framerate = 3.0f;
				x_shift = 0.0f;
				break;
			case 2: 
				framerate = 1.5f;
				x_shift = std::sinf(GetTime() * 50.0) * 4.0f;
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
	if (frame < frameorder.size() - 1){
		frametimer += GetFrameTime()*framerate;
		if (frametimer > 1.0f){
			frame++;
			frametimer = 0.0f;
		}
		if (speaker == "Bianca"){
			y_shift = (frame == 4) ? std::fabs(std::sinf(GetTime() * 30.0) * 20.0f) : 0.0f;
		} else {
			y_shift = 0.0f;
		}
	}
	src_offset.x = frameorder[frame];
	DrawTexturePro(speaker_tx, {src_offset.x * 84, src_offset.y * 78, 84, 78}, {pos.x + x_shift, pos.y - y_shift, 204, 190}, {0, 0}, 0, WHITE);

}
