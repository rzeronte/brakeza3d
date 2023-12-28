//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_FXEFFECTOPENGL_H
#define BRAKEZA3D_FXEFFECTOPENGL_H


#include <CL/cl.h>
#include "FXEffectBase.h"
#include "../Misc/cJSON.h"

class FXEffectOpenGL: public FXEffectBase {
protected:
public:
    explicit FXEffectOpenGL(bool active) : FXEffectBase(active)
    {
    }

    virtual void preUpdate() {
    }

    virtual void postUpdate() {
    }

    virtual void drawImGuiProperties() {
    }

    virtual cJSON *getJSON() {
    }
};


#endif //BRAKEZA3D_FXEFFECTOPENGL_H
