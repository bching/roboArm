#pragma once

class Camera{
	public:
		float camera_pos[3];
		float camera_target[3];
		float camera_dir[3];
		float camera_right[3];
		float camera_up[3];
		float up[3] = {0.0, 1.0, 0.0};

		float radius = 15.0f;
		float angle = 0.0;
		float camera_x;  
		float camera_z;

		//**** CONSTRUCTING INTIAL CAMERA ***////
		Camera(float x_pos, float y_pos, float z_pos,
				float x_tar, float y_tar, float z_tar){

			camera_pos[0] = x_pos; camera_pos[1] = y_pos; camera_pos[2] = z_pos;
			camera_target[0] = x_tar; camera_target[1] = y_tar; camera_target[2] = z_tar;

			float length = vec3Length(
					camera_pos[0] - camera_target[0], 
					camera_pos[1] - camera_target[1],
					camera_pos[2] - camera_target[2]);

			//*** NORMALIZED VECTOR ***/
			camera_dir[0] = (camera_pos[0] - camera_target[0])/length;
			camera_dir[1] = (camera_pos[1] - camera_target[1])/length;
			camera_dir[2] = (camera_pos[2] - camera_target[2])/length;
		}

		void updateCameraVector(){
			float cross_prod[3];
			cross_prod[0] = up[0]*camera_dir[2] - up[2]*camera_dir[1];
			cross_prod[1] = up[2]*camera_dir[0] - up[0]*camera_dir[2];
			cross_prod[2] = up[0]*camera_dir[1] - up[1]*camera_dir[0];
			float length = vec3Length(cross_prod[0], cross_prod[1], cross_prod[2]);
			camera_right[0] = cross_prod[0]/length;
			camera_right[1] = cross_prod[1]/length;
			camera_right[2] = cross_prod[2]/length;

			//*** NORMALIZED VECTOR ***/
			camera_up[0] = camera_dir[0]*camera_right[2] - camera_dir[2]*camera_right[1];
			camera_up[1] = camera_dir[2]*camera_right[0] - camera_dir[0]*camera_right[2];
			camera_up[2] = camera_dir[0]*camera_right[1] - camera_dir[1]*camera_right[0];
		}

	private:
		float vec3Length(float x, float y, float z){
			float length = sqrt(x*x + y*y + z*z);
			return length;
		}

};
