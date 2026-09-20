#include "Particle.h"

Particle::Particle(){}

void Particle::draw(){
	glPushMatrix();
		glTranslatef(x,y,z);
		glColor4f(red,green,blue,0.4);
		glutSolidSphere(size,10,10);
		/*glBegin(GL_QUADS);
		glVertex3f(-size,size,0);
		glVertex3f(-size,-size,0);
		glVertex3f(size,-size,0);
		glVertex3f(size,size,0);
	
		glVertex3f(-size,0,size);
		glVertex3f(-size,0,-size);
		glVertex3f(size,0,-size);
		glVertex3f(size,0,size);
	
		glVertex3f(0,-size,size);
		glVertex3f(0,-size,-size);
		glVertex3f(0,size,-size);
		glVertex3f(0,size,size);
		glEnd();*/

	glPopMatrix();
}