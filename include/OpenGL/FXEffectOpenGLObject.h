
#ifndef BRAKEZA3D_FXEFFECTOPENGLOBJECT_H
#define BRAKEZA3D_FXEFFECTOPENGLOBJECT_H

#include "FXOpenGL.h"
#include "../Objects/Object3D.h"

class FXEffectOpenGLObject : public FXOpenGL {
protected:
    Object3D *object;
public:
    FXEffectOpenGLObject(bool active, Object3D *object);

    [[nodiscard]] Object3D *getObject() const;

    void setObject(Object3D *object);
};


#endif //BRAKEZA3D_FXEFFECTOPENGLOBJECT_H
