
#include "../../headers/2D/TextureAnimation.h"
#include "../../headers/Render/EngineSetup.h"

TextureAnimation::TextureAnimation()
{
    for (int j = 0; j < ANIMATION2D_MAX_FRAMES ; j++) {
        this->frames[j] = new Texture();
    }
}

void TextureAnimation::setup(std::string file, int num_frames)
{
    this->base_file = file;
    this->numFrames = num_frames;

    this->loadImages();
}

void TextureAnimation::loadImages()
{
    for (int i = 0; i < this->getNumFrames() ; i++) {
        std::string file = EngineSetup::getInstance()->SPRITES_FOLDER + this->base_file + "_" + std::to_string(i) + ".png";
        this->frames[i]->loadTGA( file.c_str(), 1 );
    }
}

int TextureAnimation::getNumFrames() const
{
    return numFrames;
}

Texture *TextureAnimation::getCurrentFrame()
{
    return this->frames[currentFrame];
}

void TextureAnimation::nextFrame()
{
    currentFrame++;

    if (currentFrame >= this->getNumFrames()) {
        currentFrame = 0;
    }
}