//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_FXOPENGL_H
#define BRAKEZA3D_FXOPENGL_H

#include "FXBase.h"
#include "../Misc/cJSON.h"

class FXOpenGL: public FXBase {
protected:
public:
    explicit FXOpenGL(bool active) : FXBase(active)
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


#endif //BRAKEZA3D_FXOPENGL_H
