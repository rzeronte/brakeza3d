
#include <utility>

#include "../../include/2D/TextureAnimated.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"

TextureAnimated::TextureAnimated() : numberFramesToLoad(0), currentFrame(0), endAnimation(false), paused(false) {
}


void TextureAnimated::setup(std::string file, int num_frames, int fps)
{
    std::cout << "Importing 2D animation: " << file << std::endl;

    this->base_file = std::move(file);
    this->numberFramesToLoad = num_frames;
    this->fps = fps;

    this->loadImages();
}

void TextureAnimated::loadImages() {
    for (int i = 0; i < this->getNumFrames(); i++) {
        std::string file = this->base_file + "_" + std::to_string(i) + ".png";
        std::cout << "Importing 2D animation file: " << file << std::endl;
        this->frames.push_back(new Texture(file));
    }
}

int TextureAnimated::getNumFrames() const {
    return numberFramesToLoad;
}

Texture *TextureAnimated::getCurrentFrame() {
    return this->frames[currentFrame];
}

void TextureAnimated::nextFrame() {
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

bool TextureAnimated::isEndAnimation() const {
    return endAnimation;
}

void TextureAnimated::setEndAnimation(bool value) {
    TextureAnimated::endAnimation = value;
}

bool TextureAnimated::isPaused() const {
    return paused;
}

void TextureAnimated::setPaused(bool value) {
    TextureAnimated::paused = value;
}

int TextureAnimated::getFps() const {
    return fps;
}

void TextureAnimated::setFps(int value) {
    TextureAnimated::fps = value;
}


