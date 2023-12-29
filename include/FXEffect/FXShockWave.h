//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_FXSHOCKWAVE_H
#define BRAKEZA3D_FXSHOCKWAVE_H


#include "../OpenGL/FXEffectOpenGLObject.h"
#include "../Misc/Counter.h"

class FXShockWave : public FXEffectOpenGLObject {
    Counter ttlWave;
    bool deleteWhenEnds;
public:
    FXShockWave(bool active, Object3D *object, float ttl);

    void update() override;

    void preUpdate() override;

    void postUpdate() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;
};


#endif //BRAKEZA3D_FXSHOCKWAVE_H
