
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H


#include "Object3D.h"
#include "../Misc/Timer.h"
#include "../2D/TextureAnimationDirectional.h"
#include "../Components/Camera3D.h"
#include "../Game/Enemy.h"
#include "Triangle3D.h"
#include "../Render/Billboard.h"
#include "../Misc/Counter.h"

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

    Counter *counterAnimations;
    float step = 0;

    int numAnimations = 0;
    int currentAnimation = 0;

    TextureAnimationDirectional *animations[BILLBOARD3D_MAX_ANIMATIONS];

    SpriteDirectional3D();

    Billboard *getBillboard() const;
    void addAnimationDirectional2D(std::string, int frames, int fps, bool zeroDirection, int maxTimes);
    void updateTextureFromCameraAngle(Object3D *, Camera3D *);
    void setAnimation(int);

    int getDirectionForAngle(float enemyAngle);
    void updateStep();
    void updateTrianglesCoordinates(Camera3D *cam);
    void linkTexturesTo(SpriteDirectional3D *clone);

    TextureAnimationDirectional* getCurrentTextureAnimationDirectional();
};


#endif //BRAKEDA3D_SPRITE_H
