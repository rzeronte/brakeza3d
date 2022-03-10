
#include "../../include/2D/TextureAnimatedDirectional.h"

#include <utility>
#include "../../include/EngineSetup.h"

TextureAnimatedDirectional::TextureAnimatedDirectional() {
}

void TextureAnimatedDirectional::setup(std::string file, int newNumFrames, int newFps, int newMaxTimes) {
    this->base_file = std::move(file);
    this->numFrames = newNumFrames;
    this->fps = newFps;
    this->maxTimes = newMaxTimes;
}

void TextureAnimatedDirectional::loadImages() {
    for (int d = 1; d <= 8; d++) {
        for (int i = 0; i < this->getNumFrames(); i++) {
            std::string file = this->base_file + "/" + std::to_string(d) + "_" + std::to_string(i) + ".png";
            this->frames[d][i] = new Texture(file);
        }
    }
}

void TextureAnimatedDirectional::loadImagesForZeroDirection() {
    int d = 0;
    for (int i = 0; i < this->getNumFrames(); i++) {
        std::string file = this->base_file + "/" + std::to_string(d) + "_" + std::to_string(i) + ".png";
        this->frames[0][i] = new Texture(file);
    }
}

int TextureAnimatedDirectional::getNumFrames() const {
    return numFrames;
}

Texture *TextureAnimatedDirectional::getCurrentFrame(int direction) {
    return this->frames[direction][current];
}

void TextureAnimatedDirectional::nextFrame() {
    current++;

    if (current >= this->getNumFrames()) {
        current = 0;
        times++;
        if ((maxTimes != -1 && times >= maxTimes)) {
            current = getNumFrames() - 1;
        }
    }
}

void TextureAnimatedDirectional::importTextures(TextureAnimatedDirectional *origin, int numFrames) {
    for (int d = 0; d <= 8; d++) {
        for (int j = 0; j < numFrames; j++) {
            this->frames[d][j] = origin->frames[d][j];
        }
    }
}