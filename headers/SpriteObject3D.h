
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H


#include "Object3D.h"
#include "LTimer.h"
#include "Billboard.h"
#include "Animation2D.h"

#define ANIMATEDSPRITE_MAX_ANIMATIONS 5

class Animation2D;

class SpriteObject3D : public Object3D{
    Billboard *billboard;

public:

    float width = EngineSetup::getInstance()->SPRITE3D_DEFAULT_WIDTH;
    float height = EngineSetup::getInstance()->SPRITE3D_DEFAULT_HEIGHT;
    int num_animations = 0;
    int current_animation = 0;

    Animation2D *animations[ANIMATEDSPRITE_MAX_ANIMATIONS];

    int fps = 2;

    LTimer *timer;
    float last_ticks;
    float timerCurrent = 0;

    SpriteObject3D();

    Billboard *getBillboard() const;

    void addAnimation(std::string, int);
    void setAnimation(int);
    void setTimer(LTimer *);
    void updateTexture();
    void updateTrianglesCoordinatesAndTexture(Camera *cam);
    void draw(Camera *cam);
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
