#include <SDL_image.h>
#include "../../include/Misc/Image.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Maths.h"

Image::Image()
{
    this->loaded = false;
}

Image::Image(std::string filename)
{
    this->loadTGA(filename);
}

void Image::loadTGA(std::string filename)
{
    if (Tools::fileExists(filename)) {
        this->surface = IMG_Load(filename.c_str());
        this->fileName = filename;
        this->loaded = true;
        Logging::Log("Loading TGA texture '" + std::string(filename), "IMAGE");

    } else {
        Logging::Log("Error loading TGA texture '" + std::string(filename), "IMAGE");
    }
}

void Image::drawFlat(int pos_x, int pos_y) const
{
    if (!loaded) return;

    auto *buffer = EngineBuffers::getInstance();
    auto *pixels = (Uint32 *) surface->pixels;
    for (int i = 0; i < std::min(EngineSetup::get()->screenHeight, surface->h); i++) {
        for (int j = 0; j < std::min(EngineSetup::get()->screenWidth, surface->w); j++) {
            int x = j + pos_x;
            int y = i + pos_y;
            if (Tools::isPixelInWindow(x, y)) {
                const auto pixel = pixels[i * surface->w + j];
                Uint8 red, green, blue, alpha;
                SDL_GetRGBA(pixel, surface->format, &red, &green, &blue, &alpha);
                if (alpha == 0) {
                    continue;
                }
                buffer->setVideoBuffer(x, y, pixel);
            }
        }
    }
}

void Image::loadFromRaw(unsigned int *texture, int w, int h)
{
    this->surface = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Tools::SurfacePutPixel(surface, x, y, texture[y + x * w]);
        }
    }

}

int Image::width() const {
    return surface->w;
}

int Image::height()
{
    return surface->h;
}

void * Image::pixels()
{
    return surface->pixels;
}

bool Image::isLoaded()
{
    return loaded;
}

float Image::getAreaForVertices(Vertex3D A, Vertex3D B, Vertex3D C, int lod)
{
    float tx0 = Tools::getXTextureFromUV(surface, A.u / surface->w);
    float ty0 = Tools::getYTextureFromUV(surface, A.v / surface->h);

    float tx1 = Tools::getXTextureFromUV(surface, B.u / surface->w);
    float ty1 = Tools::getYTextureFromUV(surface, B.v / surface->h);

    float tx2 = Tools::getXTextureFromUV(surface, C.u / surface->w);
    float ty2 = Tools::getYTextureFromUV(surface, C.v / surface->h);

    float area = Maths::TriangleArea(tx0, ty0, tx1, ty1, tx2, ty2);

    return area;
}

SDL_Surface *Image::getSurface() const {
    return surface;
}

const std::string &Image::getFileName() const {
    return fileName;
}

Image::~Image()
{
    SDL_FreeSurface(surface);
}
