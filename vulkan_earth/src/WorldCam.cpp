#include "WorldCam.h"
#include "macro_crtdbg.h"

WorldCam::WorldCam(){}
WorldCam::WorldCam(GLfloat x,GLfloat y,GLfloat z){
	this->matrix[0]=0;
	this->matrix[1]=0;
	this->matrix[2]=1;
	this->matrix[3]=0;
	this->matrix[4]=1;
	this->matrix[5]=0;
	this->matrix[6]=0;
	this->matrix[7]=0;
	this->matrix[8]=0;
	this->matrix[9]=-1;
	this->matrix[10]=0;
	this->matrix[11]=0;
	this->matrix[12]=x;
	this->matrix[13]=y;
	this->matrix[14]=z;
	this->matrix[15]=0;
	glPushMatrix();
	glLoadMatrixf(this->matrix);
	glRotatef(-10,this->matrix[4],this->matrix[5],this->matrix[6]);
	glGetFloatv(GL_MODELVIEW_MATRIX,this->matrix);
	glPopMatrix();
	this->shakeCamPos[0]=0;
	this->shakeCamPos[1]=0;
	this->shakeCamPos[2]=0;
}
WorldCam::~WorldCam(){

}

void WorldCam::view(){
	gluLookAt(	this->matrix[12]+shakeCamPos[0],
				this->matrix[13]+shakeCamPos[1],
				this->matrix[14]+shakeCamPos[2],
				this->matrix[12]+this->matrix[8]+shakeCamPos[0],
				this->matrix[13]+this->matrix[9]+shakeCamPos[1],
				this->matrix[14]+this->matrix[10]+shakeCamPos[2],
				this->matrix[4],
				this->matrix[5],
				this->matrix[6]);
	updateShakeCam();
}

void WorldCam::moveCam(float x, float y, float z){
	this->matrix[12]+=x;
	this->matrix[13]+=y;
	this->matrix[14]+=z;
}

GLfloat* WorldCam::getMatrix(){
	return matrix;
}

void WorldCam::setShakeCam(int magnitude){
	this->shakeCamPos[0] = magnitude;
	this->shakeCamPos[1] = magnitude;
	this->shakeCamPos[2] = magnitude;
}

void WorldCam::updateShakeCam(){
	if(this->shakeCamPos[0] != 0){
		this->shakeCamPos[0] = (int) this->shakeCamPos[0] / 1.015281239159713;
		if(this->shakeCamPos[0]%3 == 0){
			this->shakeCamPos[0] *= -1;
		}
	}
	if(this->shakeCamPos[1] != 0){
		this->shakeCamPos[1] = (int) this->shakeCamPos[1] / 1.015281239159713;
		if(this->shakeCamPos[1]%3 == 1){
			this->shakeCamPos[1] *= -1;
		}
	}
	if(this->shakeCamPos[2] != 0){
		this->shakeCamPos[2] = (int) this->shakeCamPos[2] / 1.015281239159713;
		if(this->shakeCamPos[2]%3 == 2){
			this->shakeCamPos[2] *= -1;
		}
	}
}