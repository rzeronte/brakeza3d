#ifndef BRAKEZA3D_JOBLOADIMAGE_H
#define BRAKEZA3D_JOBLOADIMAGE_H

#include <string>
#include <utility>

#include "PendingJob.h"
#include "../Misc/Logging.h"
#include "../Render/Image.h"

class JobLoadImage : public PendingJob
{
    std::string filename;
    Image* image;
public:
    JobLoadImage(Image* image, std::string filename)
    :
        PendingJob([image, filename](){ process(image, filename); },[image]() { callback(image); }),
        filename(std::move(filename)),
        image(image)
    {
    }

    static void process(Image *image, std::string filename)
    {
        image->setImage(filename);
        //Logging::Error("[Pools] JobLoadImage %s", filename.c_str());
    }

    static void callback(Image *image)
    {
        //image->LoadSDLSurface();
        //image->CreateSDLTexture();
        //image->MakeAutoOGLImage();
        //image->setAlreadyLoaded();
        //Logging::Error("[Pools] JobLoadImage %s: textureId: %d", filename.c_str(), (int) image->getOGLTextureID());

        //image->MakeAutoOGLImage();
        //auto job = std::make_shared<JobMakeOGLImage>(image);
        //Brakeza::get()->getPoolManager().Pool().enqueueWithMainThreadCallback(job);
    }
};


#endif //BRAKEZA3D_JOBLOADIMAGE_H