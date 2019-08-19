
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H


#include "Object3D.h"
#include "../Render/EngineSetup.h"
#include "../Render/Timer.h"
#include "Camera3D.h"

#define ANIMATEDSPRITE_MAX_ANIMATIONS 5

class TextureAnimation;

class Sprite3D : public Object3D {
    Billboard *billboard;

public:

    float width = EngineSetup::getInstance()->BILLBOARD_WIDTH_DEFAULT;
    float height = EngineSetup::getInstance()->BILLBOARD_HEIGHT_DEFAULT;
    int numAnimations = 0;
    int currentAnimationIndex = 0;

    TextureAnimation *animations[ANIMATEDSPRITE_MAX_ANIMATIONS];

    int fps = 2;

    Timer *timer;
    float timerLastTicks;
    float timerCurrent = 0;

    bool autoRemoveAfterAnimation = false;

    Sprite3D();

    Billboard *getBillboard() const;

    void addAnimation(std::string, int);
    void setAnimation(int);
    void setTimer(Timer *);
    void updateTexture();
    void updateTrianglesCoordinatesAndTexture(Camera3D *cam);
    void draw(Camera3D *cam);
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
