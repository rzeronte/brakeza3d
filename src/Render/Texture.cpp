#include "../../headers/Render/Texture.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/EngineBuffers.h"
#include <SDL_image.h>

Texture::Texture()
{
}

Texture::Texture(int w, int h)
{
    this->texture_surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
}

SDL_Surface *Texture::getSurface()
{
    return this->texture_surface;
}

void Texture::loadBMP(const char *file )
{
    this->filename = file;
    this->loaded = true;
    texture_surface = SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"),1);
    //printf("Loading BMP texture '%s' (w: %d, h: %d)\r\n", file, texture_surface->w, texture_surface->h);
}

void Texture::loadJPG(const char *file )
{
    this->filename = file;
    this->loaded = true;
    texture_surface = IMG_Load(file);
    //printf("Loading JPG texture '%s' (w: %d, h: %d)\r\n", file, texture_surface->w, texture_surface->h);
}

void Texture::loadTGA(const char *file )
{
    if (Tools::fileExists(file)) {
        this->filename = file;
        this->loaded = true;
        texture_surface = IMG_Load(file);
    }
    //Logging::getInstance()->Log("Loading TGA texture '" + std::string(file), "TEXTURES");
}

void Texture::drawFlat(int pos_x, int pos_y)
{
    int width = texture_surface->w;
    int height = texture_surface->h;

    for (int i = 0 ; i < height ; i++) {
        for (int j = 0 ; j < width ; j++) {
            Uint32 *pixels = (Uint32 *)texture_surface->pixels;
            EngineBuffers::getInstance()->setVideoBuffer( j + pos_x, i + pos_y, pixels[i * getSurface()->w + j]);
        }
    }
}

void Texture::consoleInfo()
{
    Logging::getInstance()->Log("consoleInfo: Texture " + std::to_string(loaded) +  "(w: " + std::to_string(texture_surface->w) + ", h: " + std::to_string(texture_surface->h), "TEXTURE");
}

void Texture::loadFromRaw(unsigned int *texture, int w, int h)
{
    //texture_surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    //memcpy (&texture_surface->pixels, &texture, sizeof(texture_surface->pixels));

    int width = texture_surface->w;
    int height = texture_surface->h;

    Uint32 *pixels = (Uint32 *)texture_surface->pixels;

    for (int i = 0 ; i < height ; i++) {
        for (int j = 0 ; j < width ; j++) {
            Tools::SurfacePutPixel(texture_surface, j, i, texture[i * w + j]);
            //EngineBuffers::getInstance()->setVideoBuffer(j, i, texture[i * w + j]);
        }
    }
}

void Texture::setFilename(const std::string &filename) {
    Texture::filename = filename;
}

const std::string &Texture::getFilename() const {
    return filename;
}
