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

void camera_t::set_camera_route(const std::vector<Vector3>& route, float speed){
	if (camera_idx < route.size()){
		if (camera.position.x < route[camera_idx].x){
			camera.position.x += speed;
		}
		if (camera.position.x > route[camera_idx].x){
			camera.position.x -= speed;
		}
		if (camera.position.y < route[camera_idx].y){
			camera.position.y += speed;
		}
		if (camera.position.y > route[camera_idx].y){
			camera.position.y -= speed;
		}
		if (camera.position.z < route[camera_idx].z){
			camera.position.z += speed;
		}
		if (camera.position.z > route[camera_idx].z){
			camera.position.z -= speed;
		}
		if (camera.position.x == route[camera_idx].x &&
				camera.position.y == route[camera_idx].y &&
				camera.position.z == route[camera_idx].z){
			camera_idx ++;
		}
	}
}
