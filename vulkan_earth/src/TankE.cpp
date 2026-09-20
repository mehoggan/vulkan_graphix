#include "Tank.h"
#include "TankE.h"
#include "macro_crtdbg.h"
#include "VBOShaderLibrary.h"

const char* TANK_E_NAME = "Eggroid";
const int TANK_E_HP = 1000;
const int TANK_E_POWER = 4;
const int TANK_E_ARMOR = 3;
const int TANK_E_SPEED = 80;

TankE::TankE(){}
TankE::TankE(GLfloat x, GLfloat y, GLfloat z){
	this->initBody();
	this->initHead();
	this->initTurret();
	this->initWheel();
	this->initDuration();

	this->turretOffset[0]=0;
	this->turretOffset[1]=0;
	this->turretOffset[2]=0;
	this->headOffset[0]=0;
	this->headOffset[1]=100;
	this->headOffset[2]=0;
	this->bodyOffset[0]=0;
	this->bodyOffset[1]=80;
	this->bodyOffset[2]=0;

	this->hitBoxHeight=200;
	this->hitBoxLength=300;
	this->hitBoxWidth=300;

	for(int i=0 ; i<3 ; i++){
		this->bodyScale[i] = 40;
		this->headScale[i] = 40;
		this->turretScale[i] = 40;
		this->wheelScale[i] = 40;
	}

	this->power = TANK_E_POWER;
	this->armor = TANK_E_ARMOR;
	this->speed = TANK_E_SPEED;
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

	vbo_shader_turret->loadClientData("./Eggroid/Eggroid_Turret.ogl");
	vbo_shader_body->loadClientData("./Eggroid/Eggroid_Body.ogl");
	vbo_shader_head->loadClientData("./Eggroid/Eggroid_Head.ogl");
	//vbo_shader_wheel->loadClientData("./Eggroid/Eggroid_Wheel.ogl");

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
TankE::~TankE(){
	delete this->vbo_shader_head;
	delete this->vbo_shader_body;
	delete this->vbo_shader_turret;
	//delete this->vbo_shader_wheel;
}


//GETTERS
int TankE::getBaseHP()					{return TANK_E_HP;}
int TankE::getBasePower()				{return TANK_E_POWER;}
int TankE::getBaseArmor()				{return TANK_E_ARMOR;}
int TankE::getBaseSpeed()				{return TANK_E_SPEED;}
const char* TankE::getName()			{return TANK_E_NAME;}


