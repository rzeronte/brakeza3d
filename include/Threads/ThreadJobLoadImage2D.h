#ifndef BRAKEZA3D_THREADJOBLOADIMAGE2D_H
#define BRAKEZA3D_THREADJOBLOADIMAGE2D_H

#include "ThreadJobBase.h"
#include "../2D/Image2D.h"
#include "../Misc/Logging.h"
#include "../Serializers/Image2DSerializer.h"

class ThreadJobLoadImage2D: public ThreadJobBase
{
    Image2D *image = nullptr;
    cJSON *json = nullptr;
public:
    ThreadJobLoadImage2D(Image2D *image, cJSON *json)
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
            Logging::Error("[ThreadJobLoadImage2D] Image3D pointer is null");
            return;
        }

        Logging::Message("[ThreadJobLoadImage2D] Process END");
    }

    void fnCallback()
    {
        if (!image) {
            Logging::Error("[ThreadJobLoadImage2D] Mesh was destroyed before callback");
            return;
        }

        Image2DSerializer::ApplyImageFromFile(image);

        Brakeza::get()->AddObject3D(image, image->getName());

        Logging::Message("[ThreadJobLoadImage2D] Callback END");
    }

    ~ThreadJobLoadImage2D()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_THREADJOBLOADIMAGE2D_H