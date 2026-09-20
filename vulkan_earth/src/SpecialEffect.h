#ifndef SPECIAL_EFFECT_H_
#define SPECIAL_EFFECT_H_

#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

class SpecialEffect {
	public:
		SpecialEffect();
		virtual ~SpecialEffect();
		virtual void draw() = 0;
		virtual void setColors1(GLfloat* colors1)=0;
		virtual void setColors2(GLfloat* colors2)=0;
		virtual void setColors3(GLfloat* colors3)=0;
		virtual void setColors4(GLfloat* colors4)=0;
		virtual void setDefaultColors()=0;
};

#endif SPECIAL_EFFECT_H_