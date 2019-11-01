
#include "../../headers/2D/TextureAnimation.h"
#include "../../headers/Render/EngineSetup.h"

TextureAnimation::TextureAnimation() : numFrames(0), currentFrame(0), endAnimation(false), paused(false)
{
    for (int j = 0; j < ANIMATION2D_MAX_FRAMES ; j++) {
        this->frames[j] = new Texture();
    }
}


void TextureAnimation::setup(std::string file, int num_frames, int fps)
{
    this->base_file = file;
    this->numFrames = num_frames;
    this->fps = fps;

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
    setEndAnimation(false);

    if (!isPaused()) {
        currentFrame++;
    }

    // update frame
    if (currentFrame >= this->getNumFrames()) {
        currentFrame = 0;

        // flag for check if we are in end of animation
        setEndAnimation(true);
    }
}

bool TextureAnimation::isEndAnimation() const {
    return endAnimation;
}

void TextureAnimation::setEndAnimation(bool endAnimation) {
    TextureAnimation::endAnimation = endAnimation;
}

bool TextureAnimation::isPaused() const {
    return paused;
}

void TextureAnimation::setPaused(bool paused) {
    TextureAnimation::paused = paused;
}

int TextureAnimation::getFps() const {
    return fps;
}

void TextureAnimation::setFps(int fps) {
    TextureAnimation::fps = fps;
}


