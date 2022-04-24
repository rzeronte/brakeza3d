#include "../../include/Render/Texture.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Maths.h"
#include <SDL2/SDL_image.h>

Texture::Texture(std::string filename)
{
    image = new Image(filename);
}

Image *Texture::getImage() const {
    return image;
}

Texture::~Texture()
{
    delete image;
}

