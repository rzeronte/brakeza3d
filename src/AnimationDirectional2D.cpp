
#include "../headers/AnimationDirectional2D.h"
#include "../headers/EngineSetup.h"
#include "../headers/Core/Logging.h"

AnimationDirectional2D::AnimationDirectional2D()
{
    for (int d = 0; d < 8 ; d++) {
        for (int j = 0; j < ANIMATION2D_MAX_FRAMES ; j++) {
            this->frames[d][j] = new Texture();
        }
    }
}

void AnimationDirectional2D::setup(std::string file, int num_frames)
{
    this->base_file = file;
    this->n_frames = num_frames;
}

void AnimationDirectional2D::loadImages()
{
    for (int d = 0; d < 8 ; d++) {
        for (int i = 0; i < this->getNumFrames() ; i++) {
            std::string file =  this->base_file + "/" + std::to_string(d+1) + "_" + std::to_string(i) + ".png";
            this->frames[d][i]->loadTGA( file.c_str() );
        }
    }
}

int AnimationDirectional2D::getNumFrames() const
{
    return n_frames;
}

Texture *AnimationDirectional2D::getCurrentFrame(int direction)
{
    return this->frames[direction][current];
}

void AnimationDirectional2D::nextFrame()
{
    current++;

    if (current >= this->getNumFrames()) {
        current = 0;
    }
}