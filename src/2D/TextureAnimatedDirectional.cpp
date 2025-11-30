
#include "../../include/Render/TextureAnimatedDirectional.h"

#include <utility>
#include "../../include/BrakezaSetup.h"

void TextureAnimatedDirectional::LoadAnimationFile(std::string file, int newNumFrames, int newFps, int newMaxTimes)
{
    this->base_file = std::move(file);
    this->numFrames = newNumFrames;
    this->fps = newFps;
    this->maxTimes = newMaxTimes;
}

void TextureAnimatedDirectional::LoadImages()
{
    for (int d = 1; d <= 8; d++) {
        for (int i = 0; i < this->getNumFrames(); i++) {
            std::string file = this->base_file + "/" + std::to_string(d) + "_" + std::to_string(i) + ".png";
            this->frames[d][i] = new Image(file);
        }
    }

    loaded = true;
}

void TextureAnimatedDirectional::LoadImagesForZeroDirection()
{
    for (int i = 0; i < this->getNumFrames(); i++) {
        std::string file = this->base_file + "/" + std::to_string(0) + "_" + std::to_string(i) + ".png";
        this->frames[0][i] = new Image(file);
    }

    loaded = true;
}

int TextureAnimatedDirectional::getNumFrames() const {
    return numFrames;
}

Image *TextureAnimatedDirectional::getCurrentFrame(int direction)
{
    return this->frames[direction][current];
}

void TextureAnimatedDirectional::NextFrame()
{
    current++;

    if (current >= this->getNumFrames()) {
        current = 0;
        times++;
        if (maxTimes != -1 && times >= maxTimes) {
            current = getNumFrames() - 1;
        }
    }
}

void TextureAnimatedDirectional::ImportTextures(const TextureAnimatedDirectional *origin, int numFrames)
{
    for (int d = 0; d <= 8; d++) {
        for (int j = 0; j < numFrames; j++) {
            this->frames[d][j] = origin->frames[d][j];
        }
    }
}

void TextureAnimatedDirectional::DrawImGuiProperties()
{
}

bool TextureAnimatedDirectional::isLoaded() const {
    return loaded;
}

void TextureAnimatedDirectional::setIsZeroDirection(bool value)
{
    isZeroDirection = value;
}

int TextureAnimatedDirectional::getFps() const
{
    return fps;
}

std::string TextureAnimatedDirectional::getBaseFile() const
{
    return base_file;
}

bool TextureAnimatedDirectional::hasZeroDirection() const
{
    return isZeroDirection;
}
