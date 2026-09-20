#include <iostream>
#include "Tank.h"
#include "Normal.h"
#include "Vector.h"
#include "VBOShaderLibrary.h"
#include "macro_crtdbg.h"
#include "ParticleGenerator.h"

using namespace std;

Tank::Tank(){
	this->hitBoxHeight=200;
	this->hitBoxLength=400;
	this->hitBoxWidth=300;
	Vector right(0,0,0);
	Vector up(0,0,0);
	Vector at(0,0,0);
	Vector left(0,0,0);
	Vector down(0,0,0);
	Vector back(0,0,0);
	this->tankAlive=true;
	this->smokeGen=NULL;
	this->acidGen=NULL;
	this->floatGen=NULL;
}
Tank::~Tank(){
	if(this->smokeGen){
		this->smokeGen->killGenerator();
		delete this->smokeGen;
	}
	if(this->acidGen){
		this->acidGen->killGenerator();
		delete this->acidGen;
	}
	if(this->floatGen){
		this->floatGen->killGenerator();
		delete this->floatGen;
	}
}

void Tank::printTurretMatrix() {
	cout << " Turret Matrix " << endl;
	cout<<"|"<<this->turretMatrix[0]<<" "<<this->turretMatrix[4]<<" "<<this->turretMatrix[8]<<" "<<this->turretMatrix[12]<<"|"<<endl;
	cout<<"|"<<this->turretMatrix[1]<<" "<<this->turretMatrix[5]<<" "<<this->turretMatrix[9]<<" "<<this->turretMatrix[13]<<"|"<<endl;
	cout<<"|"<<this->turretMatrix[2]<<" "<<this->turretMatrix[6]<<" "<<this->turretMatrix[10]<<" "<<this->turretMatrix[14]<<"|"<<endl;
	cout<<"|"<<this->turretMatrix[3]<<" "<<this->turretMatrix[7]<<" "<<this->turretMatrix[11]<<" "<<this->turretMatrix[15]<<"|"<<endl;
}

void Tank::printHeadMatrix() {
	cout << " Head Matrix " << endl;
	cout<<"|"<<this->headMatrix[0]<<" "<<this->headMatrix[4]<<" "<<this->headMatrix[8]<<" "<<this->headMatrix[12]<<"|"<<endl;
	cout<<"|"<<this->headMatrix[1]<<" "<<this->headMatrix[5]<<" "<<this->headMatrix[9]<<" "<<this->headMatrix[13]<<"|"<<endl;
	cout<<"|"<<this->headMatrix[2]<<" "<<this->headMatrix[6]<<" "<<this->headMatrix[10]<<" "<<this->headMatrix[14]<<"|"<<endl;
	cout<<"|"<<this->headMatrix[3]<<" "<<this->headMatrix[7]<<" "<<this->headMatrix[11]<<" "<<this->headMatrix[15]<<"|"<<endl;
}

void Tank::printBodyMatrix() {
	cout << " Body Matrix " << endl;
	cout<<"|"<<this->bodyMatrix[0]<<" "<<this->bodyMatrix[4]<<" "<<this->bodyMatrix[8]<<" "<<this->bodyMatrix[12]<<"|"<<endl;
	cout<<"|"<<this->bodyMatrix[1]<<" "<<this->bodyMatrix[5]<<" "<<this->bodyMatrix[9]<<" "<<this->bodyMatrix[13]<<"|"<<endl;
	cout<<"|"<<this->bodyMatrix[2]<<" "<<this->bodyMatrix[6]<<" "<<this->bodyMatrix[10]<<" "<<this->bodyMatrix[14]<<"|"<<endl;
	cout<<"|"<<this->bodyMatrix[3]<<" "<<this->bodyMatrix[7]<<" "<<this->bodyMatrix[11]<<" "<<this->bodyMatrix[15]<<"|"<<endl;
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
}

void Tank::setTankPos(GLfloat x, GLfloat y, GLfloat z){
	//Keep Track of Tanks Position for Falling Damage
	this->previousHeight = this->currentHeight;
	this->currentHeight = y;
	
	this->bodyMatrix[12] = x+this->bodyOffset[0];
	this->bodyMatrix[13] = y+this->bodyOffset[1];
	this->bodyMatrix[14] = z+this->bodyOffset[2];
	this->headMatrix[12] = this->bodyMatrix[12]+this->headOffset[0]*this->bodyMatrix[0]+
												this->headOffset[1]*this->bodyMatrix[4]+
												this->headOffset[2]*this->bodyMatrix[8];
	this->headMatrix[13] = this->bodyMatrix[13]+this->headOffset[0]*this->bodyMatrix[1]+
												this->headOffset[1]*this->bodyMatrix[5]+
												this->headOffset[2]*this->bodyMatrix[9];
	this->headMatrix[14] = this->bodyMatrix[14]+this->headOffset[0]*this->bodyMatrix[2]+
												this->headOffset[1]*this->bodyMatrix[6]+
												this->headOffset[2]*this->bodyMatrix[10];

	this->turretMatrix[12] = this->headMatrix[12]+this->turretOffset[0]*this->headMatrix[0]+
												this->turretOffset[1]*this->headMatrix[4]+
												this->turretOffset[2]*this->headMatrix[8];
	this->turretMatrix[13] = this->headMatrix[13]+this->turretOffset[0]*this->headMatrix[1]+
												this->turretOffset[1]*this->headMatrix[5]+
												this->turretOffset[2]*this->headMatrix[9];
	this->turretMatrix[14] = this->headMatrix[14]+this->turretOffset[0]*this->headMatrix[2]+
												this->turretOffset[1]*this->headMatrix[6]+
												this->turretOffset[2]*this->headMatrix[10];
	this->updateHitBox();
	if(this->smokeGen) this->smokeGen->update(	this->bodyMatrix[12]+this->bodyOffset[0]+this->headOffset[0],
										this->bodyMatrix[13]+this->bodyOffset[1]+this->headOffset[1],
										this->bodyMatrix[14]+this->bodyOffset[2]+this->headOffset[2]);
}

void Tank::resetTurret() {
	this->turretMatrix[12] = this->headMatrix[12]+this->turretOffset[0]*this->headMatrix[0]+
												this->turretOffset[1]*this->headMatrix[4]+
												this->turretOffset[2]*this->headMatrix[8];
	this->turretMatrix[13] = this->headMatrix[13]+this->turretOffset[0]*this->headMatrix[1]+
												this->turretOffset[1]*this->headMatrix[5]+
												this->turretOffset[2]*this->headMatrix[9];
	this->turretMatrix[14] = this->headMatrix[14]+this->turretOffset[0]*this->headMatrix[2]+
												this->turretOffset[1]*this->headMatrix[6]+
												this->turretOffset[2]*this->headMatrix[10];
}

void Tank::NormalizeVector(Vector* v) {
	GLfloat mag = sqrt(v->compoX*v->compoX + v->compoY*v->compoY + v->compoZ*v->compoZ);
	if(mag != 0) {
		v->compoX/=mag;
		v->compoY/=mag;
		v->compoZ/=mag;
	}
}

GLfloat Tank::calcAngleBetweenVectors(Vector one, Vector two) {
	this->NormalizeVector(&one);
	this->NormalizeVector(&two);
	errno = 0;
	GLfloat TT = 3.141592653f;
	GLfloat u[3] = {one.compoX, one.compoY, one.compoZ};
	GLfloat v[3] = {two.compoX, two.compoY, two.compoZ};
	GLfloat angle = acos(u[0]*v[0] + u[1]*v[1] + u[2]*v[2]) * (180.0/TT);
	if(errno) {
		return .01;
	}
	return angle;
}

void Tank::orientTank(Normal* n) {
	this->rotate_degrees = 0;
	this->alignmentVector.compoX = n->compoX;
	this->alignmentVector.compoY = n->compoY;
	this->alignmentVector.compoZ = n->compoZ;
	Vector tanks_up(this->bodyMatrix[4],this->bodyMatrix[5],this->bodyMatrix[6]);
	Vector perp(this->alignmentVector.compoX, this->alignmentVector.compoY, this->alignmentVector.compoZ);
	GLfloat angle = calcAngleBetweenVectors(perp,tanks_up);
	
	GLfloat u[3] = {perp.compoX, perp.compoY, perp.compoZ};
	GLfloat v[3] = {tanks_up.compoX, tanks_up.compoY, tanks_up.compoZ}; 

	this->rotate_about.compoX = u[1]*v[2] - v[1]*u[2]; 
	this->rotate_about.compoY =	u[2]*v[0] - u[0]*v[2]; 
	this->rotate_about.compoZ = u[0]*v[1] - v[0]*u[1];

	GLfloat mag = (GLfloat)sqrt(	pow((double)(this->rotate_about.compoX),2.0) + 
									pow((double)(this->rotate_about.compoY),2.0) + 
									pow((double)(this->rotate_about.compoZ),2.0));
	if(mag != 0) {
		this->rotate_about.compoX /= mag;
		this->rotate_about.compoY /= mag;
		this->rotate_about.compoZ /= mag;
		glPushMatrix();
			glLoadMatrixf(this->bodyMatrix);
			glRotatef(angle,this->rotate_about.compoZ,this->rotate_about.compoY,this->rotate_about.compoX);
			glGetFloatv(GL_MODELVIEW_MATRIX,this->bodyMatrix);
			glGetFloatv(GL_MODELVIEW_MATRIX,this->headMatrix);
			glGetFloatv(GL_MODELVIEW_MATRIX,this->turretMatrix);
			glGetFloatv(GL_MODELVIEW_MATRIX,this->wheelMatrix);
		glPopMatrix();
		this->turret_degrees = 0;
	}		
	
	this->updateHitBox();
}

void Tank::rotateHead(GLfloat degrees) {
		this->rotate_degrees+=degrees;
		if(this->rotate_degrees > 360) {
			this->rotate_degrees-=360;
		}else if(this->rotate_degrees < 0) {
			this->rotate_degrees+=360;
		}
		
		GLfloat angle=calcAngleBetweenVectors(	Vector(this->headMatrix[4],this->headMatrix[5],this->headMatrix[6]),
												Vector(this->turretMatrix[4],this->turretMatrix[5],this->turretMatrix[6]));

		glPushMatrix();
			glLoadMatrixf(this->headMatrix);
			glRotatef(degrees,0,1,0);
			glGetFloatv(GL_MODELVIEW_MATRIX,this->headMatrix);
		glPopMatrix();

		glPushMatrix();
			glLoadMatrixf(this->turretMatrix);
			glRotatef(-angle,1,0,0);
			glTranslatef(-this->turretOffset[0],-this->turretOffset[1],-this->turretOffset[2]);
			glRotatef(degrees,0,1,0);
			glTranslatef(this->turretOffset[0],this->turretOffset[1],this->turretOffset[2]);
			glRotatef(angle,1,0,0);
			/*cout << "63. ";
			this->printTurretMatrix();
			this->printHeadMatrix();
			this->printBodyMatrix();*/
			glGetFloatv(GL_MODELVIEW_MATRIX,this->turretMatrix);
			/*cout << "64. ";
			this->printTurretMatrix();
			this->printHeadMatrix();
			this->printBodyMatrix();*/
		glPopMatrix();
		this->updateHitBox();
}


void Tank::rotateTurret(GLfloat degrees) {
	if(!((this->turret_degrees+degrees > 90)||(this->turret_degrees+degrees < 0))) {
		this->turret_degrees+=degrees;
		glPushMatrix();
			glLoadMatrixf(this->turretMatrix);
			glRotatef(degrees,1,0,0);
			//cout << "65. ";
			//this->printTurretMatrix();
			//this->printHeadMatrix();
			//this->printBodyMatrix();
			glGetFloatv(GL_MODELVIEW_MATRIX,this->turretMatrix);
			//cout << "66. ";
			//this->printTurretMatrix();
			//this->printHeadMatrix();
			//this->printBodyMatrix();
		glPopMatrix();
	}
}

bool Tank::checkCollision(GLfloat X, GLfloat Y, GLfloat Z){
	//First check distance from tank, then check each face of hit box
	if	(
			sqrt(	pow((X-this->tankPos.coordX),2)+
					pow((Y-this->tankPos.coordY),2)+
					pow((Z-this->tankPos.coordZ),2)) < 50000000
		){
		//top
		GLfloat plane_x = tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX;
		GLfloat plane_y = tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY;
		GLfloat plane_z = tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ;
		GLfloat D = up.compoX*plane_x + up.compoY*plane_y + up.compoZ*plane_z;
		if(up.compoX*X + up.compoY*Y + up.compoZ*Z - D <= 0) { 
			//right
			plane_x = tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*right.compoX+this->hitBoxHeight/2.0*up.compoX;
			plane_y = tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*right.compoY+this->hitBoxHeight/2.0*up.compoY;
			plane_z = tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*right.compoZ+this->hitBoxHeight/2.0*up.compoZ;
			D = right.compoX*plane_x + right.compoY*plane_y + right.compoZ*plane_z;
			if(right.compoX*X + right.compoY*Y + right.compoZ*Z - D <= 0) {
				//left
				plane_x = tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX;
				plane_y = tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY;
				plane_z = tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ;
				D = left.compoX*plane_x + left.compoY*plane_y + left.compoZ*plane_z;
				if(left.compoX*X + left.compoY*Y + left.compoZ*Z - D <= 0) {
					//front
					plane_x = tankPos.coordX+this->hitBoxLength/2.0*at.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX;
					plane_y = tankPos.coordY+this->hitBoxLength/2.0*at.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY;
					plane_z = tankPos.coordZ+this->hitBoxLength/2.0*at.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ;
					D = at.compoX*plane_x + at.compoY*plane_y + at.compoZ*plane_z;
					if(at.compoX*X + at.compoY*Y + at.compoZ*Z - D <= 0) {
						//back
						plane_x = tankPos.coordX+this->hitBoxLength/2.0*back.compoX+this->hitBoxWidth/2.0*left.compoX+this->hitBoxHeight/2.0*up.compoX;
						plane_y = tankPos.coordY+this->hitBoxLength/2.0*back.compoY+this->hitBoxWidth/2.0*left.compoY+this->hitBoxHeight/2.0*up.compoY;
						plane_z = tankPos.coordZ+this->hitBoxLength/2.0*back.compoZ+this->hitBoxWidth/2.0*left.compoZ+this->hitBoxHeight/2.0*up.compoZ;
						D = back.compoX*plane_x + back.compoY*plane_y + back.compoZ*plane_z;
						if(back.compoX*X + back.compoY*Y + back.compoZ*Z - D <= 0) {
							cout << "COLLISION WITH TANK DETECTED" << endl;
							return true;
						}
					}
				}
			}			
		}
	}
	return false;
}

void Tank::dealDamage(int damage){
	if(damage<0) cout << "ERROR: Negative damage";
	this->HP-=damage;
	if(this->HP<=0) {
		this->HP=0;
		this->tankAlive=false;
		this->initDuration();
		this->smokeGen = new ParticleGenerator(10,5,1,100,0);
	}
}

void Tank::checkFallingDamage(){
	if(this->previousHeight!=this->currentHeight){
		this->dealDamage((this->previousHeight-this->currentHeight)/10);
	}
}

bool Tank::isAlive(){
	return this->tankAlive;
}

void Tank::tankRevive(){
	if(this->smokeGen){
		this->smokeGen=NULL;
	}
	this->tankAlive=true;
}

void Tank::adjustPower(GLfloat amount){
	if(((this->currentPower+amount)<=10.0)&&((this->currentPower+amount)>=0.0)){
		this->currentPower+=amount;
	}
}

void Tank::initDuration(){
	durationAcid=0;
	durationShield=0;
	durationEMP=0;
	durationFloat=0;
	durationDoubleAction=0;
	durationPadlock=0;
	durationCloak=0;
	durationParalyze=0;
	if(this->acidGen) delete acidGen;
	if(this->floatGen) delete floatGen;
}

void Tank::setBodyColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
	this->bodyColor[0] = r;
	this->bodyColor[1] = g;
	this->bodyColor[2] = b;
	this->bodyColor[3] = a;
}
void Tank::setBodyScale(float x, float y, float z){
	this->bodyScale[0] = x;
	this->bodyScale[1] = y;
	this->bodyScale[2] = z;
}

void Tank::setHeadColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
	this->headColor[0] = r;
	this->headColor[1] = g;
	this->headColor[2] = b;
	this->headColor[3] = a;
}
void Tank::setHeadScale(float x, float y, float z){
	this->headScale[0] = x;
	this->headScale[1] = y;
	this->headScale[2] = z;
}

void Tank::setTurretColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
	this->turretColor[0] = r;
	this->turretColor[1] = g;
	this->turretColor[2] = b;
	this->turretColor[3] = a;
}
void Tank::setTurretScale(float x, float y, float z){
	this->turretScale[0] = x;
	this->turretScale[1] = y;
	this->turretScale[2] = z;
}

void Tank::setWheelColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a){
	this->wheelColor[0] = r;
	this->wheelColor[1] = g;
	this->wheelColor[2] = b;
	this->wheelColor[3] = a;
}
void Tank::setWheelScale(float x, float y, float z){
	this->wheelScale[0] = x;
	this->wheelScale[1] = y;
	this->wheelScale[2] = z;
}
void Tank::setHP(int hp)		{this->HP = hp;}
void Tank::setPower(int p)		{this->power = p;}
void Tank::setArmor(int a)		{this->armor = a;}
void Tank::setSpeed(int d)		{this->speed = d;}

// Other functions
void Tank::fire(){

}

void Tank::updateHitBox() {
	/*Vertex tankPos(bodyMatrix[12],bodyMatrix[13],bodyMatrix[14]);
	Vector right(-bodyMatrix[0],-bodyMatrix[1],-bodyMatrix[2]);
	Vector up(bodyMatrix[4],bodyMatrix[5],bodyMatrix[6]);
	Vector at(-bodyMatrix[8],-bodyMatrix[9],-bodyMatrix[10]);

	Vector left(bodyMatrix[0],bodyMatrix[1],bodyMatrix[2]);
	Vector down(-bodyMatrix[4],-bodyMatrix[5],-bodyMatrix[6]);
	Vector back(bodyMatrix[8],bodyMatrix[9],bodyMatrix[10]);*/
	this->tankPos.coordX = bodyMatrix[12];
	this->tankPos.coordY = bodyMatrix[13];
	this->tankPos.coordZ = bodyMatrix[14];
	this->right.compoX = -bodyMatrix[0];
	this->right.compoY = -bodyMatrix[1];
	this->right.compoZ = -bodyMatrix[2];
	this->left.compoX = bodyMatrix[0];
	this->left.compoY = bodyMatrix[1];
	this->left.compoZ = bodyMatrix[2];
	this->up.compoX = bodyMatrix[4];
	this->up.compoY = bodyMatrix[5];
	this->up.compoZ = bodyMatrix[6];
	this->down.compoX = -bodyMatrix[4];
	this->down.compoY = -bodyMatrix[5];
	this->down.compoZ = -bodyMatrix[6];
	this->at.compoX = -bodyMatrix[8];
	this->at.compoY = -bodyMatrix[9];
	this->at.compoZ = -bodyMatrix[10];
	this->back.compoX = bodyMatrix[8];
	this->back.compoY = bodyMatrix[9];
	this->back.compoZ = bodyMatrix[10];
}

void Tank::keyHandler(){
}

//Virtuals
int Tank::getBaseHP()				{return 0;}
int Tank::getBasePower()			{return 0;}
int Tank::getBaseArmor()			{return 0;}
int Tank::getBaseSpeed()			{return 0;}
const char* Tank::getName()			{return "Huh?";}
Normal Tank::getAlignmentVector()	{return this->alignmentVector;}
Normal Tank::getRotateAbout()		{return this->rotate_about;}
GLfloat Tank::getTurretDegrees()	{return this->turret_degrees;}

void Tank::draw(){
	
	glPushMatrix();
		glMultMatrixf(this->turretMatrix);
		glScalef(this->turretScale[0], this->turretScale[1], this->turretScale[2]);
		//Draw tank turret
		this->vbo_shader_turret->drawClientData();
	glPopMatrix();
	glPushMatrix();
		glMultMatrixf(this->headMatrix);
		glScalef(this->headScale[0], this->headScale[1], this->headScale[2]);
		//Draw tank head
		this->vbo_shader_head->drawClientData();
	glPopMatrix();
	glPushMatrix();
		glMultMatrixf(this->bodyMatrix);
		//float effect must be same orientation as the tank
		if(this->floatGen){
			this->floatGen->update(0,-this->bodyOffset[1],0);
			this->floatGen->draw();
		}
		//Draw tank body
		glScalef(this->bodyScale[0], this->bodyScale[1], this->bodyScale[2]);
		this->vbo_shader_body->drawClientData();
	glPopMatrix();
	glPushMatrix();
		glMultMatrixf(this->bodyMatrix);
		//shield
		if(this->durationShield>0){
			glColor4f(0,0,1,0.2);
			glutSolidSphere(300,20,20);
		}
	glPopMatrix();
	
	

	//update and draw particles
	if(this->smokeGen){
		this->smokeGen->update(	this->bodyMatrix[12]+this->bodyOffset[0]+this->headOffset[0],
								this->bodyMatrix[13]+this->bodyOffset[1]+this->headOffset[1],
								this->bodyMatrix[14]+this->bodyOffset[2]+this->headOffset[2]);
		this->smokeGen->draw();
	}
	if(this->acidGen){
		this->acidGen->update(	this->bodyMatrix[12],
								this->bodyMatrix[13],
								this->bodyMatrix[14]);
		this->acidGen->draw();
	}

	
}

void Tank::drawTankHitBox(){
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


GLfloat Tank::getCurrentPower(){return this->currentPower;}
int Tank::getPreviousPower(){return this->previousPower;}
int Tank::getPreviousAngle(){return this->previousAngle;}
GLfloat* Tank::getProjectileLandPos(){return this->projectileLandPos;}
void Tank::setPreviousPower(int previousPower){this->previousPower = previousPower;}
void Tank::setPreviousAngle(int previousAngle){this->previousAngle = previousAngle;}
void Tank::setProjectileLandPos(GLfloat x, GLfloat y){
	this->projectileLandPos[0] = x;
	this->projectileLandPos[1] = y;
}

// remove later probably
void Tank::rotateWheel(GLfloat degrees) {
	if(!(this->wheel_degrees+degrees > 90)||(this->wheel_degrees-degrees < 0)) {
		this->wheel_degrees+=degrees;
		glPushMatrix();
			glLoadMatrixf(this->wheelMatrix);
			glRotatef(degrees,this->wheelMatrix[4],this->wheelMatrix[5],this->wheelMatrix[6]);
			glGetFloatv(GL_MODELVIEW_MATRIX,this->wheelMatrix);
		glPopMatrix();
	}		
}

void Tank::changeHeadTexture(int currentPlayerIndex){
	if(currentPlayerIndex == 0)				this->vbo_shader_head->SwapTexture("player1Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 1)		this->vbo_shader_head->SwapTexture("player2Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 2)		this->vbo_shader_head->SwapTexture("player3Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 3)		this->vbo_shader_head->SwapTexture("player4Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 4)		this->vbo_shader_head->SwapTexture("player5Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 5)		this->vbo_shader_head->SwapTexture("player6Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 6)		this->vbo_shader_head->SwapTexture("player7Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 7)		this->vbo_shader_head->SwapTexture("player8Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 8)		this->vbo_shader_head->SwapTexture("player9Head.raw", 1024, 1024);
	else if(currentPlayerIndex == 9)		this->vbo_shader_head->SwapTexture("player10Head.raw", 1024, 1024);
	else{
		printf("\nERROR <Tank::changeHeadTexture(int)>: Wrong parameter passing.\n");
	}
}

int Tank::getCurrentHeight() {
	return this->currentHeight;
}

void  Tank::setCurrentHeight(int curr_height) {
	this->currentHeight = curr_height;
}

int  Tank::getPreviousHeight() {
	return this->previousHeight;
}

void  Tank::setPreviousHeight(int prev_height) {
	this->previousHeight = prev_height;
}


// Getters for durations
int Tank::getDurationAcid(){return this->durationAcid;}
int Tank::getDurationShield(){return this->durationShield;}
int Tank::getDurationEMP(){return this->durationEMP;}
int Tank::getDurationFloat(){return this->durationFloat;}
int Tank::getDurationDoubleAction(){return this->durationDoubleAction;}
int Tank::getDurationPadlock(){return this->durationPadlock;}
int Tank::getDurationCloak(){return this->durationCloak;}
int Tank::getDurationParalyze(){return this->durationParalyze;}
// Setters for durations
void Tank::setDurationAcid(int value){
	if(value!=0){
		if(this->acidGen) delete this->acidGen;
		this->acidGen = new ParticleGenerator(10,5,2,100,1);
	} else {
		if(this->acidGen){
			this->acidGen->killGenerator();
		this->acidGen=NULL;
		}	
	}
	this->durationAcid = value;
}
void Tank::setDurationShield(int value){this->durationShield = value;}
void Tank::setDurationEMP(int value){this->durationEMP = value;}
void Tank::setDurationFloat(int value){
	if(value!=0){
		if(this->floatGen) delete this->floatGen;
		this->floatGen = new ParticleGenerator(10,5,2,100,2);
	} else {
		if(this->floatGen){
			this->floatGen->killGenerator();
		this->floatGen=NULL;
		}	
	}
	this->durationFloat = value;
}
void Tank::setDurationDoubleAction(int value){this->durationDoubleAction = value;}
void Tank::setDurationPadlock(int value){this->durationPadlock = value;}
void Tank::setDurationCloak(int value){this->durationCloak = value;}
void Tank::setDurationParalyze(int value){this->durationParalyze = value;}
void Tank::setDurationAllPassTurn(){
	//acid
	if(durationAcid>0){
		durationAcid--;
	} 
	if((this->acidGen)&&(durationAcid==0)){
		this->acidGen->killGenerator();
		this->acidGen=NULL;
	}
	
	//float
	if(durationFloat>0){
		durationFloat--;
	} 
	if((this->floatGen)&&(durationFloat==0)){
		this->floatGen->killGenerator();
		this->floatGen=NULL;
	}
	if(durationEMP>0)			durationEMP--;
	if(durationDoubleAction>0)	durationDoubleAction--;
	if(durationPadlock>0)		durationPadlock--;
	if(durationCloak>0)			durationCloak--;
	if(durationParalyze>0)		durationParalyze--;
}
void Tank::initBody(){
	this->bodyRight[0]=this->bodyMatrix[0]=0;
	this->bodyRight[1]=this->bodyMatrix[1]=0;
	this->bodyRight[2]=this->bodyMatrix[2]=1;
	this->bodyMatrix[3]=0;
	this->bodyUp[0]=this->bodyMatrix[4]=0;
	this->bodyUp[1]=this->bodyMatrix[5]=1;
	this->bodyUp[2]=this->bodyMatrix[6]=0;
	this->bodyMatrix[7]=0;
	this->bodyAt[0]=this->bodyMatrix[8]=1;
	this->bodyAt[1]=this->bodyMatrix[9]=0;
	this->bodyAt[2]=this->bodyMatrix[10]=0;
	this->bodyMatrix[11]=0;
	this->bodyMatrix[12]=0;
	this->bodyMatrix[13]=0;
	this->bodyMatrix[14]=0;
	this->bodyMatrix[15]=1;

	this->bodyColor[0]=0.50f;
	this->bodyColor[1]=0.50f;
	this->bodyColor[2]=0.50f;
	this->bodyColor[3]=1.0f;
	this->bodyScale[0]=70;
	this->bodyScale[1]=70;
	this->bodyScale[2]=70;
	this->previousHeight = this->currentHeight = this->bodyMatrix[13];
}
void Tank::initHead(){
	this->headRight[0]=this->headMatrix[0]=0;
	this->headRight[1]=this->headMatrix[1]=0;
	this->headRight[2]=this->headMatrix[2]=1;
	this->headMatrix[3]=0;
	this->headUp[0]=this->headMatrix[4]=0;
	this->headUp[1]=this->headMatrix[5]=1;
	this->headUp[2]=this->headMatrix[6]=0;
	this->headMatrix[7]=0;
	this->headAt[0]=this->headMatrix[8]=1;
	this->headAt[1]=this->headMatrix[9]=0;
	this->headAt[2]=this->headMatrix[10]=0;
	this->headMatrix[11]=0;
	this->headMatrix[12]=0;
	this->headMatrix[13]=0;
	this->headMatrix[14]=0;
	this->headMatrix[15]=1;
	this->headColor[0]=0.50f;
	this->headColor[1]=0.50f;
	this->headColor[2]=0.50f;
	this->headColor[3]=1.0f;
	this->headScale[0]=70;
	this->headScale[1]=70;
	this->headScale[2]=70;
}
void Tank::initTurret(){
	this->turretRight[0]=this->turretMatrix[0]=0;
	this->turretRight[1]=this->turretMatrix[1]=0;
	this->turretRight[2]=this->turretMatrix[2]=1;
	this->turretMatrix[3]=0;
	this->turretUp[0]=this->turretMatrix[4]=0;
	this->turretUp[1]=this->turretMatrix[5]=1;
	this->turretUp[2]=this->turretMatrix[6]=0;
	this->turretMatrix[7]=0;
	this->turretAt[0]=this->turretMatrix[8]=1;
	this->turretAt[1]=this->turretMatrix[9]=0;
	this->turretAt[2]=this->turretMatrix[10]=0;
	this->turretMatrix[11]=0;
	this->turretMatrix[12]=0;
	this->turretMatrix[13]=0;
	this->turretMatrix[14]=0;
	this->turretMatrix[15]=1;
	this->turretColor[0]=0.50f;
	this->turretColor[1]=0.50f;
	this->turretColor[2]=0.50f;
	this->turretColor[3]=1.0f;
	this->turretScale[0]=70;
	this->turretScale[1]=70;
	this->turretScale[2]=70;

	this->turretOffset[0]=0;
	this->turretOffset[1]=0;
	this->turretOffset[2]=0;

	this->turret_degrees=0;
	this->wheel_degrees=0;
}
void Tank::initWheel(){
	this->wheelRight[0]=this->wheelMatrix[0]=0;
	this->wheelRight[1]=this->wheelMatrix[1]=0;
	this->wheelRight[2]=this->wheelMatrix[2]=1;
	this->wheelMatrix[3]=0;
	this->wheelUp[0]=this->wheelMatrix[4]=0;
	this->wheelUp[1]=this->wheelMatrix[5]=1;
	this->wheelUp[2]=this->wheelMatrix[6]=0;
	this->wheelMatrix[7]=0;
	this->wheelAt[0]=this->wheelMatrix[8]=1;
	this->wheelAt[1]=this->wheelMatrix[9]=0;
	this->wheelAt[2]=this->wheelMatrix[10]=0;
	this->wheelMatrix[11]=0;
	this->wheelMatrix[12]=0;
	this->wheelMatrix[13]=0;
	this->wheelMatrix[14]=0;
	this->wheelMatrix[15]=1;
	this->wheelColor[0]=0.50f;
	this->wheelColor[1]=0.50f;
	this->wheelColor[2]=0.50f;
	this->wheelColor[3]=1.0f;
	this->wheelScale[0]=70;
	this->wheelScale[1]=70;
	this->wheelScale[2]=70;
}

// GETTERS
const GLfloat* Tank::getBodyMatrix()		{return &bodyMatrix[0];}
const GLfloat* Tank::getBodyColor()			{return &bodyColor[0];}
float* Tank::getBodyScale()					{return &bodyScale[0];}

const GLfloat* Tank::getHeadMatrix()		{return &headMatrix[0];}
const GLfloat* Tank::getHeadColor()			{return &headColor[0];}
float* Tank::getHeadScale()					{return &headScale[0];}

GLfloat* Tank::getTurretMatrix()			{return &turretMatrix[0];}
const GLfloat* Tank::getTurretColor()		{return &turretColor[0];}
float* Tank::getTurretScale()				{return &turretScale[0];}

const GLfloat* Tank::getWheelMatrix()		{return &wheelMatrix[0];}
const GLfloat* Tank::getWheelColor()		{return &wheelColor[0];}
float* Tank::getWheelScale()				{return &wheelScale[0];}
	
int Tank::getHP()							{return this->HP;}
int Tank::getPower()						{return this->power;}
int Tank::getArmor()						{return this->armor;}
int Tank::getSpeed()						{return this->speed;}
Tank* Tank::getTankPointer()				{return this;}