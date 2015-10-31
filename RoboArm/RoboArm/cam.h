#pragma once
#include <math.h>

static class Camera{
	public:
		float camera_pos[3];
		float camera_target[3];
		float camera_dir[3];
		float camera_right[3];
		float camera_up[3];
		//static const float up[3];
		//float up[3];

		float radius;
		float angle;
		float camera_x;  
		float camera_z;

		//**** CONSTRUCTING INTIAL CAMERA ***////
		Camera(float x_pos, float y_pos, float z_pos,
			float x_tar, float y_tar, float z_tar);

		float getRadius();

		void updateCameraVector();

	private:
		float vec3Length(float x, float y, float z);

};
