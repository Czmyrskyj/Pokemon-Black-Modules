#pragma once
#include <vector>
#include "common.hpp"

struct Camera3D;
struct camera_t{
	Camera3D camera;
	usz camera_idx = 0;
	camera_t();
	void set_camera_route(const std::vector<Vector3>& route, float speed = 0.1f);
};
