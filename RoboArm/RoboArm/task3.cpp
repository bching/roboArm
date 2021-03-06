//Comment out for pc
//#include <GLUT/glut.h>
//#include <SDL2/SDL.h>
// Extension libraries for SDL2 to implement sound
//#include <SDL2_mixer/SDL_mixer.h>
//#include <SDL2_image/SDL_image.h>
//SDL for Windows
#include <SDL.h>

//=======================================================//
//comment out for mac
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
//======================================================//
//======================================================//
#include <iostream>
#include <stdlib.h>
//#include <math.h>
#include <string>

#include "cam.h"
//****** DONT MIND THESE METHODS *****//
void cleanUp_data(void);
void enable(void);
void drawCollision();
void sphereSphereCollision();
bool collidingBelow();
void collisionResponse(int index);
float doGravity(float y);
void updateVariables();
void updateCamera();
void drawClaw(float degrees, float x, float y, float z, int l);

//***** CAMERA STUFF *****//
Camera camera = Camera(0.0, 1.0, 10.0,
	0.0, 0.0, 0.0);
//const float Camera::up[3] = { 0.0, 1.0, 0.0 };
bool camera_left = false;
bool camera_right = false;
//**** GRAVITY STUFF ***//
float accel = -9.81; 
float velocity_fall = 0.0;
//*******************/
float ball_pos[3] = {2.0, 0.0, 0.5};
float ball_radius = 0.5;
float ball_vel_initial = 0.0;
bool xpiv_move = false; bool xneg_move = false;
bool ypiv_move = false; bool yneg_move = false;
bool zpiv_move = false; bool zneg_move = false;
//******PAD CONTACT***//
float pad_xpos = 1.5; float pad_ypos = 0.5; float pad_zpos = 2.0;
float baserot, armrot = 0.0;

//******ROBOT ARM STUFF ***//
bool claw_close = false; bool claw_open = false;
float claw_velocity = 0.01;

bool arm_left = false; bool arm_right = false;
bool arm_up = false; bool arm_down = false;
float robotXRot = 90, robotYRot = 0, clawDegrees = 0.25;
float robotArmHeight = 2, robotArmLength = 2, modelScale = 1;
float cableLength = 1.0;

//*****COLLISION SPHERES ****//
//THE INCREMENTATION OF clawDegrees is the velocity
struct contact_sphere{
	float x, y, z, r, v;
};
contact_sphere contact_pts[4];

GLUquadricObj *g_normalObject = NULL;

	
void init(void) {
	g_normalObject = gluNewQuadric();
}

void enable(void){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
}

void drawGround(void){
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
		for(int x = -10; x < 10; x+= 1){
			glVertex3f(x, -0.5, -10);
			glVertex3f(x, -0.5, 10);
			glVertex3f(-10, -0.5, x);
			glVertex3f(10, -0.5, x);
		}
	glEnd();
}

void drawBall(void){
	glColor3d(1, 0, 0);

	glPushMatrix();
		glTranslated(ball_pos[0], ball_pos[1], ball_pos[2]);
		glutSolidSphere(ball_radius, 50, 50);
	glPopMatrix();
}

void drawPad(void){
	glColor3d(1, 1, 1);
	glPushMatrix();
		glTranslated(pad_xpos, pad_ypos, pad_zpos);
		glScaled(1.0, 0.1, 1.0);
		glutWireSphere(0.5, 50, 50);
	glPopMatrix();
}

void drawRoboArm(){
	glPushMatrix();//Open robot arm
		//glScaled(modelScale, modelScale, modelScale);
		glColor3d(0.2, 0.3, 0.5);
		glTranslatef(0, 0, 0);
		glPushMatrix();//Open Base of arm
			glScalef(1.5, .1, 1.5);
			glutSolidCube(1);
		glPopMatrix();//Close base of arm
		glPushMatrix();//Open arm starting ant lower section
			//glTranslatef(0, robotArmHeight/2, 0);
			glRotatef(robotYRot, 0, 1, 0);
			glColor3d(0.9, 0.3, 0.5);
			glPushMatrix();//scale center mast
				//glScalef(.3, robotArmHeight, .3);
				glRotatef(-90, 1, 0, 0);
				gluCylinder(g_normalObject, 0.15, 0.15, robotArmHeight, 10, 10);
			glPopMatrix();//close scaling
			glPushMatrix();//Open joint
				glColor3d(.1, .9, .1);
				glTranslatef(0, robotArmHeight, 0);
				//glRotatef(90, 1, 0, 0);
				glutSolidSphere(.3, 10, 10);
				glPushMatrix();//Open upper arm
					//glTranslatef(0, robotArmLength/2, 0);
					glColor3d(0.9, 0.3, 0.0);
					//glScalef(.3, robotArmLength, .3);
					gluCylinder(g_normalObject, 0.15, 0.15, robotArmHeight, 10, 10);
					glPushMatrix();//Open joint and cable for claw
					glPushMatrix();
						glTranslatef(0, 0, robotArmLength);
						glutSolidSphere(.17, 10, 10);
					glPopMatrix();
						glTranslatef(0, 0, robotArmLength);
						glRotatef(90, 1, 0, 0);
						gluCylinder(g_normalObject, .05, .05, cableLength, 10, 10);
						glPushMatrix();//Open joint for claw
							glColor3d(.1, .0, .9);
							glTranslatef(0, 0, cableLength);
							glRotatef(-90, 1, 0, 0);
							glutSolidSphere(.3, 10, 10);
							glPushMatrix();//Open claw section
								drawClaw(90, .25, 0, 0, 0);
								drawClaw(-90, -.25, 0, 0, 1);
								drawClaw(0, 0, 0, .25, 2);
								drawClaw(180, 0, 0, -.25, 3);
							glPopMatrix();//Close claw section
						glPopMatrix();//Close joint for claw
					glPopMatrix();//Close Joint and Cable for claw
				glPopMatrix();//Close upper Arm
			glPopMatrix();//close joint
		glPopMatrix();//Close arm
	glPopMatrix();//Close robot arm
}

void drawClaw(float degrees, float x, float y, float z, int l){
	glPushMatrix();//Open claw
		glColor3f(0, 1, 0);
		glTranslatef(x, y, z);
		glRotatef(degrees, 0, 1, 0);
		//glRotatef(clawDegrees, 1, 0, 0);
		gluCylinder(g_normalObject, .05, .05, clawDegrees, 10, 10);
		glPushMatrix();//Next segment
			glTranslatef(0, 0, clawDegrees);
			glRotatef(90, 1, 0, 0);
			gluCylinder(g_normalObject, .05, .05, 1, 10, 10);
			glPushMatrix();//The part of the arm going back into the center
				glTranslatef(0, 0, 1);
				glRotatef(90, 1, 0, 0);
				gluCylinder(g_normalObject, 0.05, 0.05, .25, 10, 10);
				glPushMatrix();//Draw collision point
					glTranslatef(0, 0, 0.25);
					glColor3f(1, 0, 0);
					glutSolidSphere(contact_pts[l].r, 30, 30);
				glPopMatrix();//Close collision point
			glPopMatrix();//The part of the arm going back into the center
		glPopMatrix();//Close segment
	glPopMatrix();//Close claw
}

float doGravity(float y_pos){
	float velocity_old = velocity_fall;
	velocity_fall = velocity_fall + accel * 0.001;
	y_pos = y_pos + (velocity_old + velocity_fall) * 0.5 * 0.001;
	return y_pos;
}

void display(void) {
//	if(sphereSphereCollision()){
//		collisionResponse(o_velocity);
//	}

	sphereSphereCollision();
	updateCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	enable();

	updateVariables();

	glPushAttrib(GL_CURRENT_BIT);

	drawRoboArm();
	drawBall();
//	drawPad();

	drawGround();

	glPopAttrib();
	glutSwapBuffers();
}


void reshape(int w, int h) {
	const float ar = (float) w / (float) h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	if( ar > 0.5)
		glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
	else
		glFrustum(-1.0, 1.0, -1/ar, 1/ar, 2.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void sphereSphereCollision(){
	for(int i = 0; i < 4; i++){
		float distance = sqrt(((contact_pts[i].x - ball_pos[0]) * (contact_pts[i].x - ball_pos[0]))
								 + ((contact_pts[i].y - ball_pos[1]) * (contact_pts[i].y - ball_pos[1]))
								 + ((contact_pts[i].z - ball_pos[2]) * (contact_pts[i].z - ball_pos[2])));
		
		float sumRadius = contact_pts[i].r + ball_radius;	
		std::cout << "distance"<< distance << std::endl;
		std::cout << "sumRadius"<< sumRadius<< std::endl;

		if(distance <= sumRadius){
		//	std::cout << "collision here" << std::endl;
			collisionResponse(i);
		}
	}
}

//VELOCITIES ARE ALL THE ANGLE CHANGE OF CLAW
void collisionResponse(int index){
	/* These are points of collision but are not using them currently
	float x_coll = (o_pos[0]*ball_radius + ball_pos[0] * o_radius) / 
		(o_radius + ball_radius);
	float y_coll = (o_pos[1]*ball_radius + ball_pos[1] * o_radius) / 
		(o_radius + ball_radius);
	float z_coll = (o_pos[2]*ball_radius + ball_pos[2] * o_radius) / 
		(o_radius + ball_radius);
		*/
	//for(int i = 0; i < 4; i++){
	float y_coll = (contact_pts[index].y * ball_radius 
			+ ball_pos[1] * contact_pts[index].r) / (contact_pts[index].r + ball_radius);

	float contact_vel_x = (claw_velocity * (contact_pts[index].r - ball_radius))
		+ (2*ball_radius*ball_vel_initial)
		/ (contact_pts[index].r + ball_radius);
	float contact_vel_y = (claw_velocity * (contact_pts[index].r - ball_radius))
		+ (2*ball_radius*ball_vel_initial)
		/ (contact_pts[index].r + ball_radius);
	float contact_vel_z = (claw_velocity * (contact_pts[index].r - ball_radius))
		+ (2*ball_radius*ball_vel_initial)
		/ (contact_pts[index].r + ball_radius);
	
	float ball_vel_x = (ball_vel_initial * (ball_radius - contact_pts[index].r))
		+ (2*contact_pts[index].r*contact_vel_x)
		/ (contact_pts[index].r + ball_radius);
	float ball_vel_y = (ball_vel_initial* (ball_radius - contact_pts[index].r))
		+ (2*contact_pts[index].r*contact_vel_y)
		/ (contact_pts[index].r + ball_radius);
	float ball_vel_z = (ball_vel_initial * (ball_radius - contact_pts[index].r))
		+ (2*contact_pts[index].r*contact_vel_z)
		/ (contact_pts[index].r + ball_radius);

	//WE ARE NOT MOVING THE CONTACT SPHERES ON CONTACT
	contact_pts[index].x = contact_pts[index].x + contact_vel_x;
	contact_pts[index].y = contact_pts[index].y + contact_vel_y;
	contact_pts[index].z = contact_pts[index].z + contact_vel_z;

		std::cout << "translating here" << std::endl;
	ball_pos[0] = ball_pos[0] + ball_vel_x;
	if(y_coll < 0.35 && ball_pos[1] > 0){
		ball_pos[1] = doGravity(ball_pos[1]);
	} 
	ball_pos[2] = ball_pos[2] + ball_vel_z;
//	}
}

void keyboard(unsigned char key, int x, int y) {
	switch(key){
		case 'a':
			if (arm_left == false){
				if (arm_right)
					arm_right = false;
				arm_left = true;
			}
			else
				arm_left = false;
			break;
		case 'd':
			if (arm_right == false){
				if (arm_left)
					arm_left = false;
				arm_right = true;
			}
			else
				arm_right = false;
			break;
		case 'w':
			if (!arm_up){
				if (arm_down)
					arm_down = false;
				arm_up = true;
			}
			else
				arm_up = false;
			break;
		case 's':
			if (!arm_down){
				if (arm_up)
					arm_up = false;
				arm_down = true;
			}
			else
				arm_down = false;
			break;
		case 'q':
			if (!claw_close){
				if (claw_open)
					claw_open = false;
				claw_close = true;
			}
			else
				claw_close = false;
			break;
		case 'e':
			if (!claw_open){
				if (claw_close)
					claw_close = false;
				claw_open = true;
			}
			else
				claw_open = false;
			break;
		//**** CAMERA CONTROLS ***//
		case 'o':
			if(camera_left == false && camera_right != true)
				camera_left = true;
			else
				camera_left = false;
			break;
		case 'p':
			if(camera_right == false && camera_left != true)
				camera_right = true;
			else
				camera_right = false;
			break;
	}

	if(key == 27)
		exit(0);
}

void updateVariables(void){

	//USE THESE FOR THE CONTACT SPHERE STRUCT

	if(arm_up == true && cableLength > 0)
		cableLength -= 0.001;
	if(arm_down == true && cableLength < robotArmHeight-.5)
		cableLength += 0.001;

	if(arm_left == true)
		robotYRot += 0.5;
	if(arm_right == true)
		robotYRot -= 0.5;

	if(claw_open == true && clawDegrees < 1)
		clawDegrees += claw_velocity;
	if(claw_close == true && clawDegrees > .25)
		clawDegrees -= claw_velocity;

	//*** CAMERA BOOLEANS ***//
	if(camera.angle >= 360)
		camera.angle -= 360;
	if(camera.angle <= -360)
		camera.angle += 360;
	if(camera_left == true)
		camera.angle += 0.01;
	if(camera_right == true)
		camera.angle -= 0.01;
}

void updateCamera(){
	float camera_x = sin(camera.angle) * camera.getRadius();
	float camera_y = cos(camera.angle) * camera.getRadius();
	glLoadIdentity();
	gluLookAt(camera_x, 2.0, camera_y,
		camera.camera_target[0], camera.camera_target[1], camera.camera_target[2],
		0.0, 1.0, 0.0);
}

void cleanUp_data(void){
	if(g_normalObject)
		gluDeleteQuadric(g_normalObject);
}

////////////////////////////////////////////////////////////////
// Vsync for Windows
////////////////////////////////////////////////////////////////
void setVSync(bool sync)
{
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char *extensions = (char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
	{
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(sync);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

	for (int i = 0; i < sizeof(contact_pts); i++){
		contact_pts[i].x = 0;
		contact_pts[i].y = 0;
		contact_pts[i].z = 0;
		contact_pts[i].r = 0.1;
		contact_pts[i].v = claw_velocity;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(40, 40);
	glutCreateWindow("The Robot Arm!");

	init();
	//////////////////////
	//Comment out for Mac/
	setVSync(1);		//
	//////////////////////
	//////////////////////

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);

	//environment background color
	glClearColor(0.9, 0.2, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//light and material in the environment
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glutMainLoop();
	cleanUp_data();

	return 0;
}


