#ifndef SPECIAL_EFFECT_H_
#define SPECIAL_EFFECT_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>

class SpecialEffect {
public:
    SpecialEffect();
    virtual ~SpecialEffect();
    virtual void draw() = 0;
    virtual void setColors1(float* colors1) = 0;
    virtual void setColors2(float* colors2) = 0;
    virtual void setColors3(float* colors3) = 0;
    virtual void setColors4(float* colors4) = 0;
    virtual void setDefaultColors() = 0;
};

#endif  // SPECIAL_EFFECT_H_