#include "Tank.h"
#include "TankC.h"
#include "macro_crtdbg.h"
#include "VBOShaderLibrary.h"


const char* TANK_C_NAME = "HeavyD";
const int TANK_C_HP = 1000;
const int TANK_C_POWER = 10;
const int TANK_C_ARMOR = 7;
const int TANK_C_SPEED = 5;

TankC::TankC(){}
TankC::TankC(GLfloat x, GLfloat y, GLfloat z){
	this->initBody();
	this->initHead();
	this->initTurret();
	this->initWheel();
	this->initDuration();

	this->turretOffset[0]=0;
	this->turretOffset[1]=70;
	this->turretOffset[2]=100.0001;
	this->headOffset[0]=0;
	this->headOffset[1]=0;
	this->headOffset[2]=0;
	this->bodyOffset[0]=0;
	this->bodyOffset[1]=90;
	this->bodyOffset[2]=0;

	for(int i=0 ; i<3 ; i++){
		this->bodyScale[i] = 80;
		this->headScale[i] = 80;
		this->turretScale[i] = 80;
		this->wheelScale[i] = 80;
	}

	this->power = TANK_C_POWER;
	this->armor = TANK_C_ARMOR;
	this->speed = TANK_C_SPEED;
	this->currentPower=10;
	this->previousPower=1000;
	this->previousAngle=1;
	this->HP = this->armor*100;

	this->vbo_shader_head=new VBOShaderLibrary();
	this->vbo_shader_body=new VBOShaderLibrary();
	this->vbo_shader_turret=new VBOShaderLibrary();
	/*	CODE NEEDED TO USE SHADERS AND VBOS	*/
	if(!(VBOShaderLibrary::InitGlew())) {
		exit(1);
	}else if(!(VBOShaderLibrary::AreVBOsSupported())){
		exit(1);
	}

	vbo_shader_turret->getVBOPointerFunctions();
	vbo_shader_body->getVBOPointerFunctions();
	vbo_shader_head->getVBOPointerFunctions();

	vbo_shader_turret->loadClientData("./HeavyD/HeavyD_Turret.ogl");
	vbo_shader_body->loadClientData("./HeavyD/HeavyD_Body.ogl");
	vbo_shader_head->loadClientData("./HeavyD/HeavyD_Head.ogl");

	vbo_shader_turret->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_body->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_head->loadShaders("VertexTank.vs","FragmentTank.vs");

	vbo_shader_turret->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_body->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_head->LoadTexture("TestImage.raw",1024,1024);

	projectileLandPos[0]=9999999;
	projectileLandPos[1]=9999999;
}
TankC::~TankC(){
	delete this->vbo_shader_head;
	delete this->vbo_shader_body;
	delete this->vbo_shader_turret;
}


//GETTERS
int TankC::getBaseHP()					{return TANK_C_HP;}
int TankC::getBasePower()				{return TANK_C_POWER;}
int TankC::getBaseArmor()				{return TANK_C_ARMOR;}
int TankC::getBaseSpeed()				{return TANK_C_SPEED;}
const char* TankC::getName()			{return TANK_C_NAME;}



void TankC::drawTankHitBox(){
	/*Vertex tankPos(headMatrix[12],headMatrix[13],headMatrix[14]);
	Vector right(-headMatrix[0],-headMatrix[1],-headMatrix[2]);
	Vector up(headMatrix[4],headMatrix[5],headMatrix[6]);
	Vector at(-headMatrix[8],-headMatrix[9],-headMatrix[10]);

	Vector left(headMatrix[0],headMatrix[1],headMatrix[2]);
	Vector down(-headMatrix[4],-headMatrix[5],-headMatrix[6]);
	Vector back(headMatrix[8],headMatrix[9],headMatrix[10]);*/
	glBegin(GL_QUADS);
		//top
		glColor4f(1.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,1.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,0.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(1.0,1.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ);
	glEnd();

	glBegin(GL_QUADS);
		//right
		glColor4f(1.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,1.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*down.compoZ);
		glColor4f(0.0,0.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*down.compoZ);
		glColor4f(1.0,1.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ);
	glEnd();

	glBegin(GL_QUADS);
		//left
		glColor4f(1.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,1.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*down.compoZ);
		glColor4f(0.0,0.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*down.compoZ);
		glColor4f(1.0,1.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ);
	glEnd();

	glBegin(GL_QUADS);
		//front
		glColor4f(1.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,1.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,0.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*down.compoZ);
				glColor4f(0.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*down.compoZ);
	glEnd();

	glBegin(GL_QUADS);
		//back
		glColor4f(1.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,1.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ);
		glColor4f(0.0,0.0,1.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*down.compoZ);
				glColor4f(0.0,0.0,0.0,.75);
		glVertex3f(	tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*down.compoX,
					tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*down.compoY,
					tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*down.compoZ);
	glEnd();
}

void TankC::updateHitBox() {
	this->tankPos.coordX = headMatrix[12];
	this->tankPos.coordY = headMatrix[13];
	this->tankPos.coordZ = headMatrix[14];
	this->right.compoX = -headMatrix[0];
	this->right.compoY = -headMatrix[1];
	this->right.compoZ = -headMatrix[2];
	this->left.compoX = headMatrix[0];
	this->left.compoY = headMatrix[1];
	this->left.compoZ = headMatrix[2];
	this->up.compoX = headMatrix[4];
	this->up.compoY = headMatrix[5];
	this->up.compoZ = headMatrix[6];
	this->down.compoX = -headMatrix[4];
	this->down.compoY = -headMatrix[5];
	this->down.compoZ = -headMatrix[6];
	this->at.compoX = -headMatrix[8];
	this->at.compoY = -headMatrix[9];
	this->at.compoZ = -headMatrix[10];
	this->back.compoX = headMatrix[8];
	this->back.compoY = headMatrix[9];
	this->back.compoZ = headMatrix[10];
}