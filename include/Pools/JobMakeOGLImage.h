#ifndef BRAKEZA3D_JOBMAKEOGLIMAGE_H
#define BRAKEZA3D_JOBMAKEOGLIMAGE_H

#include "PendingJob.h"
#include "../Render/Image.h"

class JobMakeOGLImage : public PendingJob
{
    Image* image;
public:
    explicit JobMakeOGLImage(Image* image)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        image(image)
    {
    }

    void process()
    {
    }

    void callback()
    {
        Logging::Message("[Pools] JobMakeOGLImage");
        image->MakeAutoOGLImage();
    }
};


#endif //BRAKEZA3D_JOBMAKEOGLIMAGE_H