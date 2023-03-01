#include <SDL_image.h>

#include <utility>
#include "../../include/Misc/Image.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"

Image::Image(std::string filename): surface(nullptr), texture(nullptr), loaded(false)
{
    this->loadTGA(std::move(filename));
}


Image::Image(): surface(nullptr), texture(nullptr)
{
    this->loaded = false;
}

void Image::createEmpty(int w, int h)
{
    this->surface = SDL_CreateRGBSurface(SDL_PIXELFORMAT_RGBA32, w, h, 32, 0, 0, 0, 0);
    this->loaded = true;
}

void Image::loadTGA(const std::string& filename)
{
    if (Tools::fileExists(filename.c_str())) {
        this->surface = IMG_Load(filename.c_str());
        this->texture = SDL_CreateTextureFromSurface(ComponentsManager::get()->getComponentWindow()->getRenderer(), surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        this->fileName = filename;
        this->loaded = true;
        Logging::Log("Loading TGA texture '%s'", filename.c_str());

    } else {
        Logging::Log("Error loading TGA texture '%s'", filename.c_str());
    }
}

void Image::drawFlatAlpha(int pos_x, int pos_y, float alpha)
{
    setTextureAlpha((int) alpha);
    drawFlat(pos_x, pos_y);
}

void Image::drawFlat(int pos_x, int pos_y) const
{
    if (!loaded) return;

    auto renderer = ComponentsManager::get()->getComponentWindow()->getRenderer();

    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = surface->w;
    srcRect.h = surface->h;

    SDL_Rect dstRect;
    dstRect.x = (pos_x * windowWidth) / EngineSetup::get()->screenWidth;
    dstRect.y = (pos_y * windowHeight) / EngineSetup::get()->screenHeight;
    dstRect.w = (surface->w * windowWidth) / EngineSetup::get()->screenWidth;
    dstRect.h = (surface->h * windowHeight) / EngineSetup::get()->screenHeight;

    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
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
    SDL_DestroyTexture(texture);
}

Color Image::getColor(int x, int y)
{
    uint32_t *pixels = (uint32_t*) this->pixels();

    const int index = y * this->surface->w + x;

    return Color(pixels[index]);
}

void Image::setSurface(SDL_Surface *surface) {
    Image::surface = surface;
    this->loaded = true;
}

void Image::setTextureAlpha(int value)
{
    SDL_SetTextureAlphaMod(texture, value);

}
