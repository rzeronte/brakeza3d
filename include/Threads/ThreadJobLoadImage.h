#ifndef BRAKEZA3D_JOBLOADIMAGE_H
#define BRAKEZA3D_JOBLOADIMAGE_H

#include <string>
#include <utility>

#include "ThreadJobBase.h"
#include "../Render/Image.h"

class ThreadJobLoadImage : public ThreadJobBase
{
    std::string filename;
    Image* image = nullptr;
public:
    ThreadJobLoadImage(Image* image, std::string filename)
    :
        filename(std::move(filename)),
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
        image->LoadSDLSurface();
        image->CreateSDLTexture();
        image->MakeAutoOGLImage();
        image->setAlreadyLoaded();

        Logging::Message("[ThreadJobLoadMesh3D] Callback END");
    }
};

#endif //BRAKEZA3D_JOBLOADIMAGE_H