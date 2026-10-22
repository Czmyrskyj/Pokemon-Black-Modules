#include "camera.hpp"
#include <cmath>

camera_t::camera_t(){
	camera = {
		.position = {0, 0, 0},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
};

void camera_t::set_route(const std::vector<Vector3>& route, float speed){
	if (camera_idx >= route.size()){
		camera_idx = 0;
		return;
	}

	const auto& target = route[camera_idx];
	auto update_axis = [](float& current, float target_val, float spd) -> bool{
		if (std::fabs(current - target_val) <= spd){
			current = target_val;
			return true;
		} else if (current < target_val){
			current += spd;
		} else {
			current -= spd;
		}
		return false;
	};

	bool x_done = update_axis(camera.position.x, target.x, speed);
	bool y_done = update_axis(camera.position.y, target.y, speed);
	bool z_done = update_axis(camera.position.z, target.z, speed);
	if (x_done && y_done && z_done){
		camera_idx++;
	}

}


void camera_t::set_targ(Vector3 targ){
	camera.target = targ;
}
