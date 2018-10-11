
#ifndef BRAKEDA3D_BILLBOARDDIRECTIONAL_H
#define BRAKEDA3D_BILLBOARDDIRECTIONAL_H


#include "Billboard.h"

#define SPRITE_STATE_STOPPED 0
#define SPRITE_STATE_WALKING 1
#define SPRITE_STATE_SHOOTING 2
#define SPRITE_STATE_DYING 3


class BillboardDirectional : public Billboard {

public:
    const static int frames = 5;
    const static int directions = 8;

    Texture *frames_stopped[directions][frames];
    Texture *frames_walking[directions][frames];
    Texture *frames_shooting[directions][frames];
    Texture *frames_dying[directions][frames];

    int state;

    BillboardDirectional();

    void loadSprite(std::string);
    void loadTextureDirectional(std::string, std::string);
    void updateTextureFromCameraAngle(Object3D *, Camera *);
};


#endif //BRAKEDA3D_BILLBOARDDIRECTIONAL_H
