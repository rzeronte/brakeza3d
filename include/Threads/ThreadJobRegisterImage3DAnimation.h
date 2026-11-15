/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_THREADJOBREGISTERIMAGE3DANIMATION_H
#define BRAKEZA3D_THREADJOBREGISTERIMAGE3DANIMATION_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Image3DAnimation.h"

// A diferencia de ThreadJobLoadImage3DAnimation, este job no carga nada:
// las texturas ya están resueltas de forma síncrona desde TextureAnimatedCache
// antes de encolar el job (ver ObjectFactory::CreateImage3DAnimation). Solo
// difiere el AddObject3D al punto seguro de processMainThreadCallbacks(),
// evitando mutar la lista de objetos mientras el motor la está iterando.
class ThreadJobRegisterImage3DAnimation: public ThreadJobBase
{
    Image3DAnimation *image = nullptr;
public:
    explicit ThreadJobRegisterImage3DAnimation(Image3DAnimation *image)
    :
        image(image)
    {
        function = [](){};
        callback = [this](){ fnCallback(); };
    }

    void fnCallback() const
    {
        if (!image) return;
        Brakeza::get()->AddObject3D(image, image->getName());
    }
};

#endif //BRAKEZA3D_THREADJOBREGISTERIMAGE3DANIMATION_H
