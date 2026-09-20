#include "Tank.h"
#include "TankD.h"
#include "macro_crtdbg.h"
#include "VBOShaderLibrary.h"


const char* TANK_D_NAME = "Panzer";
const int TANK_D_HP = 1000;
const int TANK_D_POWER = 9;
const int TANK_D_ARMOR = 5;
const int TANK_D_SPEED = 30;

TankD::TankD(){}
TankD::TankD(GLfloat x, GLfloat y, GLfloat z){
	this->initBody();
	this->initHead();
	this->initTurret();
	this->initWheel();
	this->initDuration();

	this->turretOffset[0]=0;
	this->turretOffset[1]=10;
	this->turretOffset[2]=0.0001;
	this->headOffset[0]=0;
	this->headOffset[1]=70;
	this->headOffset[2]=40;
	this->bodyOffset[0]=0;
	this->bodyOffset[1]=45;
	this->bodyOffset[2]=0;

	for(int i=0 ; i<3 ; i++){
		this->bodyScale[i] = 50;
		this->headScale[i] = 50;
		this->turretScale[i] = 50;
		this->wheelScale[i] = 50;
	}

	this->power = TANK_D_POWER;
	this->armor = TANK_D_ARMOR;
	this->speed = TANK_D_SPEED;
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

	vbo_shader_turret->loadClientData("./Panzer/Panzer_Turret.ogl");
	vbo_shader_body->loadClientData("./Panzer/Panzer_Body.ogl");
	vbo_shader_head->loadClientData("./Panzer/Panzer_Head.ogl");

	vbo_shader_turret->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_body->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_head->loadShaders("VertexTank.vs","FragmentTank.vs");

	vbo_shader_turret->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_body->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_head->LoadTexture("TestImage.raw",1024,1024);

	projectileLandPos[0]=9999999;
	projectileLandPos[1]=9999999;
}
TankD::~TankD(){
	delete this->vbo_shader_head;
	delete this->vbo_shader_body;
	delete this->vbo_shader_turret;
}


//GETTERS
int TankD::getBaseHP()					{return TANK_D_HP;}
int TankD::getBasePower()				{return TANK_D_POWER;}
int TankD::getBaseArmor()				{return TANK_D_ARMOR;}
int TankD::getBaseSpeed()				{return TANK_D_SPEED;}
const char* TankD::getName()			{return TANK_D_NAME;}



