#include <SDL_image.h>
#include "../../include/Misc/Image.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Maths.h"

Image::Image() {
    this->loaded = false;
}

Image::Image(std::string filename) {
    this->loadTGA(filename);
}


void Image::loadTGA(std::string filename) {
    if (Tools::fileExists(filename)) {
        this->surface = IMG_Load(filename.c_str());
        this->fileName = filename;
        this->loaded = true;
    } else {
        Logging::Log("Error loading TGA texture '" + std::string(filename), "IMAGE");
    }
}

void Image::drawFlat(int pos_x, int pos_y) const {
    auto *buffer = EngineBuffers::getInstance();
    auto *pixels = (Uint32 *) surface->pixels;
    for (int i = 0; i < surface->h; i++) {
        for (int j = 0; j < surface->w; j++) {
            if (Tools::isPixelInWindow(j, i)) {
                buffer->setVideoBuffer(j + pos_x, i + pos_y, pixels[i * surface->w + j]);
            }
        }
    }
}

void Image::loadFromRaw(unsigned int *texture, int w, int h) {
    this->surface = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Tools::SurfacePutPixel(surface, x, y, texture[y + x * w]);
        }
    }

}
int Image::width() {
    return surface->w;
}

int Image::height() {
    return surface->h;
}

void * Image::pixels() {
    return surface->pixels;
}

bool Image::isLoaded() {
    return loaded;
}

float Image::getAreaForVertices(Vertex3D A, Vertex3D B, Vertex3D C, int lod) {
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