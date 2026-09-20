#include "TankF.h"
#include "Tank.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"

const char* tank_f_name = "Behemoth";
const int tank_f_hp = 1000;
const int tank_f_power = 8;
const int tank_f_armor = 8;
const int tank_f_speed = 20;

TankF::TankF() = default;
TankF::TankF(GLfloat x, GLfloat y, GLfloat z) {
    this->initBody();
    this->initHead();
    this->initTurret();
    this->initWheel();
    this->initDuration();

    this->turretOffset[0] = 0;
    this->turretOffset[1] = 20;
    this->turretOffset[2] = -20.0001;
    this->headOffset[0] = 0;
    this->headOffset[1] = 80;
    this->headOffset[2] = 100;
    this->bodyOffset[0] = 0;
    this->bodyOffset[1] = 80;
    this->bodyOffset[2] = 0;

    for (int i = 0; i < 3; i++) {
        this->bodyScale[i] = 60;
        this->headScale[i] = 60;
        this->turretScale[i] = 60;
        this->wheelScale[i] = 60;
    }

    this->power = tank_f_power;
    this->armor = tank_f_armor;
    this->speed = tank_f_speed;
    this->currentPower = 10;
    this->previousPower = 1000;
    this->previousAngle = 1;
    this->HP = this->armor * 100;

    this->vbo_shader_head = new VBOShaderLibrary();
    this->vbo_shader_body = new VBOShaderLibrary();
    this->vbo_shader_turret = new VBOShaderLibrary();
    // this->vbo_shader_wheel=new VBOShaderLibrary();
    /*	CODE NEEDED TO USE SHADERS AND VBOS	*/
    if (!(VBOShaderLibrary::InitGlew())) {
        exit(1);
    } else if (!(VBOShaderLibrary::AreVBOsSupported())) {
        exit(1);
    }

    vbo_shader_turret->getVBOPointerFunctions();
    vbo_shader_body->getVBOPointerFunctions();
    vbo_shader_head->getVBOPointerFunctions();
    // vbo_shader_wheel->getVBOPointerFunctions();

    vbo_shader_turret->loadClientData("./Behemoth/Behemoth_Turret.ogl");
    vbo_shader_body->loadClientData("./Behemoth/Behemoth_Body.ogl");
    vbo_shader_head->loadClientData("./Behemoth/Behemoth_Head.ogl");
    // vbo_shader_wheel->loadClientData("./Behemoth/Behemoth_Wheel.ogl");

    vbo_shader_turret->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_body->loadShaders("VertexTank.vs", "FragmentTank.vs");
    vbo_shader_head->loadShaders("VertexTank.vs", "FragmentTank.vs");
    // vbo_shader_wheel->loadShaders("VertexTank.vs","FragmentTank.vs");

    vbo_shader_turret->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_body->LoadTexture("TestImage.raw", 1024, 1024);
    vbo_shader_head->LoadTexture("TestImage.raw", 1024, 1024);
    // vbo_shader_wheel->LoadTexture("TestImage.raw",1024,1024);

    projectileLandPos[0] = 9999999;
    projectileLandPos[1] = 9999999;
}
TankF::~TankF() {
    delete this->vbo_shader_head;
    delete this->vbo_shader_body;
    delete this->vbo_shader_turret;
    // delete this->vbo_shader_wheel;
}

// GETTERS
int TankF::getBaseHP() { return tank_f_hp; }
int TankF::getBasePower() { return tank_f_power; }
int TankF::getBaseArmor() { return tank_f_armor; }
int TankF::getBaseSpeed() { return tank_f_speed; }
std::string TankF::getName() { return tank_f_name; }
