
#include "../../headers/2D/Animation2D.h"
#include "../../headers/Render/EngineSetup.h"

Animation2D::Animation2D()
{
    for (int j = 0; j < ANIMATION2D_MAX_FRAMES ; j++) {
        this->frames[j] = new Texture();
    }
}

void Animation2D::setup(std::string file, int num_frames)
{
    this->base_file = file;
    this->n_frames = num_frames;

    this->loadImages();
}

void Animation2D::loadImages()
{
    for (int i = 0; i < this->getNumFrames() ; i++) {
        std::string file = EngineSetup::getInstance()->SPRITES_FOLDER + this->base_file + "_" + std::to_string(i) + ".png";
        this->frames[i]->loadTGA( file.c_str(), 1 );
    }
}

int Animation2D::getNumFrames() const
{
    return n_frames;
}

Texture *Animation2D::getCurrentFrame()
{
    return this->frames[current];
}

void Animation2D::nextFrame()
{
    current++;

    if (current >= this->getNumFrames()) {
        current = 0;
    }
}