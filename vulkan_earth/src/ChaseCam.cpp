#include "ChaseCam.h"
#include "math.h"
#include "macro_crtdbg.h"

ChaseCam::ChaseCam(){}
ChaseCam::ChaseCam(GLfloat* targetPos,GLfloat* targetAt){
	this->targetPos=targetPos;
	this->targetAt=targetAt;
	this->shakeCamPos[0]=0;
	this->shakeCamPos[1]=0;
	this->shakeCamPos[2]=0;
	this->backFactor=1;
	this->upFactor=1;
}
ChaseCam::~ChaseCam(){

}

void ChaseCam::view(){
	GLfloat mag=sqrt(this->targetAt[0]*this->targetAt[0]+this->targetAt[1]*this->targetAt[1]+this->targetAt[2]*this->targetAt[2]);
	gluLookAt	(	
					targetPos[0]-500*this->targetAt[0]/mag*(this->backFactor) + this->shakeCamPos[0],
					targetPos[1]+this->upFactor + this->shakeCamPos[1],
					targetPos[2]-500*this->targetAt[2]/mag + this->shakeCamPos[2],
					targetPos[0]+200*this->targetAt[0]/mag + this->shakeCamPos[0],
					targetPos[1]+200*this->targetAt[0]/mag + this->shakeCamPos[1],
					targetPos[2]+200*this->targetAt[2]/mag + this->shakeCamPos[2],
					0,
					1,
					0
				);
	updateShakeCam();
}

void ChaseCam::updateFactor() {
	this->backFactor=0;
	this->upFactor=4000;
}

void ChaseCam::resetFactor() {
	this->backFactor=1;
	this->upFactor=100;
}

void ChaseCam::setShakeCam(int magnitude){
	this->shakeCamPos[0] = magnitude;
	this->shakeCamPos[1] = magnitude;
	this->shakeCamPos[2] = magnitude;
}

void ChaseCam::updateShakeCam(){
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



