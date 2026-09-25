//
// Created by Eduardo on 10/07/2026.
//

#ifndef BRAKEZA3D_THREADJOBLOADSOUND3D_H
#define BRAKEZA3D_THREADJOBLOADSOUND3D_H

#include "ThreadJobBase.h"
#include "../3D/Sound3D.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"

class ThreadJobLoadSound3D : public ThreadJobBase
{
    Sound3D* sound = nullptr;
    cJSON*   json  = nullptr;
public:
    ThreadJobLoadSound3D(Sound3D* s, cJSON* j)
    :
        sound(s),
        json(cJSON_Duplicate(j, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobLoadSound3D] Process END");
    }

    void fnCallback()
    {
        if (!sound->sourceFile.empty()) {
            sound->bufferLoaded = Sound3D::DecodeFile(sound->sourceFile, sound->buffer, sound->pDecodedData);
        }

        Brakeza::get()->AddObject3D(sound, sound->getName());
        LOG_MESSAGE("[ThreadJobLoadSound3D] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBLOADSOUND3D_H
