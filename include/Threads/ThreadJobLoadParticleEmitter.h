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
        Brakeza::get()->AddObject3D(emitter, emitter->getName());
        LOG_MESSAGE("[ThreadJobLoadParticleEmitter] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobLoadParticleEmitter] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBLOADPARTICLEEMITTER_H