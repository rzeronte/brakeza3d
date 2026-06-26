#ifndef BRAKEZA3D_JOBLOADIMAGE_H
#define BRAKEZA3D_JOBLOADIMAGE_H

#include <string>

#include "ThreadJobBase.h"
#include "../Render/Image.h"
#include "../Misc/FilePaths.h"

class ThreadJobLoadImage : public ThreadJobBase
{
    FilePath::ImageFile filename;
    Image* image = nullptr;
public:
    ThreadJobLoadImage(Image* image, const FilePath::ImageFile& filename)
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
        if (image->getSurface() == nullptr) {
            LOG_ERROR("[ThreadJobLoadImage] Surface is null, skipping '%s'", filename.c_str());
            return;
        }

        image->CreateSDLTexture();
        image->MakeAutoOGLImage();
        image->setAlreadyLoaded();

        LOG_MESSAGE("[ThreadJobLoadImage] Callback END");
    }
};

#endif //BRAKEZA3D_JOBLOADIMAGE_H