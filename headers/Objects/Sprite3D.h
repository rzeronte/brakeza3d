
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H


#include "Object3D.h"
#include "../Components/Camera3D.h"
#include "../EngineSetup.h"
#include "../Misc/Timer.h"
#include "../2D/TextureAnimation.h"
#include "../Render/Billboard.h"
#include "../Misc/Counter.h"
#include <vector>

#define ANIMATEDSPRITE_MAX_ANIMATIONS 25

class Sprite3D : public Object3D {
    Billboard *billboard;

public:

    int numAnimations = 0;
    int currentAnimationIndex = 0;

    TextureAnimation *animations[ANIMATEDSPRITE_MAX_ANIMATIONS];

    Counter *counter;
    float step = 0;

    bool autoRemoveAfterAnimation = false;

    Sprite3D();

    Billboard *getBillboard() const;

    void addAnimation(std::string, int, int);
    void setAnimation(int);
    void updateTexture();
    void updateTrianglesCoordinatesAndTexture(Camera3D *cam);

    bool isAutoRemoveAfterAnimation() const;

    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);
    void linkTextureAnimation(Sprite3D *);

    TextureAnimation* getCurrentTextureAnimation();
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
