#include "../../include/Render/Texture.h"
#include "../../include/Render/Logging.h"

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

