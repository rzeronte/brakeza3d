/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 13/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBLOADPARTICLEEMITTER_H
#define BRAKEZA3D_THREADJOBLOADPARTICLEEMITTER_H

#include "ThreadJobBase.h"
#include "../3D/ParticleEmitter.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"

class ThreadJobLoadParticleEmitter : public ThreadJobBase
{
    cJSON* json = nullptr;
    ParticleEmitter* emitter = nullptr;
public:
    ThreadJobLoadParticleEmitter(ParticleEmitter* light, cJSON* json)
    :
        json(cJSON_Duplicate(json, 1)),
        emitter(light)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobLoadParticleEmitter] Process END");
    }

    void fnCallback()
    {
        emitter->InitGLBuffers();
        Brakeza::get()->AddObject3D(emitter, emitter->getName());
        LOG_MESSAGE("[ThreadJobLoadParticleEmitter] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBLOADPARTICLEEMITTER_H