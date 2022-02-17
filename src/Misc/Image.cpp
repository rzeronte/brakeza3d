#include <SDL_image.h>
#include "../../include/Misc/Image.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineBuffers.h"

Image::Image() {
    this->loaded = false;
}

Image::Image(std::string filename) {
    this->filename = filename;
    loadImage();
}

bool Image::loadImage() {
    if (Tools::fileExists(filename)) {
        surface = IMG_Load(filename.c_str());
        this->loaded = true;
    } else {
        Logging::Log("Error loading TGA texture '" + std::string(filename), "IMAGE");
        return false;
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

