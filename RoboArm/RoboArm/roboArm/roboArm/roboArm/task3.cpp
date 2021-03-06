//
//Comment out for pc
#include <GLUT/glut.h>
#include <SDL2/SDL.h>
// Extension libraries for SDL2 to implement sound
#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2_image/SDL_image.h>

//=======================================================//
//comment out for mac
//#include <GL/glut.h>
//#include <GL/glu.h>
//#include <GL/gl.h>
//======================================================//
//======================================================//
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>

float xpos, ypos, zpos, xrot, yrot, angle = 0.0;

float ball_xpos = 2.0; float ball_ypos = 0.5; float ball_zpos = 0.5; float ball_radius = 0.5;
float pad_xpos = 1.5; float pad_ypos = 0.5; float pad_zpos = 2.0;
//Radius from camera to point of focus 
float cradius = 10.0f;
float lastx, lasty;
float RobotXRot = 90, RobotYRot = 0, ClawXRot = 0;

GLUquadricObj *g_normalObject = NULL;
void cleanUp_data(void);
void enable(void);
float drawCollision(float c);

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

void drawBall(void){
	glPushAttrib(GL_CURRENT_BIT);
	glColor3d(1, 0, 0);

	glPushMatrix();
		glTranslated(ball_xpos, ball_ypos, ball_zpos);
		glutSolidSphere(ball_radius, 50, 50);
	glPopMatrix();

	glPopAttrib();
}

void drawPad(void){
	glPushAttrib(GL_CURRENT_BIT);

	glColor3d(1, 1, 1);
	glPushMatrix();
		glTranslated(pad_xpos, pad_ypos, pad_zpos);
		glScaled(1.0, 0.1, 1.0);
		glutWireSphere(0.5, 50, 50);
	glPopMatrix();

	glPopAttrib();
}

GLvoid DrawRoboArm(){
	glPushMatrix();//Open robot arm
		glScaled(1.5, 1.5, 1.5);
		glColor3d(0.2, 0.3, 0.5);
		glTranslatef(0, 0, 0);
		glPushMatrix();//Open Base of arm
			glScalef(1, .1, 1);
			glutSolidCube(1.5);
		glPopMatrix();//Close base of arm
		glPushMatrix();//Open arm starting ant lower section
			glTranslatef(0, 1, 0);
			glRotatef(RobotYRot, 0, 1, 0);
			glColor3d(0.9, 0.3, 0.5);
			glPushMatrix();//scale center mast
				glScalef(.3, 2, .3);
				glutSolidCube(1);
			glPopMatrix();//close scaling
			glPushMatrix();//Open joint
				glColor3d(.1, .9, .1);
				glTranslatef(0, 1, 0);
				glRotatef(RobotXRot, 1, 0, 0);
				glutSolidSphere(.3, 10, 10);
				glPushMatrix();//Open upper arm
					glTranslatef(0, .82, 0);
					glColor3d(0.9, 0.3, 0.0);
					glPushMatrix();//scale center mast
						glScalef(.3, 2, .3);
						glutSolidCube(1);
					glPopMatrix();//close scaling
					glPushMatrix();//Open joint for claw
						glColor3d(.1, .0, .9);
						glTranslatef(0, 1, 0);
						glRotatef(ClawXRot, 1, 0, 0);
						glutSolidSphere(.3, 10, 10);
					glPopMatrix();//Close joint for claw
				glPopMatrix();//Close upper Arm
			glPopMatrix();//close joint
		glPopMatrix();//Close arm
	glPopMatrix();//Close robot arm
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	enable();
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -cradius);
	glRotatef(xrot, 1.0, 0.0, 0.0);
	glRotatef(yrot, 0.0, 1.0, 0.0);

	DrawRoboArm();
	drawBall();
	drawPad();

//	glRotatef(yrot, 0.0, 1.0, 0.0);
//	glTranslated(-xpos, 0.0, -zpos);

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

float drawCollision(float color)
{
	float xrot_arm_rad = RobotXRot*M_PI/180;


	float distance;
	distance = sqrt(( (ball_xpos - xpos_arm) * (ball_xpos - xpos_arm) )
			         + ((ball_ypos - ypos_arm) * (ball_ypos - ypos_arm))
			         + ((ball_zpos - zpos_arm) * (ball_zpos - zpos_arm)));
	//float difference = distance - (radius_horse + radius_spider);

//	if(distance <= (ball_radius + 

	return 0;
}

void keyboard(unsigned char key, int x, int y) {

	switch(key){
		case 'a':
			RobotYRot++;
			if (RobotYRot >= 360)
				RobotYRot = 0;
			break;
		case 'd':
			RobotYRot--;
			if (RobotYRot < 0)
				RobotYRot = 360 - 1;
			break;
		case 'w':
			if (RobotXRot > 0)
				RobotXRot--;
			break;
		case 's':
			if (RobotXRot < 160)
				RobotXRot++;
			break;
	}

	if(key == 27)
		exit(0);

}

void mouseMovement(int x, int y) {
	int diffx = x - lastx; 
	int diffy = y - lasty;
	lastx = x;
	lasty = y;
	xrot += (float)diffy;
	yrot += (float)diffx;
}

void cleanUp_data(void){
	if(g_normalObject)
		gluDeleteQuadric(g_normalObject);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(40, 40);
	glutCreateWindow("The Robot Arm!");

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
//	glutSpecialFunc(special);

	glutPassiveMotionFunc(mouseMovement);

	//environment background color
	glClearColor(0.9, 0.2, 0.4, 1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//light and material in the environment
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	//gluLookAt(0, 1, -3, 0, 0, 0, 0, 1, 0);

	glutMainLoop();
	cleanUp_data();

	return 0;
}

