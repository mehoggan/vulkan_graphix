#include "Tank.h"
#include "TankB.h"
#include "macro_crtdbg.h"
#include "VBOShaderLibrary.h"


const char* TANK_B_NAME = "Hellfire";
const int TANK_B_HP = 1000;
const int TANK_B_POWER = 7;
const int TANK_B_ARMOR = 5;
const int TANK_B_SPEED = 50;

TankB::TankB(){}
TankB::TankB(GLfloat x, GLfloat y, GLfloat z){
	this->initBody();
	this->initHead();
	this->initTurret();
	this->initWheel();
	this->initDuration();
	
	this->turretOffset[0]=0;
	this->turretOffset[1]=0.0001;
	this->turretOffset[2]=-50.0001;
	this->headOffset[0]=0;
	this->headOffset[1]=70;
	this->headOffset[2]=0;
	this->bodyOffset[0]=0;
	this->bodyOffset[1]=65;
	this->bodyOffset[2]=0;

	for(int i=0 ; i<3 ; i++){
		this->bodyScale[i] = 50;
		this->headScale[i] = 50;
		this->turretScale[i] = 50;
		this->wheelScale[i] = 50;
	}
	
	this->power = TANK_B_POWER;
	this->armor = TANK_B_ARMOR;
	this->speed = TANK_B_SPEED;
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


	vbo_shader_turret->loadClientData("./Hellfire/Hellfire_Turret.ogl");
	vbo_shader_body->loadClientData("./Hellfire/Hellfire_Body.ogl");
	vbo_shader_head->loadClientData("./Hellfire/Hellfire_Head.ogl");

	vbo_shader_turret->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_body->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_head->loadShaders("VertexTank.vs","FragmentTank.vs");

	vbo_shader_turret->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_body->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_head->LoadTexture("TestImage.raw",1024,1024);

	projectileLandPos[0]=9999999;
	projectileLandPos[1]=9999999;
}
TankB::~TankB(){
	delete this->vbo_shader_head;
	delete this->vbo_shader_body;
	delete this->vbo_shader_turret;
}


//GETTERS
int TankB::getBaseHP()					{return TANK_B_HP;}
int TankB::getBasePower()				{return TANK_B_POWER;}
int TankB::getBaseArmor()				{return TANK_B_ARMOR;}
int TankB::getBaseSpeed()				{return TANK_B_SPEED;}
const char* TankB::getName()			{return TANK_B_NAME;}



