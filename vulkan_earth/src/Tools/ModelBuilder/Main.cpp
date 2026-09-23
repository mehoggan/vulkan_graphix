#include <iostream>
#include <glew.h>
#include <glut.h>
#include "math.h"
#include "VBOShaderLibrary.h"

using namespace std;

float vertices[18] =	{	
							-2.1, 0, 2.1,
							-2.1, 0, -2.1,
							2.1, 0, 2.1,
							-2.1, 0, -2.1,
							2.1, 0, -2.1,
							2.1, 0, 2.1
						};
float normals[18] =		{	
							0.0, 0.0, 1.0, 
							0.0, 0.0, 1.0, 
							0.0, 0.0, 1.0, 
							0.0, 0.0, 1.0, 
							0.0, 0.0, 1.0, 
							0.0, 0.0, 1.0
						};
float tex_coord[12] =	{
							0.0, 0.0, 
							1.0, 0.0, 
							0.0, 1.0, 
							1.0, 0.0, 
							1.0, 1.0, 
							0.0, 1.0
						};


int winWidth,winHeight;
VBOShaderLibrary vbo_shader_turret;
VBOShaderLibrary vbo_shader_body;
VBOShaderLibrary vbo_shader_head;
int prevMousePositionX = 0;
int rotation = 0;

void resize(int, int);
void draw();
void timerEvent(int);
void initStuff();
void mouseFunc( int x, int y);

int main(int argc, char** argv) {
	bool run = true;
	/*if(argc < 2) {
		cout << "You must specify which file you want rendered" << endl;
		system("pause");
		return 1;
		run = false;
	}*/
	if(run) {
		cout << "Hello Cruel World" << endl;
	
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(800,600);
		glutCreateWindow("VulkanEarth");
		glutMotionFunc(mouseFunc); 
		
		/*	CODE NEEDED TO USE SHADERS AND VBOS	*/
		if(!(VBOShaderLibrary::InitGlew())) {
			cout << "ERROR: Initializing Glew " << endl;
			exit(0);
		}else if(!(VBOShaderLibrary::AreVBOsSupported())){
			cout << "ERROR: Your Computer Does Not Support VBO's" << endl;
			exit(0);
		}

		vbo_shader_turret.getVBOPointerFunctions();
		vbo_shader_body.getVBOPointerFunctions();
		vbo_shader_head.getVBOPointerFunctions();

		vbo_shader_turret.loadClientData("./Rhinoxx_Tank/Rhinoxx_Turret.ogl");
		vbo_shader_body.loadClientData("./Rhinoxx_Tank/Rhinoxx_Body.ogl");
		vbo_shader_head.loadClientData("./Rhinoxx_Tank/Rhinoxx_Head.ogl");

		vbo_shader_turret.loadShaders("VertexShader.vs","FragmentShader.vs");
		vbo_shader_body.loadShaders("VertexShader.vs","FragmentShader.vs");
		vbo_shader_head.loadShaders("VertexShader.vs","FragmentShader.vs");

		vbo_shader_turret.LoadTexture("TestImage.raw",1024,1024);
		vbo_shader_body.LoadTexture("TestImage.raw",1024,1024);
		vbo_shader_head.LoadTexture("TestImage.raw",1024,1024);

		glutReshapeFunc(resize);
		glutDisplayFunc(draw);
		glutTimerFunc(20,timerEvent,1);
		initStuff();
		glutMainLoop();
	}else {
		cout << "Good by cruel world" << endl;
	}

	return 0;
}

void resize(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(60.0, (float)width / (float)height, 1.0, 1000000.0);
	glMatrixMode(GL_MODELVIEW);
}

void timerEvent(int msec) {
	glutTimerFunc(msec, timerEvent, 1);
	draw();
}

void draw() {
	glClearColor(0, 0, 0, 0);                       	// background color
	glClearDepth(1.0f);                             	// 0 is near, 1 is far
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 6, 6, 0, 0, 0, 0, 1, 0);
	//gluLookAt(3, 0, 0, 0, 0, 0, 0, 0, 1);

	/*	CODE NEEDED TO USE SHADERS AND VBOS	*/
	glRotatef(rotation,0.0f,1.0f,0.0f);
	vbo_shader_body.drawClientData();
	vbo_shader_head.drawClientData();
	glTranslatef(0.0f,1.5f,0.0f);
	vbo_shader_turret.drawClientData();
	/*	CODE NEEDED TO USE SHADERS AND VBOS	*/

	glutSwapBuffers();
}

void initStuff(){
	GLfloat lightA[] = {0.3f, 0.3f, 0.3f, 1.0f};      // ambient light
	GLfloat lightD[] = {1.0f, 1.0f, 1.0f, 1.0f};   // diffuse light
	GLfloat lightS[] = {1.0f, 1.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightA);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightD);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightS);
	GLfloat lightDir0[4] = {1.0f,1.0f,1.0f,0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightDir0);

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);                  	
	glClearDepth(1.0f); 
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
}

void mouseFunc(int x, int y) {
	if(x < prevMousePositionX) {
		rotation+=2;
		if(rotation > 360) rotation-=360;
	}else if(x > prevMousePositionX) {
		rotation-=2;
		if(rotation < 0) rotation+=360;
	}
	prevMousePositionX = x;
}