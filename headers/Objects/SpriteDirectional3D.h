
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H


#include "Object3D.h"
#include "../Render/Timer.h"
#include "../2D/TextureAnimationDirectional.h"
#include "Camera3D.h"
#include "Enemy.h"
#include "Triangle3D.h"
#include "../Render/Billboard.h"

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

class SpriteDirectional3D : public Object3D {
    Billboard *billboard;

public:
    float width;
    float height;

    Timer *timer;

    int num_animations = 0;
    int current_animation = 0;

    float timerLastTicks;
    float timerCurrent = 0;

    TextureAnimationDirectional *animations[BILLBOARD3D_MAX_ANIMATIONS];

    SpriteDirectional3D();

    Billboard *getBillboard() const;
    void addAnimationDirectional2D(std::string, int frames, int fps, bool zeroDirection, int maxTimes);
    void updateTextureFromCameraAngle(Object3D *, Camera3D *);
    void setAnimation(int);
    void setTimer(Timer *);

    void updateTrianglesCoordinates(Camera3D *cam);
    void linkTexturesTo(SpriteDirectional3D *clone);

    TextureAnimationDirectional* getCurrentTextureAnimationDirectional();
};


#endif //BRAKEDA3D_SPRITE_H
