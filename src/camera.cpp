#include "camera.hpp"

camera_t::camera_t(){
	camera = {
		.position = {0, 0, 0},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 65,
		.projection = CAMERA_PERSPECTIVE,
	};
};
