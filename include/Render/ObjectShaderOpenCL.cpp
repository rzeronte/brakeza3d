//
// Created by eduardo on 16/10/23.
//

#include "../OpenGL/FXEffectOpenGLObject.h"

FXEffectOpenGLObject::FXEffectOpenGLObject(bool active, Object3D *object)
:
        FXOpenGL(active),
        object(object)
{
}

Object3D *FXEffectOpenGLObject::getObject() const {
    return object;
}

void FXEffectOpenGLObject::setObject(Object3D *object) {
    FXEffectOpenGLObject::object = object;
}

