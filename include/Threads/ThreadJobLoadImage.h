#ifndef BRAKEZA3D_JOBLOADIMAGE_H
#define BRAKEZA3D_JOBLOADIMAGE_H

#include <string>

#include "ThreadJobBase.h"
#include "../Render/Image.h"
#include "../Render/Profiler.h"

class ThreadJobLoadImage : public ThreadJobBase
{
    std::string filename;
    Image* image = nullptr;
public:
    ThreadJobLoadImage(Image* image, std::string filename)
    :
        filename(filename),
        image(image)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        image->setImage(filename);
    }

    void fnCallback()
    {
        image->CreateSDLTexture();
        image->MakeAutoOGLImage();
        image->setAlreadyLoaded();

        Profiler::get()->AddImage(image);

        LOG_MESSAGE("[ThreadJobLoadImage] Callback END");
    }
};

#endif //BRAKEZA3D_JOBLOADIMAGE_H