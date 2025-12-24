//
// Created by Eduardo on 13/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBLOADIMAGE2DANIMATION_H
#define BRAKEZA3D_THREADJOBLOADIMAGE2DANIMATION_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../2D/Image2DAnimation.h"
#include "../Misc/Logging.h"
#include "../Serializers/Image2DAnimationSerializer.h"

class ThreadJobLoadImage2DAnimation: public ThreadJobBase
{
    Image2DAnimation *image = nullptr;
    cJSON *json = nullptr;
public:
    ThreadJobLoadImage2DAnimation(Image2DAnimation *image, cJSON *json)
    :
        image(image),
        json(cJSON_Duplicate(json, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (!image) {
            LOG_ERROR("[Pools] Image3D pointer is null");
            return;
        }

        LOG_MESSAGE("[Pools] ThreadJobLoadImage3D::fnProcess END");
    }

    void fnCallback()
    {
        if (!image) {
            LOG_ERROR("[Pools] Mesh was destroyed before callback");
            return;
        }

        Image2DAnimationSerializer::ApplyAnimationLoad(image);

        Brakeza::get()->AddObject3D(image, image->getName());

        LOG_MESSAGE("[ThreadJobLoadImage2DAnimation] Callback END");
    }

    ~ThreadJobLoadImage2DAnimation()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_THREADJOBLOADIMAGE2DANIMATION_H