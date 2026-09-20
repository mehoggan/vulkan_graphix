#include "TexCoord.h"
#include "macro_crtdbg.h"


TexCoord::TexCoord(){
}

TexCoord::TexCoord(GLfloat s, GLfloat t){
	this->texcoordS=s;
	this->texcoordT=t;
}

TexCoord::~TexCoord(){
}