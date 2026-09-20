#include "Tank.h"
#include "TankF.h"
#include "macro_crtdbg.h"
#include "VBOShaderLibrary.h"

const char* TANK_F_NAME = "Behemoth";
const int TANK_F_HP = 1000;
const int TANK_F_POWER = 8;
const int TANK_F_ARMOR = 8;
const int TANK_F_SPEED = 20;

TankF::TankF(){}
TankF::TankF(GLfloat x, GLfloat y, GLfloat z){
	this->initBody();
	this->initHead();
	this->initTurret();
	this->initWheel();
	this->initDuration();

	this->turretOffset[0]=0;
	this->turretOffset[1]=20;
	this->turretOffset[2]=-20.0001;
	this->headOffset[0]=0;
	this->headOffset[1]=80;
	this->headOffset[2]=100;
	this->bodyOffset[0]=0;
	this->bodyOffset[1]=80;
	this->bodyOffset[2]=0;

	for(int i=0 ; i<3 ; i++){
		this->bodyScale[i] = 60;
		this->headScale[i] = 60;
		this->turretScale[i] = 60;
		this->wheelScale[i] = 60;
	}

	this->power = TANK_F_POWER;
	this->armor = TANK_F_ARMOR;
	this->speed = TANK_F_SPEED;
	this->currentPower=10;
	this->previousPower=1000;
	this->previousAngle=1;
	this->HP = this->armor*100;

	this->vbo_shader_head=new VBOShaderLibrary();
	this->vbo_shader_body=new VBOShaderLibrary();
	this->vbo_shader_turret=new VBOShaderLibrary();
	//this->vbo_shader_wheel=new VBOShaderLibrary();
	/*	CODE NEEDED TO USE SHADERS AND VBOS	*/
	if(!(VBOShaderLibrary::InitGlew())) {
		exit(1);
	}else if(!(VBOShaderLibrary::AreVBOsSupported())){
		exit(1);
	}

	vbo_shader_turret->getVBOPointerFunctions();
	vbo_shader_body->getVBOPointerFunctions();
	vbo_shader_head->getVBOPointerFunctions();
	//vbo_shader_wheel->getVBOPointerFunctions();

	vbo_shader_turret->loadClientData("./Behemoth/Behemoth_Turret.ogl");
	vbo_shader_body->loadClientData("./Behemoth/Behemoth_Body.ogl");
	vbo_shader_head->loadClientData("./Behemoth/Behemoth_Head.ogl");
	//vbo_shader_wheel->loadClientData("./Behemoth/Behemoth_Wheel.ogl");

	vbo_shader_turret->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_body->loadShaders("VertexTank.vs","FragmentTank.vs");
	vbo_shader_head->loadShaders("VertexTank.vs","FragmentTank.vs");
	//vbo_shader_wheel->loadShaders("VertexTank.vs","FragmentTank.vs");

	vbo_shader_turret->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_body->LoadTexture("TestImage.raw",1024,1024);
	vbo_shader_head->LoadTexture("TestImage.raw",1024,1024);
	//vbo_shader_wheel->LoadTexture("TestImage.raw",1024,1024);

	projectileLandPos[0]=9999999;
	projectileLandPos[1]=9999999;
}
TankF::~TankF(){
	delete this->vbo_shader_head;
	delete this->vbo_shader_body;
	delete this->vbo_shader_turret;
	//delete this->vbo_shader_wheel;
}


//GETTERS
int TankF::getBaseHP()					{return TANK_F_HP;}
int TankF::getBasePower()				{return TANK_F_POWER;}
int TankF::getBaseArmor()				{return TANK_F_ARMOR;}
int TankF::getBaseSpeed()				{return TANK_F_SPEED;}
const char* TankF::getName()			{return TANK_F_NAME;}


