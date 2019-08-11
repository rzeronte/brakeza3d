
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
    this->n_frames = num_frames;

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
    return n_frames;
}

Texture *TextureAnimation::getCurrentFrame()
{
    return this->frames[current];
}

void TextureAnimation::nextFrame()
{
    current++;

    if (current >= this->getNumFrames()) {
        current = 0;
    }
}