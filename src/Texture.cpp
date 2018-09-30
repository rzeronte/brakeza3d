#include "../headers/Texture.h"
#include "../headers/Tools.h"
#include "../headers/Color.h"
#include "../headers/Core/Logging.h"
#include <SDL_image.h>

Texture::Texture()
{
    loaded = false;
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
    this->filename = file;
    this->loaded = true;
    texture_surface = IMG_Load(file);
    //printf("Loading TGA texture '%s' (w: %d, h: %d)\r\n", file, texture_surface->w, texture_surface->h);
}

void Texture::drawFlat(SDL_Surface *surface)
{
    int width = texture_surface->w;
    int height = texture_surface->h;

    for (int i = 0 ; i < height ; i++) {
        float v = (float) i / height;
        for (int j = 0 ; j < width ; j++) {
            float u = (float) j / width;

            //Convert the pixels to 32 bit
            Uint32 *pixels = (Uint32 *)texture_surface->pixels;

            Uint32 directRead = Tools::readSurfacePixel(texture_surface, j, i);

            Uint32 colorFromUV = Tools::readSurfacePixelFromUV(texture_surface, u, v);

            Tools::SurfacePutPixel(surface, j, i, colorFromUV);

        }
    }
}

void Texture::drawUVFlat(SDL_Surface *surface, float u, float v, Uint32 color)
{
    int x = Tools::getXTextureFromUV(texture_surface, u);
    int y = Tools::getYTextureFromUV(texture_surface, v);

    Tools::SurfacePutPixel(surface, x, y, color);
}

void Texture::consoleInfo()
{
    Logging::getInstance()->Log("consoleInfo: Texture " + std::to_string(loaded) +  "(w: " + std::to_string(texture_surface->w) + ", h: " + std::to_string(texture_surface->h), "TEXTURE");
}
