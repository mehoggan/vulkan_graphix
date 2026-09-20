#include "Tank.h"
#include "TankH.h"
#include "macro_crtdbg.h"
#include "VBOShaderLibrary.h"

const char* TANK_H_NAME = "Predator";
const int TANK_H_HP = 1000;
const int TANK_H_POWER = 10;
const int TANK_H_ARMOR = 10;
const int TANK_H_SPEED = 100;

TankH::TankH(){}
TankH::TankH(GLfloat x, GLfloat y, GLfloat z){
	this->initBody();
	this->initHead();
	this->initTurret();
	this->initWheel();
	this->initDuration();

	this->turretOffset[0]=0;
	this->turretOffset[1]=0;
	this->turretOffset[2]=-35;
	this->headOffset[0]=0;
	this->headOffset[1]=65;
	this->headOffset[2]=10;
	this->bodyOffset[0]=0;
	this->bodyOffset[1]=80;
	this->bodyOffset[2]=0;

	for(int i=0 ; i<3 ; i++){
		this->bodyScale[i] = 70;
		this->headScale[i] = 80;
		this->turretScale[i] = 70;
	}

	this->power = TANK_H_POWER;
	this->armor = TANK_H_ARMOR;
	this->speed = TANK_H_SPEED;
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

	vbo_shader_turret->loadClientData("./Predator/Predator_Turret.ogl");
	vbo_shader_body->loadClientData("./Predator/Predator_Body.ogl");
	vbo_shader_head->loadClientData("./Predator/Predator_Head.ogl");

	vbo_shader_turret->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_body->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_head->loadShaders("VertexTank.vs","FragmentTank.vs");

	vbo_shader_turret->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_body->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_head->LoadTexture("TestImage.raw",1024,1024);

	projectileLandPos[0]=9999999;
	projectileLandPos[1]=9999999;
}
TankH::~TankH(){
	delete this->vbo_shader_head;
	delete this->vbo_shader_body;
	delete this->vbo_shader_turret;
}


//GETTERS
int TankH::getBaseHP()					{return TANK_H_HP;}
int TankH::getBasePower()				{return TANK_H_POWER;}
int TankH::getBaseArmor()				{return TANK_H_ARMOR;}
int TankH::getBaseSpeed()				{return TANK_H_SPEED;}
const char* TankH::getName()			{return TANK_H_NAME;}

