
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H


#include "Object3D.h"
#include "LTimer.h"
#include "AnimationDirectional2D.h"
#include "Camera.h"

#define DIR_C 0
#define DIR_S 1
#define DIR_SW 2
#define DIR_W 3
#define DIR_NW 4
#define DIR_N 5
#define DIR_NE 6
#define DIR_E 7
#define DIR_SE 8

#define BILLBOARD3D_MAX_ANIMATIONS 5

class SpriteDirectionalObject3D : public Object3D {
    Billboard *billboard;

public:
    float width;
    float height;

    LTimer *timer;

    int num_animations = 0;
    int current_animation = 0;

    int fps = 2;

    float last_ticks;
    float timerCurrent = 0;

    AnimationDirectional2D *animations[BILLBOARD3D_MAX_ANIMATIONS];

    SpriteDirectionalObject3D();

    Billboard *getBillboard() const;
    void addAnimationDirectional2D(std::string, int);
    void updateTextureFromCameraAngle(Object3D *, Camera *);
    void setAnimation(int);
    void setTimer(LTimer *);

    void draw(Camera *cam);
    void updateTrianglesCoordinates(Camera *cam);
};


#endif //BRAKEDA3D_SPRITE_H
